#include <Arduino.h>
#include <Arduino_FreeRTOS.h>  // Include the FreeRTOS library for Arduino

// Define pins for RGB LED components - PWM capable pins for variable brightness
#define GREEN_PIN   11  
#define RED_PIN     12  
#define BLUE_PIN    13  

// Define pins for buttons - using internal pull-up resistors
#define BUTTON_RED_PIN     6
#define BUTTON_GREEN_PIN   7  
#define BUTTON_BLUE_PIN    8  

// Declare FreeRTOS task handles - used to control task states (suspend/resume)
TaskHandle_t redTaskHandle, greenTaskHandle, blueTaskHandle, buttonHandlerTaskHandle;

// Set debounce time for buttons to avoid multiple triggers from a single press
// Convert 50ms to FreeRTOS ticks for timing consistency
const TickType_t debounceDelay = pdMS_TO_TICKS(50);

// Define a structure to hold button state information
struct ButtonInfo {
    volatile TickType_t lastEdgeTime;  // Time of last button state change (volatile for ISR access)
    uint8_t pin;                       // The Arduino pin number the button is connected to
    bool suspended;                    // Tracks whether the corresponding LED task is suspended
};

// Create instances of the ButtonInfo structure for each button
ButtonInfo redButton = {0, BUTTON_RED_PIN, false};
ButtonInfo greenButton = {0, BUTTON_GREEN_PIN, false};
ButtonInfo blueButton = {0, BUTTON_BLUE_PIN, false};

// Forward declarations for button interrupt handler functions
void handleRedButton();
void handleGreenButton();
void handleBlueButton();

// ======== INTERRUPT SERVICE ROUTINES ========
// Function prototype for the common button interrupt handler
void handleButtonInterrupt(ButtonInfo* button);

/**
  * This function processes button press events with debouncing
 * and passes info to the buttonHandlerTask using task notifications
 */
