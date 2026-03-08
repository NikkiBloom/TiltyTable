#include <Arduino.h>
#include <FreeRTOS_ARM.h> // https://github.com/greiman/FreeRTOS-Arduino/tree/master
#include <SPI.h>
#include <SCL3300.h>
#include <due_can.h>
#include "motors.h"

// macros - basically a varible definition handles when the code compiles, and will never update.
// all caps is code convention
#define STOPPIN 22
#define GOPIN 23
#define RESETPIN 31
#define BUS_SPEED CAN_BPS_500K

#define TESTING 1

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

// Motor

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
            if((int)inclinometer.getCalculatedAngleX() == targetX && 
            (int)inclinometer.getCalculatedAngleY() == targetY) {
                //done!!
                //stop motors
                motorsStopped = true;
            }
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

void InclinometerTask(void *pvParameters) {
    for(;;){
        if (inclinometer.available()) {
            if(TESTING){
                // Print the calculated X and Y tilt angles in degrees [3]
                Serial.print("X Angle: ");
                Serial.print(inclinometer.getCalculatedAngleX());
                Serial.print("° | Y Angle: ");
                Serial.print(inclinometer.getCalculatedAngleY());
                Serial.println("°");
            }
        } else {
            // If data is not available, the library requires a reset [2]
            inclinometer.reset();
            Serial.println("Sensor error detected. Resetting...");
        }
    }
}

// Display
void DisplayTask(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(500));
    hminit();

    Serial.println("Display task alive");

    double inclinometerX = inclinometer.getCalculatedAngleX();
    double inclinometerY = inclinometer.getCalculatedAngleY();

    for (;;) {
        if(TESTING){
            //Serial.print("State: %s\n", stateToString(systemStatus));
            //Serial.print("Inclinometer x: %d\n", inclinometer.getCalculatedAngleX());
            //Serial.print("Inclinometer y: %d\n", inclinometer.getCalculatedAngleY());
        }
        Serial.print("\n");

        inclinometerX = inclinometer.getCalculatedAngleX();
        inclinometerY = inclinometer.getCalculatedAngleY();
        setScreen(targetX, targetY, inclinometerX, inclinometerY, const char* statusText);
        
        lcdTimeout(lcdTimeoutTimer);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}


// -----------------------------------------------------
// Setup

void setup() {
    Serial.begin(115200);
    lcdTimeoutTimer = millis();

    //motorinit();

    if (inclinometer.begin()) {
        Serial.println("SCL3300 sensor initialized successfully.\n");
    } 
    else {
        Serial.println("Failed to connect to SCL3300. Check your wiring.\n");
    }

    if(TESTING) {
        Serial.print("Debug Mode Enabled. Limited Functionality Limited. \n");
    }

    else {
        // Create tasks
        xTaskCreate(InclinometerTask, "Inclinometer Watch", 256, NULL, 7, &errorTaskHandle);
        xTaskCreate(ErrorTask, "Errors", 256, NULL, 1, &errorTaskHandle);
        xTaskCreate(MotorTask, "Motors", 256, NULL, 3, &motorTaskHandle);
        xTaskCreate(StateMachineTask, "State", 256, NULL, 2, &stateMachineTaskHandle);
        xTaskCreate(ButtonTask, "Buttons", 256, NULL, 4, &buttonTaskHandle);
        xTaskCreate(DialTask, "Dials", 256, NULL, 5, &dialTaskHandle);
        xTaskCreate(DisplayTask, "Display", 256, NULL, 6, &displayTaskHandle);

        // Start scheduler
        vTaskStartScheduler();
    }
}

void loop() {
    if(TESTING){
        // Print any CAN frame we see
        CAN_FRAME rx;
        if (Can0.available()) {
            Can0.read(rx);
            Serial.print("[RAW RX] ID=0x"); Serial.print(rx.id, HEX);
            Serial.print(" DLC=");         Serial.print(rx.length);
            Serial.print(" DATA:");
            for (int i=0; i<rx.length; ++i) { Serial.print(' '); Serial.print(rx.data.bytes[i], HEX); }
            Serial.println();
        }

        if (inclinometer.available()) {
            // Print the calculated X and Y tilt angles in degrees [3]
            Serial.print("X Angle: ");
            Serial.print(inclinometer.getCalculatedAngleX());
            Serial.print("° | Y Angle: ");
            Serial.print(inclinometer.getCalculatedAngleY());
            Serial.println("°");
        } else {
            // If data is not available, the library requires a reset [2]
            inclinometer.reset();
            Serial.println("Sensor error detected. Resetting...");
        }
    }
} 