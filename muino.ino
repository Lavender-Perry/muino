#include <LiquidCrystal_I2C.h>

#include "compile_settings.h"

LiquidCrystal_I2C lcd(LCD_I2C, 16, 2);
const byte potentiometer = A2;

void setup() {
    for (byte i = 2; i <= 15; i++)
        pinMode(i, i < 4 ? INPUT : OUTPUT);
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
}

void loop() {
}