void handleButtonInterrupt(ButtonInfo* button) {
    // Get current time in FreeRTOS ticks (safe in ISR context)
    TickType_t now = xTaskGetTickCountFromISR();
    
    // Check if enough time has passed since last interrupt (debouncing)
    if ((now - button->lastEdgeTime) >= debounceDelay) {
        button->lastEdgeTime = now;  // Update the last edge time
        
        // Create notification value: upper 16 bits = pin number, lower 16 bits = pin state
        uint32_t notificationValue = (button->pin << 16) | digitalRead(button->pin);
        
        // Variable to check if a higher priority task was woken
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        
        // Send notification to button handler task with button info
        xTaskNotifyFromISR(buttonHandlerTaskHandle, notificationValue, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
        
        // If a higher priority task was woken, yield to it immediately
        if (xHigherPriorityTaskWoken == pdTRUE) {
            portYIELD_FROM_ISR();
        }
    }
}

// Button-specific interrupt handlers that call the common handler
void handleRedButton() { 
    handleButtonInterrupt(&redButton);  
}

void handleGreenButton() { 
    handleButtonInterrupt(&greenButton);  
}

void handleBlueButton() { 
    handleButtonInterrupt(&blueButton);  
}

// ======== TASK FUNCTIONS ========
/**
 * Red LED control task - Creates a fade-in effect followed by a pause
 */
void redTask(void *pvParameters) {
    while (1) {  // Infinite loop for task execution
        // Fade in the red LED from 0 to full brightness
        for (int brightness = 0; brightness <= 255; brightness++) {
            analogWrite(RED_PIN, brightness);  // Set LED brightness
            vTaskDelay(10 / portTICK_PERIOD_MS);  // Small delay for visible fade effect
        }
        analogWrite(RED_PIN, 0);  // Turn LED off
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Wait 1 second before next cycle
    }
}

/**
 * Green LED control task - Creates a simple blinking pattern
 */
void greenTask(void *pvParameters) {
    while (1) {  // Infinite loop for task execution
        digitalWrite(GREEN_PIN, HIGH);  // Turn LED on
        vTaskDelay(500 / portTICK_PERIOD_MS);  // Keep on for 500ms
        digitalWrite(GREEN_PIN, LOW);   // Turn LED off
        vTaskDelay(500 / portTICK_PERIOD_MS);  // Keep off for 500ms
    }
}

/**
 * Blue LED control task - Sets random brightness levels
 */
void blueTask(void *pvParameters) {
    while (1) {  // Infinite loop for task execution
        analogWrite(BLUE_PIN, random(0, 256));  // Set LED to random brightness (0-255)
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Wait 1 second before changing brightness
    }
}

/**
 * Button handler task - Processes button events from interrupt notifications
 * Controls task suspension/resumption based on button presses
 */
void buttonHandlerTask(void *pvParameters) {
    uint32_t notificationValue;  // Will hold the button information
    
    while (1) {  // Infinite loop for task execution
        // Wait indefinitely for a notification from an interrupt handler
        if (xTaskNotifyWait(0, 0, &notificationValue, portMAX_DELAY) == pdPASS) {
            // Extract button pin and state from notification value
            uint8_t buttonPin = notificationValue >> 16;  // Upper 16 bits = pin number
            uint8_t state = notificationValue & 0xFF;     // Lower bits = pin state
            
            // Only process button press events (LOW due to pull-up configuration)
            if (state == LOW) {  // Button pressed (falling edge)
                // Handle different buttons
                if (buttonPin == BUTTON_RED_PIN) {
                    // Toggle between suspending and resuming the red LED task
                    redButton.suspended ? vTaskResume(redTaskHandle) : vTaskSuspend(redTaskHandle);
                    redButton.suspended = !redButton.suspended;  // Update suspension state
                    analogWrite(RED_PIN, 0);  // Turn off LED when task state changes
                }
                else if (buttonPin == BUTTON_GREEN_PIN) {
                    // Toggle between suspending and resuming the green LED task
                    greenButton.suspended ? vTaskResume(greenTaskHandle) : vTaskSuspend(greenTaskHandle);
                    greenButton.suspended = !greenButton.suspended;  // Update suspension state
                    digitalWrite(GREEN_PIN, LOW);  // Turn off LED when task state changes
                }
                else if (buttonPin == BUTTON_BLUE_PIN) {
                    // Toggle between suspending and resuming the blue LED task
                    blueButton.suspended ? vTaskResume(blueTaskHandle) : vTaskSuspend(blueTaskHandle);
                    blueButton.suspended = !blueButton.suspended;  // Update suspension state
                    analogWrite(BLUE_PIN, 0);  // Turn off LED when task state changes
                }
            }
        }
    }
}

/**
 * Arduino setup function - runs once at startup
 * Initializes hardware and FreeRTOS tasks
 */
void setup() {
    Serial.begin(9600);  // Initialize serial communication for debugging
    
    // Configure LED pins as outputs
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);

    // Configure button pins as inputs with internal pull-up resistors enabled
    pinMode(BUTTON_RED_PIN, INPUT_PULLUP);
    pinMode(BUTTON_GREEN_PIN, INPUT_PULLUP);
    pinMode(BUTTON_BLUE_PIN, INPUT_PULLUP);

    // Create FreeRTOS tasks with appropriate stack sizes and priorities
    // Button handler has higher priority (2) than LED tasks (1)
    xTaskCreate(buttonHandlerTask, "Button Handler", 256, NULL, 2, &buttonHandlerTaskHandle);
    xTaskCreate(redTask, "Red Fade", 128, NULL, 1, &redTaskHandle);
    xTaskCreate(greenTask, "Green Blink", 128, NULL, 1, &greenTaskHandle);
    xTaskCreate(blueTask, "Blue Random", 128, NULL, 1, &blueTaskHandle);

    // Attach interrupt handlers to button pins after task creation
    // CHANGE mode detects both button press and release
    attachInterrupt(digitalPinToInterrupt(BUTTON_RED_PIN), handleRedButton, CHANGE);
    attachInterrupt(digitalPinToInterrupt(BUTTON_GREEN_PIN), handleGreenButton, CHANGE);
    attachInterrupt(digitalPinToInterrupt(BUTTON_BLUE_PIN), handleBlueButton, CHANGE);

    Serial.println("RGB LED FreeRTOS Demo with Interrupt Buttons Started!");
}

/**
 * Arduino loop function - normally the main program would run here
 * With FreeRTOS, this function doesn't need to do much as tasks handle functionality
 */
void loop() {
    // Not much needed in loop when using FreeRTOS
    // Just delay to prevent watchdog issues
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}