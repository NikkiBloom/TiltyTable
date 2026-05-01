// Credit to Russell and his AI skills

// =============================================================

// CL57T Stepper Driver Test — Arduino Due + SN74AHCT125

// =============================================================

// Wiring (via SN74AHCT125 level shifter):

//   Due Pin 2 -> 1A -> 1Y -> CL57T PUL+

//   Due Pin 3 -> 2A -> 2Y -> CL57T DIR+

//   Due Pin 4 -> 3A -> 3Y -> CL57T ENA+

//

// CL57T PUL-, DIR-, ENA- assumed tied to GND.

// SN74AHCT125 xOE pins (1, 4, 10) must be tied to GND.

// =============================================================

 

// --- Pin Definitions ---

const int PIN_PUL = 2;   // Pulse / Step

const int PIN_DIR = 3;   // Direction

const int PIN_ENA = 4;   // Enable

 

// --- Motion Parameters ---

const int    STEPS_PER_MOVE    = 1000;     // steps per move

const unsigned long STEP_PERIOD_MS = 2; // 1 step per second

const unsigned long PAUSE_MS       = 1000; // 1-second dwell

const int    PULSE_WIDTH_US    = 3;       // pulse HIGH time (µs) — CL57T min ~2.5 µs

 

// --- Direction Definitions ---

// Adjust if CW/CCW is reversed on your motor

const int DIR_CW  = HIGH;

const int DIR_CCW = LOW;

 

// --- Enable Logic ---

// CL57T: ENA+ LOW (when ENA- = GND) = motor ENABLED

//        ENA+ HIGH                   = motor DISABLED (free shaft)

const int MOTOR_ENABLE  = LOW;

const int MOTOR_DISABLE = HIGH;

 

// =============================================================

// Helper: send one step pulse

// =============================================================

void stepPulse() {

  digitalWrite(PIN_PUL, HIGH);

  delayMicroseconds(PULSE_WIDTH_US);

  digitalWrite(PIN_PUL, LOW);

}

 

// =============================================================

// Helper: move N steps at a given period (ms per step)

// =============================================================

void moveSteps(int steps, unsigned long periodMs) {

  for (int i = 0; i < steps; i++) {

    unsigned long t0 = millis();

 

    stepPulse();

 

    //Serial.print("  Step ");

    //Serial.print(i + 1);

    //Serial.print(" / ");

    //Serial.println(steps);

 

    // Wait the remainder of the period

    unsigned long elapsed = millis() - t0;

    if (elapsed < periodMs) {

      delay(periodMs - elapsed);

    }

  }

}

 

// =============================================================

void setup() {

  Serial.begin(115200);

  while (!Serial) { ; }  // Wait for Serial Monitor (Due native USB)

 

  // Configure pins

  pinMode(PIN_PUL, OUTPUT);

  pinMode(PIN_DIR, OUTPUT);

  pinMode(PIN_ENA, OUTPUT);

 

  delay(1000);

  // Initialize idle state

  digitalWrite(PIN_ENA, MOTOR_DISABLE);

  digitalWrite(PIN_PUL, LOW);

  digitalWrite(PIN_DIR, LOW);

 

  delay(500);  // Let driver power up

 

  Serial.println("=== CL57T Stepper Test ===");

  Serial.println();

 

  // ---- Enable Motor ----

  digitalWrite(PIN_ENA, MOTOR_ENABLE);

  delay(100);  // ENA setup time

 

  // ---- Move CW: 200 steps @ 1 step/sec ----

  Serial.println(">> Moving CW — 200 steps @ 1 step/sec");

  digitalWrite(PIN_DIR, DIR_CW);

  delay(5);  // Direction setup time (CL57T min ~5 µs, using 5 ms for margin)

  moveSteps(STEPS_PER_MOVE, STEP_PERIOD_MS);

 

  // ---- Dwell 10 seconds ----

  Serial.println(">> Pausing 10 seconds...");

  delay(PAUSE_MS);

 

  // ---- Move CCW: 200 steps @ 1 step/sec ----

  Serial.println(">> Moving CCW — 200 steps @ 1 step/sec");

  digitalWrite(PIN_DIR, DIR_CCW);

  delay(5);  // Direction setup time

  moveSteps(STEPS_PER_MOVE, STEP_PERIOD_MS);

 

  // ---- Disable Motor ----

  //digitalWrite(PIN_ENA, MOTOR_DISABLE);

 

  Serial.println();

  Serial.println("=== Test Complete ===");

}

 

// =============================================================

void loop() {

  // Single-shot — all motion runs in setup()

}