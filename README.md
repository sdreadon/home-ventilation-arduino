# home-ventilation-arduino
An Arduino technology project that 
controls a model home ventilation system

This Project uses the Arduino Uno.

There is a Raspberry Pi version as well, 
that is controlling the positive air pressure ventilation fan
in the ceiling. The arduino is more robust, but controls the fan
through a relay, something that is not legal here and will void our 
home insurance. The Raspberry Pi version uses a WeMo switch to make it legal.

#LIBRARIES 
include <PCD8544.h>
//libraries for temperature sensors 
include <OneWire.h>
include <DallasTemperature.h>

#Hardware
Arduino pins used:
LCD SPI interface
  SCLK = 3
  MOSI = 4 
  D/C = 5 
  RST -> 6 
  SCE -> 7
MOTION_SENSOR -> 2 (input)
EXHAUST FAN  -> 8 (input)
LCD BACKLIGHT  -> 9 (out) optional
CEILING FAN  -> 10 (out)
ALARM SPEAKER -> 11 (out PWM)
ONE_WIRE_BUS -> 12 (in/out)
CEILING LIGHT -> 13 (out)
ALARM SWITCH -> 14 (input) 
LIGHT SWITCH -> 15 (input) 
HEATER CONTROL -> 16 (out)
       
Hardware
•	Arduino Uno R3
•	PIR Motion Sensor
•	LCD Screen (Nokia 5110) 
•	DS18B20 One Wire Temperature Sensor (2x)
•	Relay (2 channel 5V ->12V DC/240V AC)
•	Peizo Speaker 
•	2x switches 
•	2x 12V computer fans 
•	LED (white)
•	LED (red)
•	Resistors 
•	Breadboards 
•	Wires
