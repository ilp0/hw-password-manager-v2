/*
    HARDWARE PASSWORD MANAGER
    Author: Ilpo Loikkanen
*/
//base64
#include <rBase64.h>

//software SPI
#include <DigitalIO.h>

// RC522 rfid
#include <RFID.h>

//SD card
#include <SD.h>

//128x64 OLED display. Using ascii library for smaller size
#include <SSD1306Ascii.h>
#include <SSD1306AsciiAvrI2c.h>
#include <SSD1306init.h>

//KB and mouse emulation
#include <Keyboard.h>
#include <Mouse.h>

//hardware SPI
#include <SPI.h>

//RFID PINS
#define RFID_SS_PIN 8
#define RST_PIN 4

//SD CARD PIN
#define SD_SS_PIN 10

//SCREEN STUFF
#define I2C_ADDRESS 0x3C
SSD1306AsciiAvrI2c display;

//other defines
#define PASSWORD_SIZE  8
#define PASS_ARR_SIZE  10
#define STRING_ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])

//RFID STUFF
RFID mfrc522 (RFID_SS_PIN, RST_PIN) ;
unsigned long RFID;
String readid;

//passwords and cards
String card1 = "15025321931175"; //card
String card2 = "228166202115251"; //keychain
byte password[PASSWORD_SIZE] = {3, 3, 3, 3, 3, 3, 3, 3};
byte pw_input[PASSWORD_SIZE];
byte curDigit = 0;

//joystick stuff
int joyPin1 = 1;
int joyPin2 = 0;

//other stuff
bool unlocked = false;
int page = 0;
int sel = 0;
String passwords[PASS_ARR_SIZE][2];
String menuItems[2][2] = {
  {"Create new", "Delete"},
  {"Mouse Mode", "Change key"},

};

