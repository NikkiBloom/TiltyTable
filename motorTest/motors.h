#pragma once
#include <Arduino.h>
#include <due_can.h>

// If you want these IDs available to other files, constexpr here is fine (internal linkage in C++11).
// Adjust as needed.
constexpr uint8_t MOTOR_ID  = 0;  // Default UIM342 Node ID
constexpr uint8_t MASTER_ID = 4;  // Recommended Master ID

// API
uint32_t calculateUimId(uint8_t cw);
void motorInit();
void motorSetBrake(bool lock);
void motorSetPos(long pos);
long motorGetPos();
void motorStop();
void testCommunication();