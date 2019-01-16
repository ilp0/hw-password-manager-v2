//THIS PROGRAM WILL DELETE YOUR CURRENT PASSWORDS
#define SD_SS_PIN 10

//SCREEN STUFF
#define I2C_ADDRESS 0x3C
SSD1306AsciiAvrI2c display;

#include <SPI.h>
#include <SD.h>

#define PASS_ARR_SIZE   5
String passwordArray[PASS_ARR_SIZE][1] = {
  {"", ""},
  {"", ""},
  {"", ""},
  {"", ""},
  {"", ""}
}

File myFile;

void setup() {

  display.begin(&Adafruit128x64, I2C_ADDRESS);
  display.setFont(Iain5x7);
  display.println("INITIALIZING SD CARD");
  if (!SD.begin(SD_SS_PIM)) {
    display.println("SD INITIALIZATION FAILED!");
    return;
  }
  Serial.println("INITIALIZATION COMPLETE");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  if (SD.exists("secret.txt") SD.remove("secret.txt");
  myFile = SD.open("secret.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    display.println("Writing from flash to SD...");
    for (int i = 0; i < PASS_ARR_SIZE; i++) {
      myFile.println(passwordArray[i][0] + "\;" + passwordArray[i][1]);
    }
    // close the file:
    myFile.close();
    display.println("Writing succesful!");
    display.println("You can now reflash the password manager.");


  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening secret.txt");
  }
}

void loop() {
  // nothing happens after setup
}


