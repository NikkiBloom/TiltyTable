#include <due_can.h>
#include "motors.h"

//const uint8_t MOTOR_ID = 0;      // Default UIM342 Node ID [4, 5]
//const uint8_t MASTER_ID = 4;     // Recommended Master ID for user controllers [4]

/**
 * Calculates the 29-bit Extended ID required by SimpleCAN3.0 logic [2, 3].
 * SID = ((Consumer_ID << 1) & 0x3F) | 0x0100
 * EID = (((Consumer_ID << 1) & 0xC0) << 8) | Control_Word
 */
uint32_t calculateUimId(uint8_t cw) {
  uint32_t sid = ((MOTOR_ID << 1) & 0x003F) | 0x0100;
  uint32_t eid = (((MOTOR_ID << 1) & 0x00C0) << 8) | cw;
  return (sid << 18) | eid;
}

/* Used to verify connection, will ask for a serial number to get an ACK from the motor. Does not need the driver to be on.*/
void testCommunication() {
  CAN_FRAME frame;
  // SN Inquiry: CW 0x8C requires an ACK
  frame.id = calculateUimId(0x8C); 
  frame.extended = true;
  frame.length = 0; 
  Can0.sendFrame(frame);

  uint32_t timeout = millis();
  while (millis() - timeout < 500) {
    if (Can0.available() > 0) {
      CAN_FRAME response;
      Can0.read(response);
      Serial.println("CAN Communication Verified!");
      Serial.print("Response ID: "); Serial.println(response.id, HEX);
      return;
    }
  }
  Serial.println("Communication Timeout: No response from motor.");
}

/**
 * Initializes CAN bus at 500Kbps and enables the motor driver [6].
 * CW 0x95 is the 'MO' instruction with bit7 set for ACK [3].
 */
void motorInit() {
  Can0.begin(500000); 
  delay(100);
  
  CAN_FRAME frame;
  frame.id = calculateUimId(0x95); 
  frame.extended = true;
  frame.length = 1;
  frame.data.byte[0] = 0x01;  // ✅ MO=1: Set Motor Driver ON [3]
  Can0.sendFrame(frame);
}

/**
 * Controls the internal brake (MT index 5) [7].
 * lock = true (Engage), lock = false (Release).
 * MT instruction requires 3 bytes: d0=index, d1=data, d2=0 [8].
 */
void motorSetBrake(bool lock) {
  CAN_FRAME frame;
  frame.id = calculateUimId(0x90); // CW 0x90: MT with ACK request [8]
  frame.extended = true;
  frame.length = 3;                
  frame.data.byte[0] = 0x05;                       // d0: Index 5 = Internal Controlled Brake [7]
  frame.data.byte[1] = lock ? (uint8_t)0x01 : 0x00; // d1: 1=Engage, 0=Release [7]
  frame.data.byte[2] = 0x00;                        // d2: Fixed to 0
  Can0.sendFrame(frame);
}

/**
 * Sets target absolute position (PA) and starts movement (BG) [11, 12].
 * Positions are 32-bit signed integers in little-endian format [2, 11].
 */
void motorSetPos(long pos) {
  CAN_FRAME frame;
  frame.extended = true;

  // 1. Set Absolute Position (PA, CW 0xA0) [11]
  frame.id = calculateUimId(0xA0); 
  frame.length = 4;
  frame.data.byte[0] = (uint8_t)( pos        & 0xFF);  // p0: LSB
  frame.data.byte[1] = (uint8_t)((pos >> 8)  & 0xFF);  // p1
  frame.data.byte[2] = (uint8_t)((pos >> 16) & 0xFF);  // p2
  frame.data.byte[3] = (uint8_t)((pos >> 24) & 0xFF);  // p3
  Can0.sendFrame(frame);

  // 2. Begin Motion (BG, CW 0x96) [12, 14]
  frame.id = calculateUimId(0x96);
  frame.length = 0; 
  Can0.sendFrame(frame);
}

/**
 * Requests current absolute position. 
 * ACK returns position in little-endian bytes d0-d3 [11].
 */
long motorGetPos() {
  // Send request
  CAN_FRAME frame;
  frame.id = calculateUimId(0xA0); // Request PA [11]
  frame.extended = true;
  frame.length = 0;                
  Can0.sendFrame(frame);

  // Wait for reply (simple polling with 100 ms timeout)
  uint32_t start = millis();
  while (millis() - start < 100) {
    if (Can0.available() > 0) {
      CAN_FRAME incoming;
      Can0.read(incoming);

      // (Optional) verify it's the PA response you expect:
      // if (incoming.id != calculateUimId(0xA0) || !incoming.extended) continue;

      if (incoming.length >= 4) {
        long v = 0;
        v |= (long)incoming.data.byte[0];
        v |= (long)incoming.data.byte[1] << 8;
        v |= (long)incoming.data.byte[2] << 16;
        v |= (long)incoming.data.byte[3] << 24;
        return v;
      }
    }
  }
  return -1;
}

/**
 * Stops motion immediately using the Stop Motion (ST) command [15].
 */
void motorStop() {
  CAN_FRAME frame;
  frame.id = calculateUimId(0x97); // CW 0x97: ST with ACK request
  frame.extended = true;
  frame.length = 0;
  Can0.sendFrame(frame);
}