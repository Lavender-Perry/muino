#include <LiquidCrystal_I2C.h>

#include "compile_settings.hpp"

int averagedDialRead();
bool buttonPressed(byte btnIdx);
void setLCDPlayingStatus(byte playing);
void lightOn(byte i);
void button0ISR();
void button1ISR();

LiquidCrystal_I2C lcd(LCD_I2C, 16, 2);

int noteAmount = 1;
int notes[MAX_NOTE_AMT]; // Frequencies of notes in the melody

volatile bool isr[2] = { false }; // If ISR has occurred since last button read

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
    /* Pin setup */
    for (byte i = 0; i < 2; i++)
        pinMode(buttons[i], INPUT);
    pinMode(BUZZER, OUTPUT);
    for (byte i = FIRST_LIGHT; i <= FIRST_LIGHT + 8; i++)
        pinMode(i, OUTPUT);

    /* LCD setup */
    lcd.init();
    lcd.createChar(0, pausechar);
    lcd.createChar(1, playchar);
    lcd.backlight();

    int prev_frequency;
    int frequency;
    char freq_str[5];

    /* Set up buttons */
    attachInterrupt(digitalPinToInterrupt(buttons[0]), button0ISR, RISING);
    attachInterrupt(digitalPinToInterrupt(buttons[1]), button1ISR, RISING);

    /* Get amount of notes in melody (button 0 presses before button 1 pressed + 1) */
    lcd.home();
    lcd.print("Note amount:");
    while (!buttonPressed(1)) {
        lcd.setCursor(0, 1);
        lcd.print(String(noteAmount));
        if (buttonPressed(0)) {
            noteAmount++;
            if (noteAmount == MAX_NOTE_AMT)
                break;
        }
    }

    /* Get note frequencies / rests */
    lcd.clear();
    lcd.home();
    lcd.print("Note 1:");
    for (byte i = 0; i < noteAmount; i++) {
        lcd.setCursor(0, 1);

        while (!buttonPressed(0)) {
            if (buttonPressed(1)) {
                frequency = 0;
                break;
            }
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
    for (byte i = 0; i < noteAmount; i++) {
        /* Check if pause button pressed */
        if (buttonPressed(0)) {
            setLCDPlayingStatus(0);
            while (!buttonPressed(0)); // Wait until pressed again
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
        if (notes[i] != 0)
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

/* Returns if button has been pressed, with debouncing
 * btnIdx must be 0 or 1 */
bool buttonPressed(byte btnIdx) {
    static bool switchPending[2] = { false };
    static long int timer[2];
    if (isr[btnIdx])
        if (digitalRead(buttons[btnIdx]) == HIGH) {
            switchPending[btnIdx] = true;
            timer[btnIdx] = millis();
        } else if (switchPending[btnIdx] && millis() - timer[btnIdx] >= DEBOUNCE_MS) {
            switchPending[btnIdx] = false;
            isr[btnIdx] = false;
            return true;
        }
    return false;
}

/* Prints the status on the second row of the LCD
 * playing must be 0 or 1 */
void setLCDPlayingStatus(byte playing) {
    lcd.setCursor(0, 1);
    lcd.write(playing);
    lcd.setCursor(2, 1);
    lcd.print(playing ? "Playing" : "Paused ");
}

/* Turns off the light previously given to the function if required, turns on the one
 * given. Lights are referred to as 0-7. */
void lightOn(byte i) {
    static byte previous = 0;

    if (previous != 0)
        digitalWrite(previous, LOW);

    previous = i % 8 + FIRST_LIGHT;
    digitalWrite(previous, HIGH);
}

/* ISRs, on mode RISING for buttons[0] / buttons[1] pins */
void button0ISR() {
    if (!isr[0] && digitalRead(buttons[0]) == HIGH)
        isr[0] = true;
}
void button1ISR() {
    if (!isr[1] && digitalRead(buttons[1]) == HIGH)
        isr[1] = true;
}
