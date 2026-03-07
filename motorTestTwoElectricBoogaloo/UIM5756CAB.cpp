#include "UIM5756CAB.h"

 

UIM5756CAB::UIM5756CAB(CANRaw &canBus, uint8_t nodeID) : _can(canBus), _nodeID(nodeID) {

    uint32_t sid = ((_nodeID << 1) & 0x003F) | 0x0100;

    _baseID = (sid << 18) | (((_nodeID << 1) & 0x00C0) << 8);

}

 

bool UIM5756CAB::begin(uint32_t bitrate) { return _can.begin(bitrate); }

 

void UIM5756CAB::send(uint8_t cw, uint8_t dl, uint8_t *data) {

    CAN_FRAME frame;

    frame.id = _baseID | (cw | 0x80); // Always request ACK (Bit 7 set)

    frame.extended = true;

    frame.length = dl;

    if (data) for (int i = 0; i < dl; i++) frame.data.bytes[i] = data[i];

    _can.sendFrame(frame);

}

 

// Implementation of Chapter 6 Instructions

void UIM5756CAB::setProtocolParam(uint8_t i, uint8_t n) { uint8_t d[]={i,n}; send(0x01, 2, d); } // 6.1 [5]

void UIM5756CAB::setInitialConfig(uint8_t i, uint16_t n){ uint8_t d[]={i, (uint8_t)n, (uint8_t)(n>>8)}; send(0x06, 3, d); } // 6.2 [6]

void UIM5756CAB::getModel() { send(0x0B, 0, NULL); } // 6.4 [7]

void UIM5756CAB::getSerialNumber() { send(0x0C, 0, NULL); } // 6.5 [8]

void UIM5756CAB::resetErrors() { uint8_t d[]={0,0}; send(0x0F, 2, d); } // 6.6 [9]

void UIM5756CAB::setEncoderParam(uint8_t i, uint16_t n){ uint8_t d[]={i, (uint8_t)n, (uint8_t)(n>>8)}; send(0x3D, 3, d); } // 6.7 [9]

void UIM5756CAB::systemOp(uint8_t i) { uint8_t d[]={i}; send(0x7E, 1, d); } // 6.8 [10]

void UIM5756CAB::setDriverParam(uint8_t i, uint16_t n) { uint8_t d[]={i, (uint8_t)n, (uint8_t)(n>>8)}; send(0x10, 3, d); } // 6.9 [10]

void UIM5756CAB::setPower(bool on) { uint8_t d[]={(uint8_t)(on?1:0)}; send(0x15, 1, d); } // 6.10 [11]

void UIM5756CAB::start() { send(0x16, 0, NULL); } // 6.11 [3]

void UIM5756CAB::stop() { send(0x17, 0, NULL); } // 6.12 [3]

void UIM5756CAB::setAcceleration(uint32_t n) { send(0x19, 4, (uint8_t*)&n); } // 6.14 [3]

void UIM5756CAB::setDeceleration(uint32_t n) { send(0x1A, 4, (uint8_t*)&n); } // 6.15 [12]

void UIM5756CAB::setCutInSpeed(uint32_t n) { send(0x1B, 4, (uint8_t*)&n); } // 6.16 [3]

void UIM5756CAB::setStopDecel(uint32_t n) { send(0x1C, 4, (uint8_t*)&n); } // 6.17 [13]

void UIM5756CAB::setJogVelocity(int32_t n) { send(0x1D, 4, (uint8_t*)&n); } // 6.18 [3]

void UIM5756CAB::setPTPSpeed(uint32_t n) { send(0x1E, 4, (uint8_t*)&n); } // 6.19 [14]

void UIM5756CAB::moveRelative(int32_t n) { send(0x1F, 4, (uint8_t*)&n); } // 6.20 [3]

void UIM5756CAB::moveAbsolute(int32_t n) { send(0x1B, 4, (uint8_t*)&n); } // 6.21 [15]

void UIM5756CAB::setOrigin() { send(0x21, 0, NULL); } // 6.22 [3]

void UIM5756CAB::setSoftwareLimit(uint8_t i, int32_t n) {

    uint8_t d[16]; d[16]=i; memcpy(&d[17], &n, 4); send(0x2C, 5, d); // 6.23 [18]

}

void UIM5756CAB::setBacklash(uint16_t n) { uint8_t d[]={(uint8_t)n, (uint8_t)(n>>8)}; send(0x2D, 2, d); } // 6.24 [19]

void UIM5756CAB::queryStatus(uint8_t i) { uint8_t d[]={i}; send(0x11, 1, d); } // 6.25 [18]

void UIM5756CAB::setInputLogic(uint8_t i, uint16_t n) { uint8_t d[]={i, (uint8_t)n, (uint8_t)(n>>8)}; send(0x34, 3, d); } // 6.27 [20]

void UIM5756CAB::setDigitalOut(uint8_t p, bool l) { uint8_t d[]={p,(uint8_t)(l?1:0),0,0}; send(0x37, 4, d); } // 6.29 [21]

void UIM5756CAB::setQueuedPos(uint8_t r, int32_t p) {

    uint8_t d[16]; d[16]=r; memcpy(&d[17], &p, 4); send(0x25, 5, d); // 6.33 [22]

}

 

void UIM5756CAB::checkMessages() {

    CAN_FRAME f;

    while (_can.available()) {

        _can.read(f);

        uint8_t cw = f.id & 0xFF;

        if (cw == 0x0F) Serial.println("Error Reported!"); // 2.3 [23]

        if (cw == 0x5A) Serial.println("Real-time Alarm!"); // 1.6 [24]

    }

}