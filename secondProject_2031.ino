
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Keypad.h>

#define EEPROM_ID 0xA0 >> 1
//DEFINE DIGITAL INPUT PINS
//DEFINE POSITIONS
#define INLET 0x00
#define OUTLET 0x01
#define HEATER 0x02
#define STIRRER 0x03

//DEFINE DIGITAL INPUT PINS
#define HIGHFLOATER 0x10
#define LOWFLOATER 0x11

//DEFINE DIGITAL INPUT PINS
#define VOL 0x30
#define TEMP 0x31
//START/STOP FOR EVERY PIN
#define START 0x01
#define STOP 0x00

const int rs = 13, en = 12, d4 = 8, d5 = 9, d6 = 10, d7 = 11;  //LCD DISPLAY PINS
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);                     //LCD DISPLAY PINS

bool firstCharEntered = false;   // Flag to track if the first character is entered
bool secondCharEntered = false;  // Flag to track if the second character is entered
bool thirdCharEntered = false;   // Flag to track if the third character is entered

char key1Entered, key2Entered, key3Entered, key4Entered;  // Characters entered from the keypad

const byte ROWS = 4;  // Number of rows on the keypad
const byte COLS = 4;  // Number of columns on the keypad

char hexaKeys[ROWS][COLS] = {  //keypad
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { A0, A1, A2, A3 };  //keypad row pins
byte colPins[COLS] = { 7, 6, 5, 4 };      //col pins

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);  //GET A CHARACTER

void displayRegistrationInfo() {

  lcd.setCursor(0, 0);
  lcd.print("CHARISTES ");
  lcd.print("2031");
  lcd.setCursor(0,1);
  lcd.print("ENTER VOL: ????");
}

void setup() { 

  int i;  

  lcd.begin(16, 2);              //init for lcd
  lcd.setCursor(0, 0);          
  displayRegistrationInfo();

  Serial.begin(9600);  //init serial com
  Wire.begin();        //init i2c

  Wire.beginTransmission(EEPROM_ID);  //begin transmission
  Wire.write(0);                  
  for (i = 0; i < 30; i++) {          //write 30bytes to eeprom
    Wire.write(0xAA);                 
  }
  Wire.endTransmission();  //end transmission
}

