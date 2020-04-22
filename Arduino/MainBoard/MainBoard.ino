/* This code controls the stepper motors and the LED intensity
 * according to the data received from the UART (RX, TX) (Controller) or the USB (e.g. Raspberry Pi)
 * All 6 motors share the same stepper motor pins, but only one
 * is activated depending on the status of the Enable pin of the driver
 * Data format: "50X," where 50 is the speed and X is the axis
 * X: X axis, Y: Y axis, Z: Z axis, C: camera stage, R: rotation, T: tilt 
 * The data is executed after a "," (comma) is received
 */

// Arduino board: Adafruit ItsyBitsy32u4 5V 

#include <AccelStepper.h> // Library to control stepper motors // Copyright (C) 2010-2018 Mike McCauley

AccelStepper myStepper(AccelStepper::FULL4WIRE, 12, 9, 11, 7); // stepper motor pins

#include <Adafruit_NeoPixel.h> // Library to control the optional status LED

////////////////// Arduino pins connected to the EN of each stepper motor driver

#define EnableX 10
#define EnableY A3
#define EnableZ 5
#define EnableC A4
#define EnableR 4
#define EnableT A5

/////////////////

#define LEDPWM 6 // High-power illumination LED PWM control pin
#define NeopixelPin 8 // # Status LED (optional, not used)

const int stepsPerRevolution = 2048; //set according to the specs of the stepper motor
String receivedString; //incoming data from the serial ports (USB or UART)
int stepperSpeed;
char state; //variable to define the active stepper motor, X, Y, Z, C, R, T
int LEDintensity;

Adafruit_NeoPixel pixel(1, NeopixelPin, NEO_GRB + NEO_KHZ800);


void setup() {

  Serial1.begin(57600); // UART with the jostick controller
  Serial.begin(57600); // USB, Raspberry Pi
  
  pinMode(EnableX, OUTPUT);
  pinMode(EnableY, OUTPUT);
  pinMode(EnableC, OUTPUT);
  pinMode(EnableZ, OUTPUT);
  pinMode(EnableR, OUTPUT);
  pinMode(EnableT, OUTPUT);
  pinMode(LEDPWM, OUTPUT);

  analogWrite(LEDPWM, 255); // initialize the LED intensity to 0% 
  /* PWM of the LED driver (RCD-24-0.70/PL/B, Recom) used in this project
   * works in the opposite direction, i.e. 255 is for 0% and 0 for 100% intensity
   */
  
  pixel.begin();
  pixel.setPixelColor(0, pixel.Color(0, 0, 50));
  pixel.show();
  
  myStepper.setMaxSpeed(1000);
}

void loop() {

////// data from UART: joystick controller

  if (Serial1.available() > 0) {
    char data = Serial1.read();
    if (data == ',') { // receive data until "," is received

      if (receivedString.length() > 1) {
        int stepperSpeed = receivedString.toInt(); // convert the motor speed information to an integer
        myStepper.setSpeed(stepperSpeed); // set the speed
        
        if (receivedString.indexOf('X') > 0) { //X axis
          state = 'X';
        }
        if (receivedString.indexOf('Y') > 0) { //Y axis
          state = 'Y';
        }
        if (receivedString.indexOf('C') > 0) { //Camera stage
          state = 'C';
        }

        if (receivedString.indexOf('Z') > 0) { //Z axis
          state = 'Z';
        }

        if (receivedString.indexOf('R') > 0) { //Rotation stage
          state = 'R';
        }

        if (receivedString.indexOf('T') > 0) { //Tilting mechanism
          state = 'T';
        }

        if (receivedString.indexOf('L') > 0) { //LED
          LEDintensity = map(stepperSpeed, 0, 20, 0, 255); //map to a number from 0 to 255
          analogWrite(LEDPWM, LEDintensity);
        }

        if (receivedString.indexOf('O') > 0) { //Disable all stepper motors
          state = 'O';
        }
        receivedString = ""; //reset the string for the next incoming data
      }
    }
    else {
      receivedString += data; //store the incoming data in a string until "," is received (above)
    }
  }

//// data from USB (Raspberry Pi) (same as above but Serial instead of Serial1)

  if (Serial.available() > 0) {
    char data = Serial.read();
    if (data == ',') {

      if (receivedString.length() > 1) {
        int stepperSpeed = receivedString.toInt();
        myStepper.setSpeed(stepperSpeed);
        if (receivedString.indexOf('X') > 0) {
          state = 'X';
        }
        if (receivedString.indexOf('Y') > 0) {
          state = 'Y';
        }
        if (receivedString.indexOf('C') > 0) {
          state = 'C';
        }

        if (receivedString.indexOf('Z') > 0) {
          state = 'Z';
        }

        if (receivedString.indexOf('R') > 0) {
          state = 'R';
        }

        if (receivedString.indexOf('T') > 0) {
          state = 'T';
        }

        if (receivedString.indexOf('L') > 0) {
          LEDintensity = map(stepperSpeed, 0, 20, 0, 255);
          analogWrite(LEDPWM, LEDintensity);
        }

        if (receivedString.indexOf('O') > 0) {
          state = 'O';
        }
        receivedString = "";
      }
    }
    else {
      receivedString += data;
    }
  }

///// RUN STEPPER MOTORS, only one runs at a given time

  if (state == 'X') {

    digitalWrite(EnableX, HIGH);
    myStepper.runSpeed();
  }

  if (state == 'Y') {

    digitalWrite(EnableY, HIGH);
    myStepper.runSpeed();
  }

  if (state == 'C') {

    digitalWrite(EnableC, HIGH);
    myStepper.runSpeed();
  }

  if (state == 'Z') {

    digitalWrite(EnableZ, HIGH);
    myStepper.runSpeed();
  }

  if (state == 'R') {

    digitalWrite(EnableR, HIGH);
    myStepper.runSpeed();
  }

  if (state == 'T') {

    digitalWrite(EnableT, HIGH);
    myStepper.runSpeed();
  }

// TURN OFF all stepper motors 

  if (state == 'O') {
    digitalWrite(EnableX, LOW);
    digitalWrite(EnableY, LOW);
    digitalWrite(EnableZ, LOW);
    digitalWrite(EnableC, LOW);
    digitalWrite(EnableR, LOW);
    digitalWrite(EnableT, LOW);
  }

}
