// handles interface with the screen and the knobs
// LCD portions of this file were pulled from https://github.com/TechsPassion/Arduino/blob/main/I2C_LCD.ino


#include <Wire.h> // shows unresolved in VS code; this is fine
#include <Adafruit_GFX.h> //for IDE install : Adafruit GFX
#include <LiquidCrystal_I2C.h> //for IDE install : LiquidCrystal I2C by Frank de Brabander

// Set the address, number of columns, and number of rows of the LCD
const int LCD_ADDRESS = 0x27; // I2C address of the LCD
const int LCD_COLS = 16; // Number of columns of the LCD
const int LCD_ROWS = 4; // Number of rows of the LCD
// Create an instance of the LCD object
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

// early define so it can be accessed early
void checkPosition();

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
}

// written by Faye! Updates the display with variables passed from main.ino. Called in a loop in main.ino.
void setScreen(int XTAR, int YTAR, double XACT, double YACT, const char* statusText){
    lcd.setCursor(0, 0);
    lcd.print("TARGET");

    lcd.setCursor(8, 0);
    lcd.print("ACTUAL");

    lcd.setCursor(0, 1);
    lcd.print("X:");
    lcd.print(XTAR);
    if (XTAR < 10) lcd.print(" "); 

    lcd.setCursor(8, 1);
    lcd.print("X:");
    if (XACT < 10) lcd.print("  ");
    else if (XACT < 100) lcd.print(" ");
    lcd.print(XACT);
    lcd.setCursor(15, 1);
    lcd.print(" ");

    lcd.setCursor(0, 2);
    lcd.print("Y:");
    lcd.print(YTAR);
    if (YTAR < 10) lcd.print(" "); 

    lcd.setCursor(8, 2);
    lcd.print("Y:");
    if (YACT < 10) lcd.print("  ");
    else if (YACT < 100) lcd.print(" ");
    lcd.print(YACT);
    lcd.setCursor(15, 2);
    lcd.print(" ");

    lcd.setCursor(0, 3);
    lcd.print("STATUS: ");
    lcd.print(statusText);
    lcd.print("          ");

}

void clearLCD(){
    lcd.clear();
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