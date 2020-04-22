/* This code sends data via UART from one Arduino board (controller) to another (MainBoard) to control the stepper motors and the LED illumination
 * Arduino sends the data only if there is a change in the value of the joysticks or the LED potentiometer 
 * Therefore, there is no continuous data communication between the mainboard and the controller
 * Arduino board: Adafruit ItsyBitsy 32u4 5V  
 */

#include <Wire.h> // needed for the I2C communication of the OLED, already installed in Arduino IDE
#include <Adafruit_GFX.h> // needed for the OLED // Copyright (c) 2012 Adafruit Industries.  All rights reserved.
#include <Adafruit_SSD1306.h> // needed for the OLED // Copyright (c) 2012 Adafruit Industries.  All rights reserved.

// Arduiono pins for the joysticks
#define joystickX A0
#define joystickY A1
#define joystickC A3
#define joystickZ A2
#define joystickR A5
#define joystickT A4

// potentiometer for the LED intensity
#define potLED 8


// change these values to set the direction of the movement with respect to the joystick
// the direction can also be changed by simply rotating the motor 180-deg 

#define directionX -1
#define directionY 1
#define directionZ 1
#define directionC 1
#define directionR -1
#define directionT 1

// the joystick has a variable speed control at 3 predefined speeds
// change these values to change the speed of the motor

#define fast 500
#define medium 50
#define slow 10

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

int readX;
int readY;
int readC;
int readZ;
int readR;
int readT;
int readL;

int readXlast = 0;
int readYlast = 0;
int readClast = 0;
int readZlast = 0;
int readRlast = 0;
int readTlast = 0;
int readLlast = 0;

String stepper;

