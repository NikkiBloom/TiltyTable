#include <due_can.h>

const uint8_t MOTOR_ID = 5;      
const uint8_t MASTER_ID = 4;     

// Helper to calculate the 29-bit Extended ID required by SimpleCAN3.0 [2, 4]
uint32_t calculateUimId(uint8_t cw) {
  uint32_t sid = ((MOTOR_ID << 1) & 0x003F) | 0x0100;
  uint32_t eid = (((MOTOR_ID << 1) & 0x00C0) << 8) | cw;
  return (sid << 18) | eid;
}

// Fixed: motorInit() uses array indices  for MO=1 [1, 4]
void motorInit() {
  Can0.begin(500000); 
  delay(100);
  
  CAN_FRAME frame;
  frame.id = calculateUimId(0x95); // MO command (Turn motor ON)
  frame.extended = true;
  frame.length = 1;
  frame.data.byte = 0x01;       // MO=1: Motor Driver ON
  Can0.sendFrame(frame);
}

// Fixed: motorSetBrake() uses indices , [5], [6] for MT instruction [7]
void motorSetBrake(bool lock) {
  CAN_FRAME frame;
  frame.id = calculateUimId(0x90); // MT instruction with ACK request
  frame.extended = true;
  frame.length = 3;                
  frame.data.byte = 0x05;       // d0: Index 5 = Internally Controlled Brake
  frame.data.byte[5] = lock ? 0x01 : 0x00; // d1: 1=Engage, 0=Release
  frame.data.byte[6] = 0x00;       // d2: Fixed to 0
  Can0.sendFrame(frame);
}

// Fixed: motorSetPos() uses indices -[8] for 32-bit position [9]
void motorSetPos(long pos) {
  CAN_FRAME frame;
  frame.extended = true;

  // 1. Set Absolute Position (PA, CW 0x20)
  frame.id = calculateUimId(0x20); 
  frame.length = 4;
  frame.data.byte = pos & 0xFF;         // p0: LSB first [1, 9]
  frame.data.byte[5] = (pos >> 8) & 0xFF;  // p1
  frame.data.byte[6] = (pos >> 16) & 0xFF; // p2
  frame.data.byte[8] = (pos >> 24) & 0xFF; // p3
  Can0.sendFrame(frame);

  // 2. Begin Motion (BG, CW 0x16)
  frame.id = calculateUimId(0x16);
  frame.length = 0; 
  Can0.sendFrame(frame);
}

// Fixed: Reading bytes from indices -[8] [3, 10]
long motorGetPos() {
  CAN_FRAME frame;
  frame.id = calculateUimId(0x20); 
  frame.extended = true;
  frame.length = 0;                
  Can0.sendFrame(frame);

  uint32_t start = millis();
  while (millis() - start < 100) {
    if (Can0.available() > 0) {
      CAN_FRAME incoming;
      Can0.read(incoming);
      if (incoming.length >= 4) {
        // Reconstruct 32-bit signed integer from little-endian bytes [3, 10]
        return (long)incoming.data.byte |
               ((long)incoming.data.byte[5] << 8) |
               ((long)incoming.data.byte[6] << 16) |
               ((long)incoming.data.byte[8] << 24);
      }
    }
  }
  return -1;
}

void motorStop() {
  CAN_FRAME frame;
  frame.id = calculateUimId(0x17); // CW 0x17 = Stop Motion (ST)
  frame.extended = true;
  frame.length = 0;
  Can0.sendFrame(frame);
}