#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

// RGB LED Pins (PWM-capable pins)
#define GREEN_PIN   11
#define RED_PIN     12
#define BLUE_PIN    13

// FreeRTOS task handles
TaskHandle_t redTaskHandle, greenTaskHandle, blueTaskHandle;

// ======== TASK FUNCTIONS ========
// Red LED fade task
void redTask(void *pvParameters) {
  (void)pvParameters;
  while (1) {
    for (int brightness = 0; brightness <= 255; brightness++) {
      analogWrite(RED_PIN, brightness);
      vTaskDelay(10 / portTICK_PERIOD_MS);  // Smooth fade
    }
    analogWrite(RED_PIN, 0);  // Turn off
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// Green LED blink task
void greenTask(void *pvParameters) {
  (void)pvParameters;
  while (1) {
    digitalWrite(GREEN_PIN, HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    digitalWrite(GREEN_PIN, LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// Blue LED random brightness task
void blueTask(void *pvParameters) {
  (void)pvParameters;
  while (1) {
    int randomBrightness = random(0, 256);
    analogWrite(BLUE_PIN, randomBrightness);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// ======== SETUP & LOOP ========
void setup() {
  Serial.begin(9600);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Create FreeRTOS tasks
  xTaskCreate(redTask, "Red Fade", 128, NULL, 1, &redTaskHandle);
  xTaskCreate(greenTask, "Green Blink", 128, NULL, 1, &greenTaskHandle);
  xTaskCreate(blueTask, "Blue Random", 128, NULL, 1, &blueTaskHandle);

  Serial.println("RGB LED FreeRTOS Demo Started!");
}

void loop() {
  // FreeRTOS handles tasks; loop() is unused
  vTaskDelay(1000 / portTICK_PERIOD_MS);  // Prevent watchdog warnings
}