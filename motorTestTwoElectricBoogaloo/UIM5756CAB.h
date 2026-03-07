#ifndef UIM5756CAB_H

#define UIM5756CAB_H

 

#include <Arduino.h>

#include <due_can.h>

 

class UIM5756CAB {

public:

    UIM5756CAB(CANRaw &canBus, uint8_t nodeID);

    bool begin(uint32_t bitrate = CAN_BPS_500K);

 

    // 6.1 - 6.8 System and Protocol

    void setProtocolParam(uint8_t index, uint8_t val);        // PP[i]

    void setInitialConfig(uint8_t index, uint16_t val);       // IC[i]

    void getModel();                                          // ML

    void getSerialNumber();                                   // SN

    void resetErrors();                                       // ER=0

    void setEncoderParam(uint8_t index, uint16_t val);        // QE[i]

    void systemOp(uint8_t op);                                // SY[i] (1=Reboot, 2=Reset)

 

    // 6.9 - 6.10 Motor Driver

    void setDriverParam(uint8_t index, uint16_t val);         // MT[i] (Index 5=Brake)

    void setPower(bool on);                                   // MO

 

    // 6.11 - 6.22 Motion Control

    void start();                                             // BG

    void stop();                                              // ST

    void setAcceleration(uint32_t val);                       // AC

    void setDeceleration(uint32_t val);                       // DC

    void setCutInSpeed(uint32_t val);                         // SS

    void setStopDecel(uint32_t val);                          // SD

    void setJogVelocity(int32_t val);                         // JV

    void setPTPSpeed(uint32_t val);                           // SP

    void moveRelative(int32_t pulses);                        // PR

    void moveAbsolute(int32_t pulses);                        // PA

    void setOrigin();                                         // OG

 

    // 6.23 - 6.26 Status and Limits

    void setSoftwareLimit(uint8_t index, int32_t val);        // LM[i]

    void setBacklash(uint16_t pulses);                        // BL

    void queryStatus(uint8_t type);                           // MS[i] (0=Flags, 1=Pos)

 

    // 6.27 - 6.29 Input/Output

    void setInputLogic(uint8_t index, uint16_t action);       // IL[i]

    void setDigitalOut(uint8_t port, bool level);             // DI

 

    // 6.33 - 6.37 PVT / PT Motion

    void setQueuedPos(uint8_t row, int32_t pos);              // QP[N]

    void setPTData(uint8_t row, int32_t pos, uint16_t time);  // PT[N]

 

    void checkMessages(); // Parser for all ACKs and Errors

 

private:

    CANRaw &_can;

    uint8_t _nodeID;

    uint32_t _baseID;

    void send(uint8_t cw, uint8_t dl, uint8_t *data);

};

 

#endif