// main operating/running files

// ALL digital pins support interrupts, so this is fine
int stopButtonPIN = 22;
int goButtonPIN = 23;

int xDeg = 20;
int yDeg = 24;
int status = 1;
// 1: ok | 2: going | 3: stopped (done) | 4: stopped (button) | 5+: error
// see hmi.ino

void setup(){
    // initialize pins, input variables, general setup
    pinMode(stopButtonPIN, INPUT_PULLUP);
    pinMode(goButtonPIN, INPUT_PULLUP);

    // run init() function in hmi.imo
    hminit();

    // setup ISR
    attachInterrupt(digitalPinToInterrupt(goButtonPIN), goButton, FALLING);
    attachInterrupt(digitalPinToInterrupt(stopButtonPIN), stopButton, FALLING);
}

// void goButton(knob1, knob2, motor1Pos, motor2Pos){
void goButton(){
    status = 1; // temporarily using this function as a reset

    /*
    double xDeg = getinput(knob1);
    double yDeg = getinput(knob2);

    double newMotorPos1, newMotorPos2 = motorMath(xDeg, yDeg, motor1.getPos(), motor2.getPos());
    
    if(motorsAvailable){ // check motor semaphore
        motor1.set(newMotorPos1);
        motor2.set(newMotorPos1);
    } */
}


void stopButton(){
    status = 4;
    //int motorsAvailable = 0; // set motor semaphore
    //motor1.stop();
    //motor2.stop();
    //int motorsAvailable = 1;
}

void loop(){

    //TODO in the future, set a calibration process
    
    //loop
    setScreen(xDeg, yDeg, status);
    delay(100); // keeps the screen's "queue" from filling too fast
}