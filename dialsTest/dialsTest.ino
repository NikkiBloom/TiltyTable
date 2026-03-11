// written and tested by connor

#include <RotaryEncoder.h>
#include <Arduino.h>

// Define pins on DUE for both dials 
#define DIAL1PINA 52
#define DIAL1PINB 53
#define DIAL2PINA 50
#define DIAL2PINB 51

// A pointer to the dynamic created rotary encoder instance.
// This will be done in setup()
RotaryEncoder *encoder = nullptr;

void checkPosition()
{
  encoder->tick(); // just call tick() to check the state.
}


void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println("InterruptRotator example for the RotaryEncoder library.");

  // setup the rotary encoder functionality

  // use FOUR3 mode when PIN_IN1, PIN_IN2 signals are always HIGH in latch position.
  // encoder = new RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::FOUR3);

  // use FOUR0 mode when PIN_IN1, PIN_IN2 signals are always LOW in latch position.
  // encoder = new RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::FOUR0);

  // use TWO03 mode when PIN_IN1, PIN_IN2 signals are both LOW or HIGH in latch position.
  encoder = new RotaryEncoder(DIAL1PINA, DIAL1PINB, RotaryEncoder::LatchMode::FOUR0);

  // register interrupt routine
  attachInterrupt(digitalPinToInterrupt(DIAL1PINA), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DIAL1PINB), checkPosition, CHANGE);
} // setup()


// Read the current position of the encoder and print out when changed.
void loop()
{
  static int pos = 0;

  encoder->tick(); // just call tick() to check the state.

  int newPos = encoder->getPosition();
  if (pos != newPos) {
    Serial.print("pos:");
    Serial.print(newPos);
    Serial.print(" dir:");
    Serial.println((int)(encoder->getDirection()));
    pos = newPos;
  } // if
} // loop ()