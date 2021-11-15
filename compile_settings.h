#ifndef COMPILE_SETTINGS_H
#define COMPILE_SETTINGS_H

#define LCD_I2C 0x27 // I2C address of the LCD screen

#define DIAL A0 // Pin the potentiometer is on
#define BUTTON 3 // Pin the button is on

#define BUZZER 4 // Pin the buzzer is on

#define FIRST_LIGHT 5 // Pin the first light is on
                      // The rest of the lights will be the 7 pins after
                      // (for example if FIRST_LIGHT is 5 the lights will be 5-12)

#define DEBOUNCE_MS 10 // Millisecond delay required between valid button presses

#endif // COMPILE_SETTINGS_H
