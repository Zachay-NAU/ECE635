/* Includes ---------------------------------------------------------------- */
#include <Arduino.h>
// Include Edge Impulse Library
#include <Smart_Doorbell_New_-_Facial_Recognition_inferencing.h> 
#include "edge-impulse-sdk/dsp/image/image.hpp"
#include "esp_camera.h"

/* ----------------------- Configuration Settings ----------------------- */

// --- Camera Model Settings ---
// Select camera model (XIAO ESP32S3)
#define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM

// --- Pin Definitions for Camera (XIAO ESP32S3) ---
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     10
#define SIOD_GPIO_NUM     40
#define SIOC_GPIO_NUM     39
#define Y9_GPIO_NUM       48
#define Y8_GPIO_NUM       11
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       14
#define Y5_GPIO_NUM       16
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM       17
#define Y2_GPIO_NUM       15
#define VSYNC_GPIO_NUM    38
#define HREF_GPIO_NUM     47
#define PCLK_GPIO_NUM     13
#define LED_GPIO_NUM      21 [cite: 2]

// --- Wakeup / PIR Sensor Settings ---
// XIAO ESP32S3 D1 pin corresponds to GPIO 1
#define PIR_PIN 1
// Calculate GPIO 1 bitmask (used for EXT1 wakeup setup)
#define PIR_PIN_MASK (1ULL << PIR_PIN) 

/* ----------------------- Global Variables ----------------------- */

// Edge Impulse Constants
#define EI_CAMERA_RAW_FRAME_BUFFER_COLS           320
#define EI_CAMERA_RAW_FRAME_BUFFER_ROWS           240
#define EI_CAMERA_FRAME_BYTE_SIZE                 3 [cite: 2]

static bool debug_nn = false; // Set to true to see features generated
static bool is_initialised = false;
uint8_t *snapshot_buf; // Points to the capture output [cite: 3]

// Boot counter stored in RTC memory (survives deep sleep)
RTC_DATA_ATTR int bootCount = 0; [cite: 50]

// Camera Configuration
static camera_config_t camera_config = {
    .pin_pwdn = PWDN_GPIO_NUM,
    .pin_reset = RESET_GPIO_NUM,
    .pin_xclk = XCLK_GPIO_NUM,
    .pin_sscb_sda = SIOD_GPIO_NUM,
    .pin_sscb_scl = SIOC_GPIO_NUM,
    .pin_d7 = Y9_GPIO_NUM,
    .pin_d6 = Y8_GPIO_NUM,
    .pin_d5 = Y7_GPIO_NUM,
    .pin_d4 = Y6_GPIO_NUM,
    .pin_d3 = Y5_GPIO_NUM,
    .pin_d2 = Y4_GPIO_NUM,
    .pin_d1 = Y3_GPIO_NUM,
    .pin_d0 = Y2_GPIO_NUM,
    .pin_vsync = VSYNC_GPIO_NUM,
    .pin_href = HREF_GPIO_NUM,
    .pin_pclk = PCLK_GPIO_NUM,
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
    .pixel_format = PIXFORMAT_JPEG, 
    .frame_size = FRAMESIZE_QVGA, 
    .jpeg_quality = 12, 
    .fb_count = 1, 
    .fb_location = CAMERA_FB_IN_PSRAM,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY, 
}; [cite: 4, 5, 6]

/* ----------------------- Function Prototypes ----------------------- */
bool ei_camera_init(void);
void ei_camera_deinit(void);
bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf);
static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr);
void run_inference_single_shot();
void print_wakeup_reason();

/* ----------------------- Main Setup (Runs on Wakeup) ----------------------- */
void setup()
{
    Serial.begin(115200);
    // Wait for serial to stabilize (optional, can be removed for faster boot)
    // while (!Serial); 

    ++bootCount;
    Serial.println("========= System Wakeup =========");
    Serial.println("Boot count: " + String(bootCount)); [cite: 56]

    // 1. Print why we woke up
    print_wakeup_reason(); [cite: 57]

    // 2. Setup PIR Pin
    pinMode(PIR_PIN, INPUT_PULLDOWN); 

    // 3. Check if Wakeup was caused by PIR (Motion)
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    
    // Logic: Only run camera if woken by PIR (EXT1) or if it's the very first boot (Reset)
    if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT1 || wakeup_reason == 0) {
        Serial.println(">>> Motion Detected or First Boot - Starting Inference <<<");
        
        // Initialize Camera
        if (ei_camera_init() == false) {
            ei_printf("Failed to initialize Camera!\r\n");
        } else {
            ei_printf("Camera initialized\r\n");
            
            // Allow camera sensors to settle (Auto Exposure/White Balance)
            // This consumes power but ensures the image isn't too dark/bright.
            ei_sleep(100); 

            // Run Classification ONCE
            run_inference_single_shot();

            // De-initialize camera to save power before sleep
            ei_camera_deinit();
        }
    }

    // 4. Prepare for Sleep
    // Check if PIR is still HIGH. If we sleep while HIGH, we wake immediately.
    // Simple logic: Wait for it to go LOW or just sleep and accept immediate re-trigger.
    // For now, we simply configure the wakeup.

    Serial.println("Configuring EXT1 Wakeup...");
    // Enable wakeup on PIR_PIN (GPIO 1) going HIGH
    esp_sleep_enable_ext1_wakeup(PIR_PIN_MASK, ESP_EXT1_WAKEUP_ANY_HIGH); [cite: 62]

    Serial.println("Going to deep sleep...");
    Serial.flush(); 
    
    // 5. Enter Deep Sleep
    esp_deep_sleep_start();
}

