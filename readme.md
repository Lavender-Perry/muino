# Muino
A melody maker on Arduino.
It has only been tested on the Arduino Uno, so there may be issues with other boards.

## Creating the circuit
You will need to connect the following to your Arduino
If you want to save some time, you should connect them to match compile_settings.h
* 8 LEDs (must be on adjacent pins)
* A buzzer compatable with the tone function
* A potentiometer
* A pushbutton
* A 16x2 LCD screen via I2C

You should use resistors on the LEDs & button like always. \
If you did not connect everything to match compile_settings.h,
you should edit compile_settings.h to match how you connected everything. \

If you have questions, ask in issue #1.

## Uploading the code
First, make sure [the LiquidCrystal_I2C library](https://www.arduino.cc/reference/en/libraries/liquidcrystal-i2c/) is installed. \
After that, it works the same as any other Arduino sketch: \
[CLI Tutorial](https://arduino.github.io/arduino-cli/0.19/getting-started/#compile-and-upload-the-sketch) \
[IDE 1 Tutorial](https://docs.arduino.cc/software/ide-v1/tutorials/arduino-ide-v1-basics#uploading) \
[IDE 2 Tutorial](https://docs.arduino.cc/software/ide-v2/tutorials/getting-started/ide-v2-uploading-a-sketch)
