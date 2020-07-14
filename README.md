# HW Password Manager V2

[Youtube Video](https://www.youtube.com/watch?v=QssnJo2gxCg)

## Libraries needed:

[SSD1306Ascii](https://github.com/greiman/SSD1306Ascii)

[RFID](https://github.com/miguelbalboa/rfid)

[SD](https://github.com/arduino-libraries/SD)

[rBase64](https://github.com/boseji/rBASE64)

[DigitalIO](https://github.com/greiman/DigitalIO)

[SPI](https://www.arduino.cc/en/reference/SPI)

## Wiring

### SSD1306 display

| SSD1306 Pin 	| Pro Micro Pin 	|
|-	|-	|
| GND 	| GND 	|
| VCC 	| 5V 	|
| SCL 	| SCL 	|
| SDA 	| SDA 	|

### RC522 RFID 

| RC522 	| Pro Micro Pin 	|
|-	|-	|
| SS(component most likely says SDA) 	| 8 	|
| MISO 	| MISO (14) 	|
| MOSI 	| MOSI (16) 	|
| SCK 	| SCLK (15) 	|
| RST 	| 4 	|
| GND 	| GND 	|
| 3.3V 	| 3.3V 	|

### Micro SD card reader

| SD Card reader 	| Pro Micro Pin 	|
|-	|-	|
| SS 	| 10 	|
| MISO 	| MISO (14) 	|
| MOSI 	| MOSI (16) 	|
| SCK 	| SCLK (15) 	|
| GND 	| GND 	|
| 3.3V 	| 3.3V 	|

### Joystick

| Joystick | Pro Micro Pin |
|- |- |
| VCC | VCC|
| Y | A1 |
| GND | GND |
| X | A0 |

### Push buitton

Pro Micro Pin 9