void loop() {  
               
  unsigned waterLevel = 0;
  unsigned tankVol = 0;
  unsigned temp = 0;
  unsigned char tempLowByte, tempHighByte, tankLowByte, tankHighByte;
  unsigned int firstNum, secondNum, thirdNum, fourthNum;

if (!firstCharEntered) {                // Check if the first character has been entered
    key1Entered = customKeypad.getKey();  // Read a character from the keypad

    if (key1Entered != NO_KEY) {  // If a key is pressed
      firstCharEntered = true;    // Mark the first character as entered
      lcd.setCursor(11, 1);       // Set cursor position for LCD
      lcd.print(key1Entered);     // Print the first entered character
    }
  } else if (!secondCharEntered) {        // If the second character has not been entered
    key2Entered = customKeypad.getKey();  // Read a character from the keypad
    if (key2Entered != NO_KEY) {          // If a key is pressed
      secondCharEntered = true;           // Mark the second character as entered
      lcd.setCursor(12, 1);               // Set cursor position for LCD
      lcd.print(key2Entered);             // Print the second entered character
    }
  } else if (!thirdCharEntered) {         // If the third character has not been entered
    key3Entered = customKeypad.getKey();  // Read a character from the keypad
    if (key3Entered != NO_KEY) {          // If a key is pressed
      thirdCharEntered = true;            // Mark the third character as entered
      lcd.setCursor(13, 1);               // Set cursor position for LCD
      lcd.print(key3Entered);             // Print the third entered character
    }
  } else {                                // All characters have been entered
    key4Entered = customKeypad.getKey();  // Read a character from the keypad
    if (key4Entered != NO_KEY) {          // If a key is pressed
      lcd.setCursor(14, 1);               // Set cursor position for LCD
      lcd.print(key4Entered);             // Print the fourth entered character

      // Delay for visibility
      delay(1000);
      lcd.clear();  // Clear the LCD screen

      lcd.setCursor(0, 0);         // Set cursor position for LCD
      lcd.print("SAVING VOLUME");  // Print message
      lcd.setCursor(0, 1);         // Set cursor position for LCD
      lcd.print("TO EEPROM");      // Print message

      delay(1000);  // Delay between messages

      // Save the volume to EEPROM
      Wire.beginTransmission(EEPROM_ID);  // Begin transmission
      Wire.write(0x10);                   // Start from position 10
      Wire.write(key1Entered);            // Write the first number
      Wire.write(key2Entered);            // Write the second number
      Wire.write(key3Entered);            // Write the third number
      Wire.write(key4Entered);            // Write the fourth number
      Wire.endTransmission();             // End transmission

      // Convert characters to integers
      firstNum = key1Entered - '0';   // Convert the character representation to integer value
      secondNum = key2Entered - '0';  // Convert the character representation to integer value
      thirdNum = key3Entered - '0';   // Convert the character representation to integer value
      fourthNum = key4Entered - '0';  // Convert the character representation to integer value

      // Calculate the desired volume
      unsigned int desired_vol = (firstNum * 1000 + secondNum * 100 + thirdNum * 10 + fourthNum);

      // Clear the LCD screen
      lcd.clear();

      // Print the volume
      lcd.setCursor(0, 0);
      lcd.print("VOLUME");
      lcd.setCursor(10, 0);
      lcd.print(desired_vol);

      // Delay for visibility
      delay(1000);

      // Clear the LCD screen
      lcd.clear();

      // Print messages
      lcd.setCursor(0, 0);
      lcd.print("OUTLET ON");
      lcd.setCursor(0, 1);
      lcd.print("INLET  OFF");

      // Control valves
      Serial.write(INLET);
      Serial.write(STOP);
      Serial.write(OUTLET);
      Serial.write(START);

      // Control water level
      do {
        Serial.write(LOWFLOATER);
        while (!Serial.available()) {};
        waterLevel = Serial.read();
        if (waterLevel == 0) {
          Serial.write(OUTLET);
          Serial.write(STOP);
        }
      } while (waterLevel > 0);

      // Delay for visibility
      delay(1000);

      // Clear the LCD screen
      lcd.clear();

      // Print messages
      lcd.setCursor(0, 0);
      lcd.print("OUTLET  OFF");
      lcd.setCursor(0, 1);
      lcd.print("INLET  ON");

      // Control valves
      Serial.write(INLET);
      Serial.write(START);

      // Control tank volume
      while (tankVol <= desired_vol) {
        Serial.read();
        Serial.read();
        Serial.write(VOL);
        while (!Serial.available()) {}
        tankHighByte = Serial.read();
        while (!Serial.available()) {}
        tankLowByte = Serial.read();

        tankVol = tankHighByte;
        tankVol = (tankVol << 8) | (int)tankLowByte;

        lcd.setCursor(12, 1);
        lcd.print(tankVol);
      };

      // Control valves
      Serial.write(INLET);
      Serial.write(STOP);

      // Delay for visibility
      delay(1000);

      // Clear the LCD screen
      lcd.clear();

      // Control temperature
      Serial.read();
      Serial.read();
      Serial.write(TEMP);
      while (!Serial.available()) {}
      tempHighByte = Serial.read();
      while (!Serial.available()) {}
      tempLowByte = Serial.read();

      temp = tempHighByte;
      temp = (temp << 8) | (int)tempLowByte;

      // Print temperature
      lcd.setCursor(0, 0);
      lcd.print("TEMP");
      lcd.setCursor(10, 0);
      lcd.print(temp);
      lcd.setCursor(0, 1);
      lcd.print("STIR/HEATER ON");

      // Delay for visibility
      delay(1000);

      // Clear the LCD screen
      lcd.clear();

      // Control heaters and stirrer
      Serial.write(HEATER);
      Serial.write(START);
      Serial.write(STIRRER);
      Serial.write(START);

      // Control temperature
      while (temp <= 25) {
        Serial.read();
        Serial.read();
        Serial.write(TEMP);
        while (!Serial.available()) {}
        tempHighByte = Serial.read();
        while (!Serial.available()) {}
        tempLowByte = Serial.read();

        temp = tempHighByte;
        temp = (temp << 8) | (int)tempLowByte;

        lcd.setCursor(12, 0);
        lcd.print(temp);
      };

      // Control heaters and stirrer
      Serial.write(HEATER);
      Serial.write(STOP);
      Serial.write(STIRRER);
      Serial.write(STOP);

      // Delay for visibility
      delay(100);

      // Clear  
      lcd.clear(); 

      lcd.setCursor(0, 0);          
      lcd.print("TEMP");            
      lcd.setCursor(10, 0);         
      lcd.print(temp);       
      lcd.setCursor(0, 1);          
      lcd.print("CYCLE COMPLETED");  //print

      firstCharEntered = false;
      secondCharEntered = false;
      thirdCharEntered = false;
      key1Entered = 0;
      key2Entered = 0;
      key3Entered = 0;
      key4Entered = 0;
    }
  }
}