//Encryption stuff
uint8_t key[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; //CHANGE THIS!!!
File secret;

void setup()
{
  display.begin(&Adafruit128x64, I2C_ADDRESS);
  display.setFont(Iain5x7);
  SPI.begin();
  mfrc522.init();
  if (!SD.begin(10)) {
    display.println("SD CARD initialization failed!");
    return;
  }
  pinMode(9, INPUT);
  display.clear();
  display.set2X();
  display.setCursor(7, 2);
  display.println("AUTHENTICATE");
  display.setCursor(10, 5);
  display.println("________________");
  display.setCursor(20, 4);
  //rfid
  Keyboard.begin();

}

void bootAnim() {
  //todo
}

void temp()
{
  readid = "";
  if (mfrc522.readCardSerial())
  {
    for (int i = 0; i <= 4; i++) //card value: "xyz xyz xyz xyz xyz" (15 digits maximum; 5 pairs of xyz)hence 0<=i<=4 //
    {
      RFID = mfrc522.serNum[i];
      readid += RFID; // store RFID value into string "cardNum" and concatinate it with each iteration
    }
  }
  mfrc522.halt();
}

//get joystick direction, using psp joystick.
byte getDirection(int x, int y) {
  byte dir = 0;
  if (x >= 450 && x <= 550 && y >= 400 && y <= 480) dir = 0;              //CENTER
  else if (x >= 200 && x <= 370  && y >= 500 && y <= 660) dir = 1;        //UP
  else if (x >= 560 && x <= 700  && y >= 150 && y <= 300) dir = 2;        //DOWN
  else if (x >= 370 && x <= 460  && y >= 560 && y <= 700) dir = 3;        //LEFT
  else if (x >= 700 && x <= 900  && y >= 250 && y <= 400) dir = 4;        //RIGTH
  else dir = 5;    //UNKNOWN
  return dir;
}

void showMenu(int index, String items[][2], String title, int arrSize) {
  if (index >= arrSize) index = 0;
  if (index < 0) index = arrSize - 1;
  display.clear();
  display.set2X();
  display.println(title);
  display.set1X();
  //print menuitems
  if (page != 0) {
    for (int i = 0; i < arrSize && i < 6; i++) {
      if (arrSize >= i) {
        if (index < 6) display.println("   " + items[page - 1][i]);
        else display.println("   " + items[page - 1][i + index - 6]);
      }
    }
  } else {

    //print password menu
    for (int i = 0; i < arrSize && i < 6; i++) {
      if (index < 6) display.println("   " + items[i][0]);
      else display.println("   " + items[(i + index) - 5][0]);

    }
  }
  //print selector
  if (index < arrSize) {
  } else index = 0;
  display.setRow(2 + index);
  if (index > 6) display.setRow(7);
  display.setCol(0);
  display.print(">");
  sel = index;

}
void showSuccess() {
  display.clear();
  display.set2X();
  display.print("Success");
  delay(1500);
}

void showFail() {
  display.clear();
  display.set2X();
  display.print("Action failed.");
  delay(1500);
}

void inputPass(int i) {
  display.clear();
  display.set2X();
  display.println("UP Input");
  display.println("RIGHT Copy");
  display.println("DOWN Show");
  display.println("LEFT Cancel");
  while (getDirection(analogRead(joyPin1), analogRead(joyPin2)) != 0 || getDirection(analogRead(joyPin1), analogRead(joyPin2)) != 5) {
    switch (getDirection(analogRead(joyPin1), analogRead(joyPin2))) {
      case 1:
        Keyboard.print(passwords[i][1]);
        Keyboard.press(KEY_RETURN);
        Keyboard.releaseAll();
        showSuccess();
        return;
        break;
      case 2:
        display.clear();
        display.setCursor(0, 0);
        display.set2X();
        display.println("USER");
        display.set1X();
        display.println(passwords[i][0]);
        display.set2X();
        display.println("PW");
        display.set1X();
        display.println(passwords[i][1]);
        display.println("\nPress the button to exit.");
        while (digitalRead(9) != HIGH);
        delay(500);
        return;
        break;
      case 3:
        return;
        break;
      case 4:
        Keyboard.print(passwords[i][0]);
        Keyboard.press(KEY_RIGHT_SHIFT);
        for (int i = 0; i < passwords[i][0].length(); i++) {
          Keyboard.press(KEY_LEFT_ARROW);
          Keyboard.release(KEY_LEFT_ARROW);
        }
        Keyboard.release(KEY_RIGHT_SHIFT);
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press('x');
        Keyboard.releaseAll();
        display.clear();
        showSuccess();
        return;
        break;
      default:
        delay(60);
        break;
    }
  }
  display.clear();
}
//read passwords from sd card
void refreshSDCard() {

  secret = SD.open("secret.txt");
  if (secret) {
    for (int i = 0; secret.available(); i++) {
      passwords[i][0] = secret.readStringUntil(';');
      char *tmp = secret.readStringUntil('\n').c_str();
      if (rbase64.decode(tmp) == RBASE64_STATUS_OK ) passwords[i][1] = rbase64.result() + '\0';
      else {
        passwords[i][1] = tmp;
        display.println("could not decode password");
      }
    }

    secret.close();

  } else {
    showFail();
  }
}
//generate new creds and write to sd card
void generateCreds() {
  char *alphabeth = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'?^&/%$£-+";
  const long alphabethLength = 70;//sizeof(alphabeth);
  String pass = "";
  display.clear();
  display.set1X();
  display.print("Hold a random\ndirection on \njoystick for randomness");
  display.set2X();
  delay(2000);
  randomSeed(analogRead(joyPin1)*analogRead(joyPin2) + millis());
  for (int n = 0; n < 20; n++)
  {
    pass += alphabeth[random(0, alphabethLength)];
  }
  secret = SD.open("secret.txt", FILE_WRITE);
  if (secret) {
    secret.print("\ngen\;" + pass + "\n");
    showSuccess();
    secret.close();
  } else showFail();
  delay(500);

}

void mouseMode() {
  display.clear();
  display.set2X();
  display.println("MOUSE MODE");
  delay(200);
  Mouse.begin();
  bool isActive = true;
  int clickStart = 0;
  bool clickActive = false;
  while (isActive) {
    if (digitalRead(9) != HIGH) {
      if (clickActive) {
        Mouse.release();
        if ((millis() - clickStart) > 2000) isActive = false;
        clickActive = false;
        clickStart = 0;
        delay(300);
      }

      switch (getDirection(analogRead(joyPin1), analogRead(joyPin2))) {
        case 0:
          break;
        case 1:
          Mouse.move(0, -10, 0);
          break;
        case 2:
          Mouse.move(0, 10, 0);
          break;
        case 3:
          Mouse.move(-10, 0);
          break;
        case 4:
          Mouse.move(10, 0, 0);
          break;
        default:
          break;
      }
      delay(50);

    } else {
      if (clickActive == false) clickStart = millis();
      clickActive = true;
      Mouse.press();
    }
  }
}

void unlock() {
  unlocked = true;
  refreshSDCard();
  showMenu(sel, passwords, "Vault", PASS_ARR_SIZE);
}

void loop()
{
  //pasword and rfid unlock
  if (!unlocked) {
    byte curDir = getDirection(analogRead(joyPin1), analogRead(joyPin2));
    bool enterPress = false;
    if (digitalRead(9) == HIGH) enterPress = true;
    if ((curDir != 5 && curDir != 0) || enterPress)  {
      if (!enterPress) {
        pw_input[curDigit] = curDir;
        display.print("*");
        curDigit++;
      }
      if (curDigit >= PASSWORD_SIZE || enterPress) {
        delay(400);
        enterPress = false;
        bool isRight = true;
        for (int i = 0; i < PASSWORD_SIZE; i++) {
          if (pw_input[i] != password[i]) isRight = false;
        }
        if (isRight) {
          unlock();
        }
        else {
          display.clear();
          display.setCursor(5, 2);
          display.println("AUTHENTICATE");
          display.setCursor(5, 6);
          display.println("________");
          display.setCursor(5, 5);
          display.println("ERROR!");
          delay(400);
          display.setCursor(5, 5);
          display.println("        ");
          display.setCursor(5, 5);
          curDigit = 0;
        }
      }
      delay(100);

    }
    if (!mfrc522.isCard())
    {
      return;
    }
    temp();
    if (readid == card1 || readid == card2) unlock();
    else return;
  } else {
    //IF UNLOCKED
    if (digitalRead(9) == HIGH) {
      //menu select
      switch (page) {
        case 0:
          //passwords
          inputPass(sel);
          showMenu(sel, passwords, "VAULT", PASS_ARR_SIZE);
          return;
          break;
        case 1:
          //generate new
          generateCreds();
          showMenu(sel, menuItems, "GENERATE", 1);
          break;
        case 2:
          //Tools
          switch (sel) {
            case 0:
              mouseMode();
              showSuccess();
              break;
            default:
              break;
          }
          showMenu(sel, menuItems, "TOOLS", 2);
          break;
        case -1:
          page = 2;
          showMenu(sel, menuItems, "SETTINGS", 2);
          break;
        default:
          page = 0;
          showMenu(sel, passwords, "VAULT", PASS_ARR_SIZE);
          break;

      }
    }
    switch (getDirection(analogRead(joyPin1), analogRead(joyPin2))) {
      case 1:
        sel--;
        delay(200);
        break;
      case 2:
        sel++;
        delay(200);
        break;
      case 3:
        page--;
        delay(200);
        break;
      case 4:
        page++;
        delay(200);
        break;
      default:
        return;
        break;
    }

    switch (page) {
      case 0:
        //passwords
        showMenu(sel, passwords, "VAULT", PASS_ARR_SIZE);
        break;
      case 1:
        //generate new
        showMenu(sel, menuItems, "GENERATE", 1);
        break;
      case 2:
        //change password
        showMenu(sel, menuItems, "TOOLS", 2);
        break;
      case -1:
        page = 2;
        showMenu(sel, menuItems, "TOOLS", 2);
        break;
      default:
        page = 0;
        showMenu(sel, passwords, "VAULT", PASS_ARR_SIZE);
        break;
    }
    delay(50);
  }
}





