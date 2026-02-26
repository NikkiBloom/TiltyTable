#include <due_can.h>

// Default settings for UIM342
const uint8_t MOTOR_ID = 5;      // Initial UIM342 Node ID is 5 [3]
const uint8_t MASTER_ID = 4;     // Recommended Master ID for user controllers [2]

/**
 * Helper: Calculates the 29-bit Extended CAN ID required by SimpleCAN3.0.
 * The ID is composed of the Producer ID (Master), Consumer ID (Motor), and Control Word (CW). [2, 4]
 */
uint32_t calculateUimId(uint8_t cw) {
  uint32_t sid = ((MOTOR_ID << 1) & 0x003F) | 0x0100; // ID calculation for SID [1, 4]
  uint32_t eid = (((MOTOR_ID << 1) & 0x00C0) << 8) | cw; // ID calculation for EID [1, 4]
  return (sid << 18) | eid; // Resulting 29-bit CAN-ID [1, 2]
}

/**
 * Initializes CAN0 at 500Kbps and enables the motor driver.
 */
void motorInit() {
  Can0.begin(500000); // UIM342 Initial Bitrate is 500K [3]
  
  CAN_FRAME frame;
  frame.id = calculateUimId(0x95); // CW 0x95 = Enable Motor Driver (MO) [4, 5]
  frame.extended = true;
  frame.length = 1;
  frame.data.byte = 0x01;       // MO=1: Set Driver On [5, 6]
  Can0.sendFrame(frame);
}

/**
 * Sets an absolute target position and begins motion.
 */
void motorSetPos(long pos) {
  CAN_FRAME frame;
  frame.extended = true;

  // 1. Set Absolute Position (PA) - CW 0x20 [7]
  frame.id = calculateUimId(0x20); 
  frame.length = 4;
  frame.data.byte = pos & 0xFF;         // p0: Little-endian format [1, 7]
  frame.data.byte[8] = (pos >> 8) & 0xFF;  // p1
  frame.data.byte[9] = (pos >> 16) & 0xFF; // p2
  frame.data.byte[10] = (pos >> 24) & 0xFF; // p3
  Can0.sendFrame(frame);

  // 2. Begin Motion (BG) - CW 0x16 [6]
  // The controller requires BG to execute the move command [6].
  frame.id = calculateUimId(0x16);
  frame.length = 0; 
  Can0.sendFrame(frame);
}

/**
 * Requests the current absolute position from the controller.
 */
long motorGetPos() {
  CAN_FRAME frame;
  frame.id = calculateUimId(0x20); // Request Absolute Position (PA)
  frame.extended = true;
  frame.length = 0;                // Sending a command with 0 data acts as a "Get" request [7]
  Can0.sendFrame(frame);

  // Poll for response
  uint32_t start = millis();
  while (millis() - start < 100) { // 100ms timeout
    if (Can0.available() > 0) {
      CAN_FRAME incoming;
      Can0.read(incoming);
      
      // The ACK frame contains the 4-byte position data [7, 11]
      if (incoming.length >= 4) {
        long currentPos = (long)incoming.data.byte |
                          ((long)incoming.data.byte[8] << 8) |
                          ((long)incoming.data.byte[9] << 16) |
                          ((long)incoming.data.byte[10] << 24);
        return currentPos;
      }
    }
  }
  return -1; // Timeout or error
}

/**
 * Sends an emergency stop command to the motor.
 */
void motorStop() {
  CAN_FRAME frame;
  frame.id = calculateUimId(0x17); // CW 0x17 = Stop Motion (ST) [6]
  frame.extended = true;
  frame.length = 0; 
  Can0.sendFrame(frame);
}