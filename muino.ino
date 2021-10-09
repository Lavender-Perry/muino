#include <LiquidCrystal_I2C.h>

#include "compile_settings.h"

int averagedDialRead();
bool buttonPressed();
void setLCDPlayingStatus(byte playing);
void lightOn(byte light);
void buttonISR();

LiquidCrystal_I2C lcd(LCD_I2C, 16, 2);
int notes[8]; // Frequencies of notes in the melody
volatile bool isr = false; // If ISR has occurred since last button read

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
    pinMode(BUTTON, INPUT);
    for (byte i = 4; i <= 12; i++)
        pinMode(i, OUTPUT);

    lcd.init();
    lcd.createChar(0, pausechar);
    lcd.createChar(1, playchar);
    lcd.backlight();
    lcd.home();
    lcd.print("Note 1:");

    int prev_frequency;
    int frequency;
    char freq_str[5];

    attachInterrupt(digitalPinToInterrupt(BUTTON), buttonISR, RISING);

    for (byte i = 0; i < 8; i++) {
        lcd.setCursor(0, 1);

        while (!buttonPressed()) { // Until interrupt
            frequency = averagedDialRead() + 31;
            if (frequency != prev_frequency) {
                sprintf(freq_str, "%.4d", frequency);
                lcd.print(freq_str);
                tone(4, frequency);
                lcd.setCursor(0, 1);
                prev_frequency = frequency;
            }
        }

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
        if (buttonPressed()) {
            setLCDPlayingStatus(0);
            // Wait until pressed again
            while (!buttonPressed());
            setLCDPlayingStatus(1);
        }

        /* Update speed */
        int noteDuration = 1054 - analogRead(DIAL); // Not averaged for duration
        int notePause = noteDuration * 1.3;
        char bpm[4];
        sprintf(bpm, "%.3d", 30 * 1000 / noteDuration);
        lcd.setCursor(5, 0);
        lcd.print(bpm);

        /* Play note */
        tone(4, notes[i], noteDuration);
        lightOn(i);
        delay(notePause);
    }
}

/* Returns average of potentiometer reads to reduce fluctuation */
int averagedDialRead() {
    static int readAvg = 0;
    readAvg = readAvg * 0.95 + analogRead(DIAL) * 0.05;
    return readAvg;
}

/* Returns if button has been pressed, with debouncing */
bool buttonPressed() {
    static bool switchPending = false;
    static long int timer;
    if (isr)
        if (digitalRead(BUTTON) == HIGH) {
            switchPending = true;
            timer = millis();
        } else
            if (switchPending && millis() - timer >= DEBOUNCE_MS) {
                switchPending = false;
                isr = false;
                return true;
            }
    return false;
}

/* Prints the status on the second row of the LCD */
void setLCDPlayingStatus(byte playing) {
    lcd.setCursor(0, 1);
    lcd.write(playing);
    lcd.setCursor(2, 1);
    lcd.print(playing ? "Playing" : "Paused ");
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

/* ISR, on mode RISING for BUTTON pin */
void buttonISR() {
    if (!isr && digitalRead(BUTTON) == HIGH)
        isr = true;
}