/* ----------------------- Main Loop (Unused) ----------------------- */
void loop()
{
    // This will not run because we enter deep sleep at the end of setup()
}

/* ----------------------- Helper Functions ----------------------- */

/**
 * @brief Logic to capture image and run Edge Impulse classifier once
 */
void run_inference_single_shot() {
    
    // Allocate memory for the snapshot
    snapshot_buf = (uint8_t*)malloc(EI_CAMERA_RAW_FRAME_BUFFER_COLS * EI_CAMERA_RAW_FRAME_BUFFER_ROWS * EI_CAMERA_FRAME_BYTE_SIZE);
    
    if(snapshot_buf == nullptr) {
        ei_printf("ERR: Failed to allocate snapshot buffer!\n");
        return;
    }

    ei::signal_t signal;
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    signal.get_data = &ei_camera_get_data;

    // Capture Image
    if (ei_camera_capture((size_t)EI_CLASSIFIER_INPUT_WIDTH, (size_t)EI_CLASSIFIER_INPUT_HEIGHT, snapshot_buf) == false) {
        ei_printf("Failed to capture image\r\n");
        free(snapshot_buf);
        return;
    }

    // Run the classifier
    ei_impulse_result_t result = { 0 };
    EI_IMPULSE_ERROR err = run_classifier(&signal, &result, debug_nn); [cite: 19]
    if (err != EI_IMPULSE_OK) {
        ei_printf("ERR: Failed to run classifier (%d)\n", err);
        free(snapshot_buf);
        return;
    }

    // Print Predictions
    ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
                result.timing.dsp, result.timing.classification, result.timing.anomaly);

#if EI_CLASSIFIER_OBJECT_DETECTION == 1
    bool bb_found = result.bounding_boxes[0].value > 0;
    for (size_t ix = 0; ix < result.bounding_boxes_count; ix++) {
        auto bb = result.bounding_boxes[ix];
        if (bb.value == 0) continue;
        ei_printf("    %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\n", bb.label, bb.value, bb.x, bb.y, bb.width, bb.height);
    }
    if (!bb_found) ei_printf("    No objects found\n");
#else
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        ei_printf("    %s: %.5f\n", result.classification[ix].label, result.classification[ix].value);
    }
#endif

    // Free memory
    free(snapshot_buf);
}

/**
 * @brief Prints the reason the ESP32 woke up
 */
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:     Serial.println("Wakeup reason: EXT0"); break;
    case ESP_SLEEP_WAKEUP_EXT1:     Serial.println("Wakeup reason: PIR Sensor (EXT1)"); break;
    case ESP_SLEEP_WAKEUP_TIMER:    Serial.println("Wakeup reason: Timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup reason: Touchpad"); break;
    default:                        Serial.printf("Wakeup reason: %d (Reset/Cold Boot)\n", wakeup_reason); break;
  } [cite: 51, 52, 53, 54]
}

// ----------------------- Camera Driver Functions ----------------------- //
// (Standard Edge Impulse Camera Driver Implementations)

bool ei_camera_init(void) {
    if (is_initialised) return true;

    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
      Serial.printf("Camera init failed with error 0x%x\n", err);
      return false;
    }

    // Adjust sensors (Specific to OV3660 or others if needed)
    sensor_t * s = esp_camera_sensor_get();
    if (s->id.PID == OV3660_PID) {
      s->set_vflip(s, 1);
      s->set_brightness(s, 1);
      s->set_saturation(s, 0);
    }
    
    is_initialised = true;
    return true;
}

void ei_camera_deinit(void) {
    esp_err_t err = esp_camera_deinit();
    if (err != ESP_OK) {
        ei_printf("Camera deinit failed\n");
        return;
    }
    is_initialised = false;
    return;
}

bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf) {
    bool do_resize = false;
    if (!is_initialised) {
        ei_printf("ERR: Camera is not initialized\r\n");
        return false;
    }

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        ei_printf("Camera capture failed\n");
        return false;
    }

   bool converted = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, snapshot_buf);
   esp_camera_fb_return(fb);

   if(!converted){
       ei_printf("Conversion failed\n");
       return false;
   }

    if ((img_width != EI_CAMERA_RAW_FRAME_BUFFER_COLS)
        || (img_height != EI_CAMERA_RAW_FRAME_BUFFER_ROWS)) {
        do_resize = true;
    }

    if (do_resize) {
        ei::image::processing::crop_and_interpolate_rgb888(
        out_buf,
        EI_CAMERA_RAW_FRAME_BUFFER_COLS,
        EI_CAMERA_RAW_FRAME_BUFFER_ROWS,
        out_buf,
        img_width,
        img_height);
    }
    return true;
}

static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr)
{
    size_t pixel_ix = offset * 3;
    size_t pixels_left = length;
    size_t out_ptr_ix = 0;

    while (pixels_left != 0) {
        out_ptr[out_ptr_ix] = (snapshot_buf[pixel_ix] << 16) + (snapshot_buf[pixel_ix + 1] << 8) + snapshot_buf[pixel_ix + 2];
        out_ptr_ix++;
        pixel_ix+=3;
        pixels_left--;
    }
    return 0;
}