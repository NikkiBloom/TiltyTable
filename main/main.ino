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

#define PULSE_PIN_X 11
#define DIR_PIN_X 12
#define ENA_PIN_X 13

// todo
#define PULSE_PIN_Y 38
#define DIR_PIN_Y 39
#define ENA_PIN_Y 40

#define PULSE_WIDTH_US 3
#define TOL 0.5  // degrees tolerance on the motor

#define BRAKE_DELAY_MS 2000

#define TESTING 1

const int DIR_CW  = HIGH;
const int DIR_CCW = LOW;

const int MOTOR_ENABLE  = LOW;
const int MOTOR_DISABLE = HIGH;

// const shared by both motors
const unsigned long STEP_PERIOD_MS = 2; // 1 step per second
const unsigned long PAUSE_MS = 1000; // 1-second dwell

// Task Handles
TaskHandle_t errorTaskHandle;
TaskHandle_t motorTaskHandleX;
TaskHandle_t motorTaskHandleY;
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

volatile bool XmotorTargetReached = false;
volatile bool YmotorTargetReached = false;
volatile double motorX = 0;
volatile double motorY = 0;

// Button flags
volatile bool goPressed = false;
volatile bool stopPressed = false;
volatile bool resetPressed = false;

volatile bool stopSteps = false;
// wake button

// Inclinometer
SCL3300 inclinometer;
// Default SPI chip/slave select pin is D10

