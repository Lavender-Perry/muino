#include "compile_settings.hpp"

#ifdef HAS_EEPROM
#include <EEPROM.h>
#endif // HAS_EEPROM
#include <LiquidCrystal_I2C.h>

#ifdef HAS_CONTROLS
volatile bool isr[2] = { false }; // If ISR has occurred since last button read
#endif // HAS_CONTROLS

int note_amount = 1;
int notes[MAX_NOTE_AMT]; // Frequencies of notes in the melody

uint8_t pausechar[8] = {
    B11011,
    B11011,
    B11011,
    B11011,
    B11011,
    B11011,
    B11011,
    B11011
};
uint8_t playchar[8] = {
    B10000,
    B11000,
    B11100,
    B11110,
    B11110,
    B11100,
    B11000,
    B10000
};

LiquidCrystal_I2C lcd(LCD_I2C, 16, 2);

void setup() {
    Serial.begin(9600);

    // Lights setup
    for (byte i = FIRST_LIGHT; i <= FIRST_LIGHT + 8; i++) {
        pinMode(i, OUTPUT);
    }

    // LCD setup
    lcd.init();
    lcd.createChar(0, pausechar);
    lcd.createChar(1, playchar);
    lcd.backlight();

    int prev_frequency;
    int frequency;
    char freq_str[5];

#ifdef HAS_EEPROM
    // If there is data to read, set note info to that data
    int res = 0;
    if (EEPROM.get(0, res) == -69) {
        EEPROM.get(sizeof(int), note_amount);
        for (byte i = 0; i < note_amount; i++) {
            EEPROM.get((i + 3) * sizeof(int), notes[i]);
        }
        EEPROM.get(sizeof(int) * 2, res);
        playNotes(notes, note_amount, res);
        return;
    }
#endif // HAS_EEPROM

#ifdef HAS_CONTROLS
    // Controls setup
    for (byte i = 0; i < 2; i++) {
        pinMode(buttons[i], INPUT);
    }
    pinMode(BUZZER, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(buttons[0]), button0ISR, RISING);
    attachInterrupt(digitalPinToInterrupt(buttons[1]), button1ISR, RISING);

    // Get amount of notes in melody (button 0 presses before button 1 pressed + 1)
    lcd.home();
    lcd.print("Note amount:");
    while (!buttonPressed(1)) {
        lcd.setCursor(0, 1);
        lcd.print(String(note_amount));
        if (buttonPressed(0)) {
            note_amount++;
            if (note_amount == MAX_NOTE_AMT) {
                break;
            }
        }
    }

    // Get note frequencies / rests
    lcd.clear();
    lcd.home();
    lcd.print("Note 1:");
    for (byte i = 0; i < note_amount; i++) {
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
#endif // HAS_CONTROLS

    lcd.clear();
    lcd.home();
    lcd.print("BPM:");
    setLCDPlayingStatus(1);
}

void loop() {
#ifdef HAS_CONTROLS
    playNotes(notes, note_amount, 1054 - analogRead(DIAL));
#endif // HAS_CONTROLS
}

void serialEvent() {
    // Read from serial
    String line = "";
    while (Serial.available() > 0) {
        line += (char) Serial.read();
    }
    line.trim();
    // Process command
    int data[MAX_NOTE_AMT + 2];
    parseNoteData(data, line.substring(line.indexOf(' ')));
    
    if (line.startsWith("play")) {
        note_amount = data[0];
        for (byte i = 0; i < note_amount; i++) {
            notes[i] = data[i + 2];
        }
        playNotes(notes, note_amount, data[1]);
        return;
    }
#ifdef HAS_EEPROM
    if (line.startsWith("save")) {
        // Write data to EEPROM, will be loaded on next restart
        size_t addr = 0;
        EEPROM.put(addr, -69); // Number indicating there is data to read
        for (byte i = 0; i < data[0] + 2; i++) {
            addr += sizeof(int);
            EEPROM.put(addr, data[i]);
        }
        return;
    }
    if (line.startsWith("reset")) {
        EEPROM.put(0, 0);
        return;
    }
#endif // HAS_EEPROM
}

/* Loads note data from string of numbers separated by semicolons.
 * First number is amount of notes, second is BPM, rest is note frequencies. */
void parseNoteData(int* buf, String str) {
    buf[0] = str.toInt();
    if (buf[0] > MAX_NOTE_AMT) {
        buf[0] = MAX_NOTE_AMT;
    }
    for (byte i = 1; i < buf[0] + 2; i++) {
        str.remove(0, str.indexOf(';') + 1);
        buf[i] = str.toInt();
    }
}

/* Plays a sequence of notes. */
void playNotes(int* notes, int amount, int duration) {
    const int note_pause = duration * 1.3;

    // Print BPM
    char bpm[4];
    sprintf(bpm, "%(5).1f", 30000.0 / duration);
    lcd.setCursor(5, 0);
    lcd.print(bpm);

    for (byte i = 0; i < amount; i++) {
#ifdef HAS_CONTROLS
        // Check if pause button pressed
        if (buttonPressed(0)) {
            setLCDPlayingStatus(0);
            while (!buttonPressed(0)); // Wait until pressed again
            setLCDPlayingStatus(1);
        }
#endif // HAS_CONTROLS

        // Play note
        if (notes[i] != 0) {
            tone(4, notes[i], duration);
        }
        lightOn(i);
        delay(note_pause);
    }
}

/* Prints the status on the second row of the LCD.
 * playing must be 0 or 1. */
void setLCDPlayingStatus(byte playing) {
    lcd.setCursor(0, 1);
    lcd.write(playing);
    lcd.setCursor(2, 1);
    lcd.print(playing ? "Playing" : "Paused ");
}

/* Turns off the light previously given to the function if required, turns on the one
 * given.  Wraps around to control 8 lights. */
void lightOn(byte i) {
    static byte previous = 0;

    if (previous != 0) {
        digitalWrite(previous, LOW);
    }

    previous = i % 8 + FIRST_LIGHT;
    digitalWrite(previous, HIGH);
}

#ifdef HAS_CONTROLS
/* Returns average of potentiometer reads to reduce fluctuation. */
int averagedDialRead() {
    static int read_avg = 0;
    read_avg = read_avg * 0.95 + analogRead(DIAL) * 0.05;
    return read_avg;
}

/* Returns if button has been pressed, with debouncing.
 * i must be 0 or 1. */
bool buttonPressed(byte i) {
    static bool switch_pending[2] = { false };
    static long int timer[2];
    if (isr[i]) {
        if (digitalRead(buttons[i]) == HIGH) {
            switch_pending[i] = true;
            timer[i] = millis();
        } else if (switch_pending[i] && millis() - timer[i] >= DEBOUNCE_MS) {
            switch_pending[i] = false;
            isr[i] = false;
            return true;
        }
    }
    return false;
}

/* ISRs, on mode RISING for buttons[0] / buttons[1] pins. */
void button0ISR() {
    if (!isr[0] && digitalRead(buttons[0]) == HIGH) {
        isr[0] = true;
    }
}
void button1ISR() {
    if (!isr[1] && digitalRead(buttons[1]) == HIGH) {
        isr[1] = true;
    }
}
#endif // HAS_CONTROLS
