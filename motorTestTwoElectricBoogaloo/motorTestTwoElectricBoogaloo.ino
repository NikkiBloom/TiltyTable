#include <due_can.h>

#include "UIM5756CAB.h"

 

UIM5756CAB motor(Can0, 5);

 

void setup() {

  Serial.begin(115200);

  motor.begin();

 

  // 1. Initial Configuration for Heavy Load

  motor.setInitialConfig(1, 0);   // IC[17]=0: Positive direction is CW [6]

  motor.setInitialConfig(6, 1);   // IC[27]=1: Use Closed-loop control for accuracy [28]

  motor.setInitialConfig(8, 1);   // IC[29]=1: Enable Brake Logic [6]

 

  // 2. Set Driver & Safety Parameters

  motor.setDriverParam(0, 16);    // MT=16: 16 micro-steps (3200 pulses/rev) [30]

  motor.setDriverParam(1, 40);    // MT[17]=40: Set current to 4.0A for high torque [30]

  motor.setStopDecel(10000);      // SD: High deceleration to stop heavy inertia [13]

 

  // 3. Set Software Limits (Safety)

  motor.setSoftwareLimit(2, 45000); // LM[31]: Upper limit slightly past 90 degrees [26]

  motor.setSoftwareLimit(1, -5000); // LM[17]: Lower limit slightly past 0 degrees [26]

 

  // 4. Operation Sequence

  motor.setPower(true);           // MO=1: Power on [11]

  motor.setDriverParam(5, 0);     // MT[16]=0: Release Brake [25]

 

  motor.setPTPSpeed(800);         // SP: 1/4 rev/sec (at motor shaft) [14]

 

  Serial.println("Moving to 90 Degrees (Vertical)...");

  motor.moveAbsolute(40000);      // PA=40000: Rotate output 90° [32]

  motor.start();                  // BG: Begin movement [33]

 

  delay(10000); // Wait for move

 

  motor.setDriverParam(5, 1);     // MT[16]=1: Engage Brake to hold heavy load [25]

  Serial.println("Load Secured at 90.");

}

 

void loop() {

  motor.checkMessages(); // Parse real-time stall or error notifications [24]

}