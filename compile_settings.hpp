#ifndef COMPILE_SETTINGS_H
#define COMPILE_SETTINGS_H

#define LCD_I2C 0x27 // I2C address of the LCD screen

#define BUZZER 4 // Pin the buzzer is on

#define FIRST_LIGHT 5 // Pin the first light is on
                      // The rest of the lights will be the 7 pins after
                      // (for example if FIRST_LIGHT is 5 the lights will be 5-12)

#define MAX_NOTE_AMT 256 // Maximum amount of notes in a melody

#define DEBOUNCE_MS 10 // Millisecond delay required between valid button presses

#define HAS_EEPROM // If your board does not have EEPROM
                   // or you don't want melody saving features
                   // remove or comment out this line.
#define HAS_CONTROLS // If you have not set up the 2 buttons & potentiometer,
                     // remove or comment out this line.

#ifdef HAS_CONTROLS
const int buttons[2] = { 2, 3 }; // Pins the buttons are on
#define DIAL A0 // Pin the potentiometer is on
#endif // HAS_CONTROLS

#endif // COMPILE_SETTINGS_H
