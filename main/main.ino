#include <Arduino.h>
#include <FreeRTOS_ARM.h> // https://github.com/greiman/FreeRTOS-Arduino/tree/master
#include <SPI.h>
#include <SCL3300.h>
#include <RotaryEncoder.h> // for IDE install : rotaryEncoder by Matthias Hertel

// macros - basically a varible definition handles when the code compiles, and will never update.
// all caps is code convention
#define STOPPIN 22
#define GOPIN 23
#define RESETPIN 31

#define TESTING 1

// Task Handles
TaskHandle_t errorTaskHandle;
TaskHandle_t motorTaskHandle;
TaskHandle_t stateMachineTaskHandle;
TaskHandle_t buttonTaskHandle;
TaskHandle_t dialTaskHandle;
TaskHandle_t displayTaskHandle;
TaskHandle_t inclinometerTaskHandle;

// Shared/Global Variables
// Target angles from dials
volatile int targetX = 0;
volatile int targetY = 0;

volatile double inclinometerX = 0;
volatile double inclinometerY = 0;
volatile double roundedX = 0; // rounded variables for printing
volatile double roundedY = 0;

uint32_t lcdTimeoutTimer = 0; // only ever set to 0; data sharing not a concern

volatile bool motorsStopped = true; // assume breaks at startup?
volatile bool motorTargetReached = false; // assume breaks at startup?
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
    STATE_OK,       // idle
    STATE_MOVING,   // motors moving to target
    STATE_DONE,     // target reached
    STATE_STOP,     // user-stop
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

int hminit();
void setScreen(int XTAR, int YTAR, double XACT, double YACT, const char* statusText);
void clearLCD();
void lcdTimeout(uint32_t lastUpdated);

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
            int start = millis();
            while(hminit() != 1) {
                // nothing until it says "I'm ready"
                if((millis() - start) >= 3000){
                    Serial.print("hminit() timed out in Error Task during reset.\n");
                    break;
                }
            }
            // time to gap startup
            //vTaskDelay(pdMS_TO_TICKS(500));
            vTaskResume(displayTaskHandle);
        }
       
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

