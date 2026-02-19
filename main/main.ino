#include <Arduino.h>
#include <FreeRTOS_ARM.h> // https://github.com/greiman/FreeRTOS-Arduino/tree/master
#include <SPI.h>
#include <SCL3300.h>

// macros - basically a varible definition handles when the code compiles, and will never update.
// all caps is code convention
#define STOPPIN 22
#define GOPIN 23
#define RESETPIN 31

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

volatile bool motorsStopped = true; // assume breaks at startup?
volatile double motorX = 0;
volatile double motorY = 0;

// Button flags
volatile bool goPressed = false;
volatile bool stopPressed = false;
volatile bool resetPressed = false;
// wake button

// Inclinometer
SCL3300 inclinometer;
// Default SPI chip/slave select pin is D10

// System state enum to track states/status/what its doing. 
enum SystemState {
    STATE_OK,
    STATE_MOVING,
    STATE_DONE,
    STATE_STOP,
    STATE_ERROR,
    STATE_RESET
};
volatile SystemState systemStatus = STATE_OK;

const char* stateToString(SystemState s) { // for passing to hmi.ino
    switch(s) {
        case STATE_OK:      return "OK";
        case STATE_MOVING:  return "MOVING";
        case STATE_DONE:    return "DONE";
        case STATE_STOP:    return "STOPPING";
        case STATE_ERROR:   return "ERROR";
        case STATE_RESET:   return "RESETTING";
        default:            return "UNKNOWN";
    }
}


// -----------------------------------------------------
// Task Definitions
// This is the heart of main. Each task runs in parallel, which improves code-flow, speed, and reliability for our system.

// Safety
void ErrorTask(void *pvParameters) {
    for (;;) {

        // Reset LCD. Also works as a "wake" function.
        if (resetPressed){
            resetPressed = false;
            // suspend updates during reset
            vTaskSuspend(displayTaskHandle);
            while(hminit() != 1) {
                // nothing until it says "I'm ready"
            }
            // time to gap startup
            //vTaskDelay(pdMS_TO_TICKS(500));
            vTaskResume(displayTaskHandle);
        }
        /*
        if (inclinometer.begin() == false) {
            Serial.println("Murata SCL3300 inclinometer not connected.");
            while(1); //Freeze
        }*/
       
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

// Motor Control
void MotorTask(void *pvParameters) {
    for (;;) {
        if(systemStatus == STATE_STOP){
            // stop motors
            motorsStopped = true;
        }
        else if(systemStatus == STATE_MOVING){
            motorsStopped = false;
            // move motors
            motorsStopped = true;
        }
        // move motors toward targetX/targetY
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

// State Machine
void StateMachineTask(void *pvParameters) {
    for (;;) {
        // check button changes
        if (stopPressed) {
            lcdTimeoutTimer = millis();
            stopPressed = false;
            systemStatus = STATE_STOP;
        }
        else if (goPressed) {
            lcdTimeoutTimer = millis();
            goPressed = false;
            systemStatus = STATE_MOVING;
        }

        // Actual state machine
        switch (systemStatus) {

            case STATE_OK:
                // Idle, waiting for GO
                break;

            case STATE_MOVING:
                // Check if motors reached target
                if(motorsStopped){
                    systemStatus = STATE_DONE;
                }
                break;

            case STATE_DONE:
                // Completed movement
                systemStatus = STATE_OK;
                break;

            case STATE_STOP:
                // Freeze all motor movement
                // Wait until user presses GO again
                // for debug:
                if(motorsStopped){
                    systemStatus = STATE_RESET;
                }

                break;

            case STATE_RESET:
                // runs after STOP to reset any moving variables
                systemStatus = STATE_OK;
                break;

            case STATE_ERROR:
                // ErrorTask should set this state
                // Stay here until reset
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

// Detects and flags button presses
void ButtonTask(void *pvParameters) {
    pinMode(STOPPIN, INPUT_PULLUP); 
    pinMode(GOPIN, INPUT_PULLUP);
    pinMode(RESETPIN, INPUT_PULLUP);

    bool lastGo = HIGH;
    bool lastStop = HIGH;
    bool lastReset = HIGH;

    for (;;) {
        bool goState = digitalRead(GOPIN);
        bool stopState = digitalRead(STOPPIN);
        bool resetState = digitalRead(RESETPIN);

        if (lastGo == HIGH && goState == LOW) {
            goPressed = true;
        }

        if (lastStop == HIGH && stopState == LOW) {
            stopPressed = true;
        }

        if (lastReset == HIGH && resetState == LOW) {
            resetPressed = true;
        }

        lastGo = goState;
        lastStop = stopState;
        lastReset = resetState;

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