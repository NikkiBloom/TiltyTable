#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Change this if your LCD uses a different I2C address
#define LCD_ADDRESS 0x27
#define LCD_COLS 20
#define LCD_ROWS 4

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

void setup() {
    Serial.begin(115200);
    delay(500);  // allow USB Serial to start

    Serial.println("Starting LCD test...");

    Wire.begin();
    delay(100);

    lcd.init();
    lcd.backlight();

    Serial.println("LCD initialized.");

    lcd.setCursor(0, 0);
    lcd.print("20x4 LCD TEST");

    lcd.setCursor(0, 1);
    lcd.print("Line 2 OK");

    lcd.setCursor(0, 2);
    lcd.print("Line 3 OK");

    lcd.setCursor(0, 3);
    lcd.print("Line 4 OK");
}

void loop() {
    // Blink backlight so you know it's alive
    lcd.backlight();
}