// Motor Control
void MotorTask(void *pvParameters) {
    const double TOL = 0.5;  // degrees tolerance
    for (;;) {
        switch (systemStatus) {
            case STATE_MOVING:
                // Motors are actively moving
                motorsStopped = false;

                // TODO: motor control code here
                // moveMotorToward(targetX, targetY);

                // Check if target reached
                if (fabs(inclinometerX - targetX) < TOL &&
                    fabs(inclinometerY - targetY) < TOL) {
                    motorTargetReached = true;
                    motorsStopped = true;
                }
                if(TESTING){
                    // simulate movement for LCD test
                    vTaskDelay(pdMS_TO_TICKS(2000));
                    motorsStopped = true;
                    motorTargetReached = true;
                    break;
                }
                break;

            case STATE_STOP:
                // hault motor movement
                if(TESTING){
                    // simulate movement for LCD test
                    vTaskDelay(pdMS_TO_TICKS(2000));
                    motorsStopped = true;
                    motorTargetReached = true;
                    break;
                }
            case STATE_OK:
            case STATE_DONE:
                // Motors should not move
                motorsStopped = true;
                break;
            default:
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// State Machine
void StateMachineTask(void *pvParameters) {
    for (;;) {

        /* --- Button handling --- */

        if (resetPressed) {
            resetPressed = false;
            systemStatus = STATE_RESET;
        }

        if (stopPressed) {
            stopPressed = false;
            systemStatus = STATE_STOP;
        }

        if (goPressed) {
            goPressed = false;

            // Prepare for motion
            motorTargetReached = false;
            motorsStopped = false;

            systemStatus = STATE_MOVING;
        }

        switch (systemStatus) {

            case STATE_OK:
                // Idle
                break;

            case STATE_MOVING:
                if (motorTargetReached) {
                    systemStatus = STATE_DONE;
                }
                break;

            case STATE_DONE:
                // Stay DONE until user action
                break;

            case STATE_STOP:
                if(motorsStopped){
                    systemStatus = STATE_OK;
                }
                break;

            case STATE_RESET:
                // Reset system variables here
                motorTargetReached = false;
                motorsStopped = true;
                systemStatus = STATE_OK;
                break;

            case STATE_ERROR:
                // Wait for reset
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
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

        vTaskDelay(pdMS_TO_TICKS(15)); // debounce
    }
}

// Dial setup variables
#define DIAL1PINA 52
#define DIAL1PINB 53
#define DIAL2PINA 50
#define DIAL2PINB 51

RotaryEncoder *xEncoder = nullptr;
RotaryEncoder *yEncoder = nullptr;
// helper for DialTask on init
void checkPosition(){
    xEncoder->tick(); // just call tick() to check the state.
    yEncoder->tick();
}
// Dials (lives in hmi.ino)
void DialTask(void *pvParameters) {
    // Initialize dials
    xEncoder = new RotaryEncoder(DIAL1PINB, DIAL1PINA, RotaryEncoder::LatchMode::FOUR0);
    yEncoder = new RotaryEncoder(DIAL2PINB, DIAL2PINA, RotaryEncoder::LatchMode::FOUR0);
    
    // loop
    for (;;) {
        checkPosition();
        targetX = (xEncoder->getPosition());
        targetY = (yEncoder->getPosition());

        if(targetX > 90) xEncoder->setPosition(90);
        if(targetY > 90) yEncoder->setPosition(90);

        if(targetX < 0) xEncoder->setPosition(0);
        if(targetY < 0) yEncoder->setPosition(0);

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

// Display
void DisplayTask(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(500));
    hminit();

    for (;;) {
        //Serial.println("Display task alive");
        setScreen(targetX, targetY, roundedX, roundedY, stateToString(systemStatus));
        lcdTimeout(lcdTimeoutTimer);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void InclinometerTask(void *pvParameters) {
    int oldX = 0;
    int oldY = 0;
    for(;;){
        inclinometerX = inclinometer.getCalculatedAngleX();
        inclinometerY = inclinometer.getCalculatedAngleY();

        // round to tenths
        roundedX = round(inclinometerX * 10.0) / 10.0;
        roundedY = round(inclinometerY * 10.0) / 10.0;
        
        if(TESTING && (roundedX != oldX || roundedY != oldY)){
            oldX = roundedX;
            oldY = roundedY;
            // Print the calculated X and Y tilt angles in degrees [3]
            // Serial.print("X Angle: ");
            // Serial.print(inclinometer.getCalculatedAngleX());
            // Serial.print("° | Y Angle: ");
            // Serial.print(inclinometer.getCalculatedAngleY());
            // Serial.println("°");
        }
        if(!inclinometer.available()){
            inclinometer.reset();
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// -----------------------------------------------------
// Setup

void setup() {
    Serial.begin(115200);
    lcdTimeoutTimer = millis();

    if (inclinometer.begin()) {
        Serial.println("SCL3300 sensor initialized successfully.\n");
    } 
    else {
        Serial.println("Failed to connect to SCL3300. Check your wiring.\n");
    }

    if(TESTING) {
        Serial.print("Debug Mode Enabled. \n");
    }

    // Create tasks
    xTaskCreate(ErrorTask, "Errors", 256, NULL, 3, &errorTaskHandle);
    xTaskCreate(MotorTask, "Motors", 256, NULL, 2, &motorTaskHandle);
    xTaskCreate(StateMachineTask, "State", 256, NULL, 2, &stateMachineTaskHandle);
    xTaskCreate(ButtonTask, "Buttons", 256, NULL, 1, &buttonTaskHandle);
    xTaskCreate(DialTask, "Dials", 256, NULL, 1, &dialTaskHandle);
    xTaskCreate(DisplayTask, "Display", 256, NULL, 1, &displayTaskHandle);
    xTaskCreate(InclinometerTask, "Inclinometer", 256, NULL, 1, &displayTaskHandle);

    // Start scheduler
    vTaskStartScheduler();
}

void loop() {} // FreeRTOS takes over; loop() is unused