void setup() {

  display.begin(SSD1306_SWITCHCAPVCC, 0x3c); //initialize the display
  Serial1.begin(57600); //start the serial communication, Serial1: UART

  pinMode(joystickX, INPUT);
  pinMode(joystickY, INPUT);
  pinMode(joystickZ, INPUT);
  pinMode(joystickC, INPUT);
  pinMode(joystickR, INPUT);
  pinMode(joystickT, INPUT);
  pinMode(potLED, INPUT);

// show a start-up text on the OLED display

  display.clearDisplay();
  display.setRotation(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.setTextSize(2);
  display.println("Welcome!");
  display.display();

  delay(2000); //optional delay for the start-up message

// set the LED illumination to the potentiometer value at the start-up

  readLlast = map((analogRead(potLED)), 0, 1023, 0, 20); // map to 20 steps (5% steps) 
  Serial1.print(readL); Serial1.print("L"); Serial1.print(","); // send the data to the MainBoard to update the LED intensity
  
// show the LED intensity on the OLED display

  display.clearDisplay();
  display.setCursor(0, 10);
  display.setTextSize(2);
  display.print("LED:"); display.print(100 - (readLlast * 5)); display.print("%"); // PWM of the LED driver used in the prototype works in the opposite direction
  display.display();

}

void loop() {

  readX = map((analogRead(joystickX)), 0, 1023, -3, 3); // map the measured value to 3 positive and 3 negative speed values
  if ((readX != readXlast) ) { //check if it has changed
    if (readX == -3) {
      Serial1.print(fast * -1 * directionX); Serial1.print("X"); Serial1.print(",");
      readXlast = readX;
    }
    if (readX == -2)  {
      Serial1.print(medium * -1 * directionX); Serial1.print("X"); Serial1.print(",");
      readXlast = readX;
    }
    if (readX == -1)  {
      Serial1.print(slow * -1 * directionX); Serial1.print("X"); Serial1.print(",");
      readXlast = readX;
    }
    if (readX == 0) {
      Serial1.print(0); Serial1.print("O"); Serial1.print(","); //stop the stepper motor by sending "0O,"
      readXlast = readX;
    }
    if (readX == 1)  {
      Serial1.print(slow * directionX); Serial1.print("X"); Serial1.print(",");
      readXlast = readX;
    }

    if (readX == 2)  {
      Serial1.print(medium * directionX); Serial1.print("X"); Serial1.print(",");
      readXlast = readX;
    }

    if (readX == 3)  {
      Serial1.print(fast * directionX); Serial1.print("X"); Serial1.print(",");
      readXlast = readX;
    }

  }

// REPEAT THE SAME FOR ALL OTHER JOYSTICK POTENTIOMETERS 

  readY = map((analogRead(joystickY)), 0, 1023, -3, 3);
  if (readY != readYlast) {
    if (readY == -3)  {
      Serial1.print(fast * -1 * directionY); Serial1.print("Y"); Serial1.print(",");
      readYlast = readY;
    }
    if (readY == -2)  {
      Serial1.print(medium * -1 * directionY); Serial1.print("Y"); Serial1.print(",");
      readYlast = readY;
    }
    if (readY == -1)  {
      Serial1.print(slow * -1 * directionY); Serial1.print("Y"); Serial1.print(",");
      readYlast = readY;
    }
    if (readY == 0) {
      Serial1.print(0); Serial1.print("O"); Serial1.print(",");
      readYlast = readY;
    }
    if (readY == 1)  {
      Serial1.print(slow * directionY); Serial1.print("Y"); Serial1.print(",");
      readYlast = readY;
    }

    if (readY == 2)  {
      Serial1.print(medium * directionY); Serial1.print("Y"); Serial1.print(",");
      readYlast = readY;
    }

    if (readY == 3)  {
      Serial1.print(fast * directionY); Serial1.print("Y"); Serial1.print(",");
      readYlast = readY;
    }

  }

  readC = map((analogRead(joystickC)), 0, 1023, -3, 3);
  if (readC != readClast) {
    if (readC == -3)  {
      Serial1.print(fast * -1 * directionC); Serial1.print("C"); Serial1.print(",");
      readClast = readC;
    }
    if (readC == -2)  {
      Serial1.print(medium * -1 * directionC); Serial1.print("C"); Serial1.print(",");
      readClast = readC;
    }
    if (readC == -1) {
      Serial1.print(slow * -1 * directionC); Serial1.print("C"); Serial1.print(",");
      readClast = readC;
    }
    if (readC == 0)  {
      Serial1.print(0); Serial1.print("O"); Serial1.print(",");
      readClast = readC;
    }
    if (readC == 1)  {
      Serial1.print(slow * directionC); Serial1.print("C"); Serial1.print(",");
      readClast = readC;
    }

    if (readC == 2)  {
      Serial1.print(medium * directionC); Serial1.print("C"); Serial1.print(",");
      readClast = readC;
    }

    if (readC == 3) {
      Serial1.print(fast * directionC); Serial1.print("C"); Serial1.print(",");
      readClast = readC;
    }

  }

  readZ = map((analogRead(joystickZ)), 0, 1023, -3, 3);
  if (readZ != readZlast) {
    if (readZ == -3)  {
      Serial1.print(fast * -1 * directionZ); Serial1.print("Z"); Serial1.print(",");
      readZlast = readZ;
    }
    if (readZ == -2)  {
      Serial1.print(medium * -1 * directionZ); Serial1.print("Z"); Serial1.print(",");
      readZlast = readZ;
    }
    if (readZ == -1)  {
      Serial1.print(slow * -1 * directionZ); Serial1.print("Z"); Serial1.print(",");
      readZlast = readZ;
    }
    if (readZ == 0) {
      Serial1.print(0); Serial1.print("O"); Serial1.print(",");
      readZlast = readZ;
    }
    if (readZ == 1) {
      Serial1.print(slow * directionZ); Serial1.print("Z"); Serial1.print(",");
      readZlast = readZ;
    }

    if (readZ == 2)  {
      Serial1.print(medium * directionZ); Serial1.print("Z"); Serial1.print(",");
      readZlast = readZ;
    }

    if (readZ == 3)  {
      Serial1.print(fast * directionZ); Serial1.print("Z"); Serial1.print(",");
      readZlast = readZ;
    }

  }

  readR = map((analogRead(joystickR)), 0, 1023, -3, 3);
  if (readR != readRlast)  {
    if (readR == -3) {
      Serial1.print(fast * -1 * directionR); Serial1.print("R"); Serial1.print(",");
      readRlast = readR;
    }
    if (readR == -2) {
      Serial1.print(medium * -1 * directionR); Serial1.print("R"); Serial1.print(",");
      readRlast = readR;
    }
    if (readR == -1)  {
      Serial1.print(slow * -1 * directionR); Serial1.print("R"); Serial1.print(",");
      readRlast = readR;
    }
    if (readR == 0) {
      Serial1.print(0); Serial1.print("O"); Serial1.print(",");
      readRlast = readR;
    }
    if (readR == 1)  {
      Serial1.print(slow * directionR); Serial1.print("R"); Serial1.print(",");
      readRlast = readR;
    }

    if (readR == 2)  {
      Serial1.print(medium * directionR); Serial1.print("R"); Serial1.print(",");
      readRlast = readR;
    }

    if (readR == 3) {
      Serial1.print(fast * directionR); Serial1.print("R"); Serial1.print(",");
      readRlast = readR;
    }

  }

  readT = map((analogRead(joystickT)), 0, 1023, -3, 3);
  if (readT != readTlast)  {
    if (readT == -3) {
      Serial1.print(fast * -1 * directionT); Serial1.print("T"); Serial1.print(",");
      readTlast = readT;
    }
    if (readT == -2) {
      Serial1.print(medium * -1 * directionT); Serial1.print("T"); Serial1.print(",");
      readTlast = readT;
    }
    if (readT == -1) {
      Serial1.print(slow * -1 * directionT); Serial1.print("T"); Serial1.print(",");
      readTlast = readT;
    }
    if (readT == 0)  {
      Serial1.print(0); Serial1.print("O"); Serial1.print(",");
      readTlast = readT;
    }
    if (readT == 1)  {
      Serial1.print(slow * directionT); Serial1.print("T"); Serial1.print(",");
      readTlast = readT;
    }

    if (readT == 2)  {
      Serial1.print(medium * directionT); Serial1.print("T"); Serial1.print(",");
      readTlast = readT;
    }

    if (readT == 3)  {
      Serial1.print(fast * directionT); Serial1.print("T"); Serial1.print(",");
      readTlast = readT;
    }

  }

// LED INTENSITY

  readL = map((analogRead(potLED)), 0, 1023, 0, 20);
  if (readL != readLlast) {
    Serial1.print(readL); Serial1.print("L"); Serial1.print(",");
    display.clearDisplay();
    display.setCursor(0, 10);
    display.setTextSize(2);
    display.print("LED:"); display.print(100 - (readL * 5)); display.print("%");
    display.display();
    readLlast = readL;
  }

}
