#include <SCL3300.h>

SCL3300 inclinometer;

void setup() {
  // Initialize Serial communication at 9600 baud
  Serial.begin(9600);
  while (!Serial); // Wait for Serial Monitor to open

  // Initialize the sensor (defaults to Chip Select Pin 10)
  if (inclinometer.begin()) {
    Serial.println("SCL3300 sensor initialized successfully.");
  } else {
    Serial.println("Failed to connect to SCL3300. Check your wiring.");
  }
}

void loop() {
  // available() must be used as a conditional to ensure data consistency
  // and handle potential communication errors [2]
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

  // Wait 1000 milliseconds (1 second) before the next reading
  delay(100);
}