// main operating/running files
#include "hmi.ino"
#include "math.ino"

// ALL digital pins support interrupts, so this is fine
int stopButtonPIN = 22;
int goButtonPIN = 23;

void init(){
    // initialize pins, input variables, general setup

    // setup ISR
    attachInterrupt(digitalPinToInterrupt(goButtonPIN), goButton, FALLING);
    attachInterrupt(digitalPinToInterrupt(stopButtonPIN), stopButton, FALLING);
}

void goButton(knob1, knob2, motor1Pos, motor2Pos){
    double xDeg = getinput(knob1);
    double yDeg = getinput(knob2);

    double newMotorPos1, newMotorPos2 = motorMath(xDeg, yDeg, motor1.getPos(), motor2.getPos());
    
    if(motorsAvailable){ // check motor semaphore
        motor1.set(newMotorPos1);
        motor2.set(newMotorPos1);
    }
}

void stopButton(){
    int motorsAvailable = 0; // set motor semaphore
    motor1.stop();
    motor2.stop();
    int int motorsAvailable = 1;
}

int main(){
    init();

    while(1){
        //loop
        setScreen(xDeg, yDeg, status);
    }

    return;
}