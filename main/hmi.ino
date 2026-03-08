// handles interface with the screen and the knobs
// LCD portions of this file were pulled from https://github.com/TechsPassion/Arduino/blob/main/I2C_LCD.ino


#include <Wire.h> // shows unresolved in VS code; this is fine
#include <Adafruit_GFX.h> // for IDE install : Adafruit GFX
#include <LiquidCrystal_I2C.h> // for IDE install : LiquidCrystal I2C by Frank de Brabander
#include <RotaryEncoder.h> // for IDE install : rotaryEncoder by Matthias Hertel

// Set the address, number of columns, and number of rows of the LCD
const int LCD_ADDRESS = 0x27; // I2C address of the LCD
const int LCD_COLS = 20; // Number of columns of the LCD
const int LCD_ROWS = 4; // Number of rows of the LCD
// Create an instance of the LCD object
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

// Dial setup variables
#DEFINE DIAL1PINA 52 
#DEFINE DIAL1PINB 53 

#DEFINE DIAL2PINA 50 
#DEFINE DIAL2PINB 51 

// Create instance of a clock
uint32_t lastUpdated = 0; // time screen was last updated for screen sleeping in loop()
uint32_t currTime = 0;
bool screenOn = true; // starts on my default
#define TIMEOUT 300000
// #define TIMEOUT 300000 // 5 minutes

int hminit(){
    // Initialize the I2C bus
    Wire.begin();
    delay(50); // fixes lcd startup issues; display did not update
    // Initialize the LCD
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("BOOTING ...");
    // setup current time
    currTime = millis(); 
    lcd.clear();
    return 1;

    encoder = new RotaryEncoder(DIAL1PIN1, DIAL2PIN2, RotaryEncoder::LatchMode::TWO03);
    attachInterrupt(digitalPinToInterrupt(DIAL1PIN1), checkPosition, CHANGE);
    attachInterrupt(digitalPinToInterrupt(DIAL1PIN2), checkPosition, CHANGE);
}

// TODO FOR FAYE
// This is for our 2-line display
// update with more info to fill 4 lines
void setScreen(double XTAR, double YTAR, double XACT, double YACT, const char* statusText){
    lcd.setCursor(0, 0);
    lcd.print("TARGET");

    lcd.setCursor(8, 0);
    lcd.print("ACTUAL");

    lcd.setCursor(0, 1);
    lcd.print("X:");
    lcd.print(XTAR);

    lcd.setCursor(8, 1);
    lcd.print("X:");
    lcd.print(XACT);

    lcd.setCursor(0, 2);
    lcd.print("Y:");
    lcd.print(YTAR);

    lcd.setCursor(8, 2);
    lcd.print("Y:");
    lcd.print(YACT);

    lcd.setCursor(0, 3);
    lcd.print("STATUS: ");
    lcd.print(statusText);
    lcd.print("          ");

}

// turns off the backlight if TIMEOUT time has been passed
void lcdTimeout(uint32_t lastUpdated){
    currTime = millis();
    if(currTime - lastUpdated >= TIMEOUT){
        lcd.noBacklight();
        screenOn = false;
    }
    else{
        lcd.backlight();
        screenOn = true;
    }
}

int getXDial(){
    return 20;
}

int getYDial(){
    return 30;
}
