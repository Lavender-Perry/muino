#include <LiquidCrystal_I2C.h>

#include "compile_settings.h"
#include "lights.h"

LiquidCrystal_I2C lcd(LCD_I2C, 16, 2);

void setup() {
    for (byte i = 2; i <= 12; i++)
        pinMode(i, i < 4 ? INPUT : OUTPUT);

    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
}

void loop() {
    // TESTING
    for (byte i = 0; i <= 8; i++) {
        lightOn(i);
        delay(1000);
    }
}