// System state enum to track states/status/what its doing. 
enum SystemState {
    STATE_OK,       // idle
    STATE_RELEASE_BREAK,
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
        case STATE_RELEASE_BREAK: return "LIFT BRAKE";
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

// helpers for motor tasks
void stepPulseX() {
    digitalWrite(PULSE_PIN_X, HIGH);
    delayMicroseconds(PULSE_WIDTH_US);
    digitalWrite(PULSE_PIN_X, LOW);
}
void stepPulseY() {
    digitalWrite(PULSE_PIN_Y, HIGH);
    delayMicroseconds(PULSE_WIDTH_US);
    digitalWrite(PULSE_PIN_Y, LOW);
}

// Motor Control X
void MotorTaskX(void *pvParameters) {
    for (;;) {
        switch (systemStatus) {
            case STATE_RELEASE_BREAK:
                break;
            case STATE_MOVING:
                if (TESTING) Serial.println("Motor sees STATE_MOVING\n");

                if(!XmotorTargetReached){
                    // typecast to ints for inclinometer sensitivity
                    if( (inclinometerX - targetX) > 0){
                        digitalWrite(DIR_PIN_X, DIR_CW);
                        stepPulseX();
                        vTaskDelay(pdMS_TO_TICKS(STEP_PERIOD_MS));
                    }
                    else if( (inclinometerX - targetX) < 0){
                        digitalWrite(DIR_PIN_X, DIR_CCW);
                        stepPulseX();
                        vTaskDelay(pdMS_TO_TICKS(STEP_PERIOD_MS));
                    }
                }
                // TODO: motor control code here
                // moveMotorToward(targetX, targetY);

                // Check if target reached
                if (fabs(inclinometerX - targetX) < TOL ){
                    XmotorTargetReached = true;
                }
                break;
                

            case STATE_STOP:
                if (TESTING) Serial.println("Motor sees STATE_STOP\n");
                // todo
                // lock motor movement and add a check in state machine
            case STATE_OK:
                if (TESTING) Serial.println("Motor sees STATE_OK\n");
                break;
            case STATE_DONE:
                if (TESTING) Serial.println("Motor sees STATE_DONE\n");
                // Motors should not move
                break;
            default:
                if (TESTING) Serial.println("Motor sees DEFAULT\n");
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

// Motor Control Y
void MotorTaskY(void *pvParameters) {
    for (;;) {
        switch (systemStatus) {
            case STATE_RELEASE_BREAK:
                break;
            case STATE_MOVING:
                if (TESTING) Serial.println("Motor sees STATE_MOVING\n");

                if(!YmotorTargetReached){
                    // typecast to ints for inclinometer sensitivity
                    if( (inclinometerY - targetY) > 0){
                        digitalWrite(DIR_PIN_Y, DIR_CW);
                        stepPulseY();
                        vTaskDelay(pdMS_TO_TICKS(STEP_PERIOD_MS));
                    }
                    else if( (inclinometerY - targetY) < 0){
                        digitalWrite(DIR_PIN_Y, DIR_CCW);
                        stepPulseY();
                        vTaskDelay(pdMS_TO_TICKS(STEP_PERIOD_MS));
                    }
                }
                // TODO: motor control code here
                // moveMotorToward(targetX, targetY);

                // Check if target reached
                if (fabs(inclinometerY - targetY) < TOL ){
                    YmotorTargetReached = true;
                }
                break;
                

            case STATE_STOP:
                if (TESTING) Serial.println("Motor sees STATE_STOP\n");
                //todo
                // lock motor movement and add a check in state machine
            case STATE_OK:
                if (TESTING) Serial.println("Motor sees STATE_OK\n");
                break;
            case STATE_DONE:
                if (TESTING) Serial.println("Motor sees STATE_DONE\n");
                // Motors should not move
                break;
            default:
                if (TESTING) Serial.println("Motor sees DEFAULT\n");
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
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
            XmotorTargetReached = false;
            YmotorTargetReached = false;

            systemStatus = STATE_RELEASE_BREAK;
        }

        switch (systemStatus) {

            case STATE_OK:
                if (TESTING) Serial.println("STATE_OK\n");
                // Idle
                break;

            case STATE_RELEASE_BREAK:
                static TickType_t startTick = 0;

                // First entry into this state
                if (startTick == 0) {
                    startTick = xTaskGetTickCount();
                }

                // Wait until brake delay expires (non-blocking)
                if ((xTaskGetTickCount() - startTick) >= pdMS_TO_TICKS(BRAKE_DELAY_MS)) {
                    startTick = 0;                 // reset for next time
                    systemStatus = STATE_MOVING;   // advance state
                }
                break;

            case STATE_MOVING:
                if (TESTING) Serial.println("STATE_MOVING\n");
                if (XmotorTargetReached && YmotorTargetReached) {
                    systemStatus = STATE_DONE;
                }
                break;

            case STATE_DONE:
                if (TESTING) Serial.println("STATE_DONE\n");
                systemStatus = STATE_RESET;
                // Stay DONE until user action
                break;

            case STATE_STOP:
                if (TESTING) Serial.println("STATE_STOP\n");
                systemStatus = STATE_OK;
                break;

            case STATE_RESET:
                if (TESTING) Serial.println("STATE_RESET\n");
                // Reset system variables here
                XmotorTargetReached = false;
                YmotorTargetReached = false;
                systemStatus = STATE_OK;
                break;

            case STATE_ERROR:
                if (TESTING) Serial.println("STATE_ERROR\n");
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
#define DIAL1PINA 51
#define DIAL1PINB 53
#define DIAL2PINA 50
#define DIAL2PINB 52

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

    // motor inits
    pinMode(PULSE_PIN_X, OUTPUT);
    pinMode(DIR_PIN_X, OUTPUT);
    pinMode(ENA_PIN_X, OUTPUT);
    pinMode(PULSE_PIN_Y, OUTPUT);
    pinMode(DIR_PIN_Y, OUTPUT);
    pinMode(ENA_PIN_Y, OUTPUT);
    delay(100);
    // Initialize idle state for motor
    digitalWrite(ENA_PIN_X, MOTOR_DISABLE);
    digitalWrite(PULSE_PIN_X, LOW);
    digitalWrite(DIR_PIN_X, LOW);
    digitalWrite(ENA_PIN_Y, MOTOR_DISABLE);
    digitalWrite(PULSE_PIN_Y, LOW);
    digitalWrite(DIR_PIN_Y, LOW);
    delay(500);  // Let driver power up
    // ---- Enable Motor ----
    digitalWrite(ENA_PIN_X, MOTOR_ENABLE);
    digitalWrite(ENA_PIN_Y, MOTOR_ENABLE);
    delay(100);  // ENA setup time

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
    xTaskCreate(MotorTaskX, "MotorsX", 256, NULL, 2, &motorTaskHandleX);
    xTaskCreate(MotorTaskY, "MotorsY", 256, NULL, 2, &motorTaskHandleY);
    xTaskCreate(StateMachineTask, "State", 256, NULL, 2, &stateMachineTaskHandle);
    xTaskCreate(ButtonTask, "Buttons", 256, NULL, 1, &buttonTaskHandle);
    xTaskCreate(DialTask, "Dials", 256, NULL, 1, &dialTaskHandle);
    xTaskCreate(DisplayTask, "Display", 256, NULL, 1, &displayTaskHandle);
    xTaskCreate(InclinometerTask, "Inclinometer", 256, NULL, 1, &inclinometerTaskHandle);

    // Start scheduler
    vTaskStartScheduler();
}

void loop() {} // FreeRTOS takes over; loop() is unused