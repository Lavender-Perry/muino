#include "compile_settings.h"

/* Turns off the light previously given to the function if required, turns on the one
 * given. Lights are referred to as 1-8. */
void lightOn(byte light) {
    static byte previous = 0;

    if (previous)
        digitalWrite(previous, LOW);

    const byte pin = light + 4;
    digitalWrite(pin, HIGH);
    previous = pin;
}
