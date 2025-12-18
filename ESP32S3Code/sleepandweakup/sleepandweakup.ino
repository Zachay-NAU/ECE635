#include <Arduino.h>

// XIAO ESP32S3 D1 pin corresponds to GPIO 1
#define PIR_PIN 1
// Calculate GPIO 1 bitmask (used for EXT1 wakeup setup)
#define PIR_PIN_MASK (1ULL << PIR_PIN)

RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:     Serial.println("Wakeup reason: EXT0 (Not supported on S3)"); break;
    case ESP_SLEEP_WAKEUP_EXT1:     Serial.println("Wakeup reason: PIR Sensor triggered (EXT1 External Signal)"); break;
    case ESP_SLEEP_WAKEUP_TIMER:    Serial.println("Wakeup reason: Timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup reason: Touchpad"); break;
    default:                        Serial.printf("Wakeup reason: %d (Not deep sleep or Reset)\n", wakeup_reason); break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Wait a bit for serial stability

  ++bootCount;
  Serial.println("========= System Start =========");
  Serial.println("Boot count: " + String(bootCount));

  // 1. Print wakeup reason
  print_wakeup_reason();

  // 2. Initialize pin
  // Use INPUT_PULLDOWN to ensure the pin is low when idle, preventing floating triggers
  pinMode(PIR_PIN, INPUT_PULLDOWN);
  pinMode(LED_BUILTIN, OUTPUT);

  // 3. Execute task
  // Since this is "External Interrupt Wakeup", reaching here means motion was detected
  if (digitalRead(PIR_PIN) == HIGH) {
    Serial.println(">>> Motion Detected! (PIR HIGH) <<<");
    
    // Execute your logic here (e.g., send notification, turn on LED)
    
    // Note: PIR sensors often hold HIGH for seconds (hardware delay).
    // If you sleep immediately while PIR is still HIGH, it will wake up again instantly.
    // You can add a small delay here if you want to limit the frequency of wakeups.
    delay(100); 
  }

  // 4. Setup next wakeup source: External Interrupt (EXT1)
  // Arg 1: Pin bitmask (GPIO 1)
  // Arg 2: Trigger mode (ESP_EXT1_WAKEUP_ANY_HIGH means wakeup on ANY high pin)
  esp_sleep_enable_ext1_wakeup(PIR_PIN_MASK, ESP_EXT1_WAKEUP_ANY_HIGH);
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);  
  Serial.println("Going to deep sleep, waiting for next PIR signal...");
  Serial.flush(); 
  
  // 5. Enter deep sleep
  esp_deep_sleep_start();
  
  Serial.println("This will never be printed");
}

void loop() {
  // Loop does not execute in deep sleep mode
}