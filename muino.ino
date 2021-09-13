#include <LiquidCrystal_I2C.h>

#include "compile_settings.h"

void setLCDPlayingStatus(byte playing);
void lightOn(byte light);
void buttonISR();

LiquidCrystal_I2C lcd(LCD_I2C, 16, 2);
int notes[8]; // Frequencies of notes in the melody
volatile bool buttonPressed = false;

const byte pausechar[8] = {
    B11011,
    B11011,
    B11011,
    B11011,
    B11011,
    B11011,
    B11011,
    B11011
};
const byte playchar[8] = {
    B10000,
    B11000,
    B11100,
    B11110,
    B11110,
    B11100,
    B11000,
    B10000
};

void setup() {
    pinMode(2, INPUT);
    for (byte i = 4; i <= 12; i++)
        pinMode(i, OUTPUT);

    lcd.init();
    lcd.createChar(0, pausechar);
    lcd.createChar(1, playchar);
    lcd.backlight();
    lcd.home();
    lcd.print("Note 1:");


    int frequency;
    char* freq_str;

    attachInterrupt(digitalPinToInterrupt(3), buttonISR, FALLING);

    for (byte i = 0; i < 8; i++) {
        lcd.setCursor(0, 1);

        while (!buttonPressed) { // Until interrupt
            frequency = analogRead(DIAL) + 31;
            sprintf(freq_str, "%.4d", frequency);
            lcd.print(freq_str);
            lcd.setCursor(0, 1);
        }
        buttonPressed = false;

        notes[i] = frequency;
        lcd.setCursor(5, 0);
        lcd.print(String(i + 2));
    }

    lcd.clear();
    lcd.home();
    lcd.print("BPM:");
    setLCDPlayingStatus(1);
}

void loop() {
    for (byte i = 0; i < 8; i++) {
        /* Check if pause button pressed */
        if (buttonPressed) {
            setLCDPlayingStatus(0);
            // Wait until pressed again
            buttonPressed = false;
            while (!buttonPressed);
            buttonPressed = false;
            setLCDPlayingStatus(1);
        }

        /* Update speed */
        int noteDuration = 5002 - analogRead(DIAL);
        int notePause = noteDuration * 1.3;
        char* bpm;
        sprintf(bpm, "%.3d", 30 * 100 / noteDuration);
        lcd.setCursor(5, 0);
        lcd.print(bpm);

        /* Play note */
        tone(4, notes[i], noteDuration);
        lightOn(i);
        delay(notePause);
    }
}

/* Prints the status on the second row of the LCD */
void setLCDPlayingStatus(byte playing) {
    lcd.setCursor(0, 1);
    lcd.write(playing);
    lcd.setCursor(2, 1);
    lcd.print(playing ? "Playing" : "Paused");
}

/* Turns off the light previously given to the function if required, turns on the one
 * given. Lights are referred to as 0-7. */
void lightOn(byte light) {
    static byte previous = 0;

    if (previous)
        digitalWrite(previous, LOW);

    previous = light + 5;
    digitalWrite(previous, HIGH);
}

void buttonISR() {
    buttonPressed = true;
}
