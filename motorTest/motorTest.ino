#include "motors.h"

bool testFinished = false;
const long TEST_TARGET = 10000; // Target pulses

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println("--- UIM342 Test Sequence ---");
  motorInit(); // Turns driver ON (MO=1)
  delay(500);

  Serial.println("Step 1: Disengaging Brake...");
  motorSetBrake(false); 
  delay(500); // Wait for mechanical release

  Serial.print("Step 2: Spinning to ");
  Serial.println(TEST_TARGET);
  motorSetPos(TEST_TARGET);

  testCommunication();
}

void loop() {
  if (!testFinished) {
    long pos = motorGetPos();
    if (pos != -1) {
      Serial.print("Position: ");
      Serial.println(pos);

      // Target reached (within 10 pulse tolerance)
      if (abs(pos - TEST_TARGET) < 10) {
        Serial.println("Step 3: Target Reached. Re-engaging Brake...");
        motorSetBrake(true);
        testFinished = true;
        Serial.println("Test Complete.");
      }
    }
    delay(200);
  }
}