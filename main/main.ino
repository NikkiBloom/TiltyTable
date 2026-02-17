#include <Arduino.h>
#include <FreeRTOS_ARM.h>

// macros - basically a varible definition handles when the code compiles, and will never update.
// all caps is code convention
#define STOPPIN 22
#define GOPIN 23

// Task Handles
TaskHandle_t errorTaskHandle;
TaskHandle_t motorTaskHandle;
TaskHandle_t stateMachineTaskHandle;
TaskHandle_t buttonTaskHandle;
TaskHandle_t dialTaskHandle;
TaskHandle_t displayTaskHandle;

// Shared/Global Variables
// Target angles from dials
volatile double targetX = 0;
volatile double targetY = 0;
uint32_t lcdTimeoutTimer = 0; // only ever set to 0; data sharing not a concern

// Button flags
volatile bool goPressed = false;
volatile bool stopPressed = false;

// System state enum to track states/status/what its doing. 
enum SystemState {
    STATE_OK,
    STATE_MOVING,
    STATE_DONE,
    STATE_STOP,
    STATE_ERROR
};
volatile SystemState systemStatus = STATE_OK;

const char* stateToString(SystemState s) { // for passing to hmi.ino
    switch(s) {
        case STATE_OK:      return "OK";
        case STATE_MOVING:  return "MOVING";
        case STATE_DONE:    return "DONE";
        case STATE_STOP:    return "STOPPING";
        case STATE_ERROR:   return "ERROR";
        default:            return "UNKNOWN";
    }
}


// -----------------------------------------------------
// Task Definitions
// This is the heart of main. Each task runs in parallel, which improves code-flow, speed, and reliability for our system.

// Safety
void ErrorTask(void *pvParameters) {
    for (;;) {
        // deal with motor faults and other errors
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

// Motor Control
void MotorTask(void *pvParameters) {
    for (;;) {
        // move motors toward targetX/targetY
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// State Machine
void StateMachineTask(void *pvParameters) {
    for (;;) {

        // check button changes
        if (goPressed) {
            lcdTimeoutTimer = millis();
            goPressed = false;
            systemStatus = STATE_MOVING;
        }
        if (stopPressed) {
            lcdTimeoutTimer = millis();
            stopPressed = false;
            systemStatus = STATE_STOP;
        }

        // Actual state machine
        switch (systemStatus) {

            case STATE_OK:
                // Idle, waiting for GO
                break;

            case STATE_MOVING:
                // Check if motors reached target
                // if (motorsAtTarget()) {
                //     systemStatus = STATE_DONE;
                // }
                // for debug:
                vTaskDelay(pdMS_TO_TICKS(5000)); // wait 5 seconds to simulate motor movement
                systemStatus = STATE_DONE;
                break;

            case STATE_DONE:
                // Completed movement, waiting for next GO
                vTaskDelay(pdMS_TO_TICKS(1000)); // simulate any tasks we may put here. May remain empty!
                systemStatus = STATE_OK;
                break;

            case STATE_STOP:
                // Freeze all motor movement
                // Wait until user presses GO again
                // for debug:
                vTaskDelay(pdMS_TO_TICKS(2000)); // wait 2 seconds to simulate motor movement
                systemStatus = STATE_DONE;

                break;

            case STATE_ERROR:
                // ErrorTask should set this state
                // Stay here until reset
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

// Buttons
void ButtonTask(void *pvParameters) {
    pinMode(STOPPIN, INPUT_PULLUP); 
    pinMode(GOPIN, INPUT_PULLUP);

    bool lastGo = HIGH;
    bool lastStop = HIGH;

    for (;;) {
        bool goState = digitalRead(GOPIN);
        bool stopState = digitalRead(STOPPIN);

        if (lastGo == HIGH && goState == LOW) {
            goPressed = true;
        }

        if (lastStop == HIGH && stopState == LOW) {
            stopPressed = true;
        }

        lastGo = goState;
        lastStop = stopState;

        vTaskDelay(pdMS_TO_TICKS(10)); // debounce
    }
}

// Dials (lives in hmi.ino)
void DialTask(void *pvParameters) {
    for (;;) {
        targetX = getXDial();
        targetY = getYDial();

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

// Display
void DisplayTask(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(500));
    hminit();

    for (;;) {
        Serial.println("Display task alive");
        setScreen(targetX, targetY, stateToString(systemStatus));
        lcdTimeout(lcdTimeoutTimer);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}


// -----------------------------------------------------
// Setup

void setup() {
    Serial.begin(115200);
    lcdTimeoutTimer = millis();

    // Create tasks
    xTaskCreate(ErrorTask, "Errors", 256, NULL, 3, &errorTaskHandle);
    xTaskCreate(MotorTask, "Motors", 256, NULL, 2, &motorTaskHandle);
    xTaskCreate(StateMachineTask, "State", 256, NULL, 2, &stateMachineTaskHandle);
    xTaskCreate(ButtonTask, "Buttons", 256, NULL, 1, &buttonTaskHandle);
    xTaskCreate(DialTask, "Dials", 256, NULL, 1, &dialTaskHandle);
    xTaskCreate(DisplayTask, "Display", 256, NULL, 1, &displayTaskHandle);

    // Start scheduler
    vTaskStartScheduler();
}

void loop() {} // FreeRTOS takes over; loop() is unused