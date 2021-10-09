# Muino
A melody maker on Arduino.

## Running
Open the sketch in the Arduino IDE & upload it to your Arduino board.
\
Alternatively, install arduino-cli & run
`arduino-cli compile -b [fqbn] && arduino-cli upload -p [port] -b [fqbn]` in the
directory of this repository,
replacing "[fqbn]" with the FQBN for your Arduino board (for example
arduino:avr:uno), & "[port]" with the port of your Arduino (for example
/dev/ttyACM0).  This has only been tested on the Arduino Uno, so there may
be issues with other boards.
