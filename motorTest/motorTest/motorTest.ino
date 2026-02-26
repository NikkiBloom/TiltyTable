#include "motors.cpp"

bool testComplete = false;
const long TARGET_POSITION = 10000; // 10,000 pulses

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println("Initializing UIM342...");
  motorInit();
  delay(500);

  // 1. Disengage Brake
  // Note: Releasing the brake before enabling the driver is an error [8].
  // Driver was enabled in motorInit (MO=1).
  Serial.println("Disengaging Brake...");
  motorSetBrake(false); 
  delay(200); // Small delay to allow mechanical release

  // 2. Spin Motor
  Serial.print("Spinning to position: ");
  Serial.println(TARGET_POSITION);
  motorSetPos(TARGET_POSITION);
}

void loop() {
  if (!testComplete) {
    uint32_t currentPos = motorGetPos();
    
    if (currentPos != -1) {
      Serial.print("Current Position: ");
      Serial.println(currentPos);

      // Check if we have reached the target
      if (abs(currentPos - TARGET_POSITION) < 10) {
        // 3. Re-engage Brake
        Serial.println("Target reached. Re-engaging Brake...");
        motorSetBrake(true);
        
        testComplete = true;
        Serial.println("Test Sequence Complete.");
      }
    }
    delay(500); // Poll every half second
  }
}