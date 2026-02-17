// handles interface with the screen and the knobs
// LCD portions of this file were pulled from https://github.com/TechsPassion/Arduino/blob/main/I2C_LCD.ino


#include <Wire.h> // shows unresolved in VS code; this is fine
#include <Adafruit_GFX.h> //for IDE install : Adafruit GFX
#include <LiquidCrystal_I2C.h> //for IDE install : LiquidCrystal I2C by Frank de Brabander

// Set the address, number of columns, and number of rows of the LCD
const int LCD_ADDRESS = 0x27; // I2C address of the LCD
const int LCD_COLS = 16; // Number of columns of the LCD
const int LCD_ROWS = 2; // Number of rows of the LCD
// Create an instance of the LCD object
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

void hminit(){
    // Initialize the I2C bus
    Wire.begin();
    // Initialize the LCD
    lcd.init();
    lcd.backlight();  
    lcd.clear();
 
}

void setScreen(int xDeg, int yDeg, int status){
    lcd.setCursor(0, 0);
    lcd.print("X:");
    lcd.print(xDeg);

    lcd.setCursor(7, 0);
    lcd.print(" Y:");
    lcd.print(yDeg);

    lcd.setCursor(0, 1);

    // 1: ok | 2: going | 3: stopped (done) | 4: stopped (button) | 5+: error
    if(status == 1) lcd.print("STATUS: OK      ");
    if(status == 2) lcd.print("STATUS: MOVING      ");
    else if(status == 3) lcd.print("STATUS: DONE  ");
    else if(status == 4) lcd.print("STATUS: STOPPED  ");
    else lcd.print("STATUS: ERROR  ");
}


/*
int getKnob1(){
    return knobVal;
}

int getKnob2(){
    return knobVal;
}
*/