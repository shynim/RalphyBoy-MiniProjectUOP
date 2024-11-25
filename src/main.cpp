#include <Arduino.h>
#include <QTRSensors.h>
#include <MotorDriver.h>
#include <PID.h>
#include <SensorPanel.h>
#include <Wire.h>
#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
SensorPanel frontQtr(const_cast<uint8_t *>((const uint8_t[]) {30, 29, 28, 27, 26, 24, 25, 23}));
SensorPanel checkQtr(const_cast<uint8_t *>((const uint8_t[]) {9, 11, 10 ,15, 17, 16, 14, 12}));

MotorDriver driver;

const int buzzPin = A0;

const int rightPins[] = {5, 45, 44};
const int leftPins[] = {4, 47, 46};

const int leftEncoderPins[] = {2, 3};
const int rightEncoderPins[] = {18, 19};

unsigned long leftEncoder = 1;
unsigned long rightEncoder = 1;
unsigned long leftCount = 1;
unsigned long rightCount = 1;

const int led1RedPin = 42;
const int led1GreenPin = 40;
const int led1BluePin = 38;

const int led2RedPin = A2;
const int led2GreenPin = A3;
const int led2BluePin = A4;

void countLeftOut1(){
  leftEncoder += 1;
}

void countRightOut1(){
  rightEncoder += 1;
}

int absss(int value){
  if(value < 0){
    return -value;
  }else{
    return value;
  }
}

void beep(int endDelay = 0){
  digitalWrite(buzzPin, HIGH);
  delay(200);
  digitalWrite(buzzPin, LOW);
  delay(endDelay);
}

// Function to turn off LED1
void turnOffLED1() {
  digitalWrite(led1RedPin, HIGH);
  digitalWrite(led1GreenPin, HIGH);
  digitalWrite(led1BluePin, HIGH);
}

// Function to turn off LED2
void turnOffLED2() {
  digitalWrite(led2RedPin, HIGH);
  digitalWrite(led2GreenPin, HIGH);
  digitalWrite(led2BluePin, HIGH);
}

// Function to control LED1 based on color character
void setLED1(char colour) {
  turnOffLED1(); // Turn off LED1 before setting color
  switch (colour) {
    case 'R': // Red
      digitalWrite(led1RedPin, LOW);
      break;
    case 'G': // Green
      digitalWrite(led1GreenPin, LOW);
      break;
    case 'B': // Blue
      digitalWrite(led1BluePin, LOW);
      break;
    case 'O': // Off
      // Already turned off
      break;
    default:
      // Handle invalid color input if needed
      break;
  }
}

// Function to control LED2 based on color character
void setLED2(char colour) {
  turnOffLED2(); // Turn off LED2 before setting color
  switch (colour) {
    case 'R': // Red
      digitalWrite(led2RedPin, LOW);
      break;
    case 'G': // Green
      digitalWrite(led2GreenPin, LOW);
      break;
    case 'B': // Blue
      digitalWrite(led2BluePin, LOW);
      break;
    case 'O': // Off
      // Already turned off
      break;
    default:
      // Handle invalid color input if needed
      break;
  }
}

void initEncoder(){
  leftCount = 1;
  rightCount = 1;
  leftEncoder = 1;
  rightEncoder = 1;
}

void autoCalibrate(int loops = 2){
  int loopCount = 0;
  while(loopCount < loops){
    
    initEncoder();
    while(leftEncoder < 150){
      driver.turnLeft(70, 70);
      frontQtr.calibrate();
      checkQtr.calibrate();
    }
    driver.stop(50);

    initEncoder();
    while(leftEncoder < 280){
      driver.turnRight(70, 70);
      frontQtr.calibrate();
      checkQtr.calibrate();
    }
    driver.stop(50);

    initEncoder();
    while(leftEncoder < 150){
      driver.turnLeft(70, 70);
      frontQtr.calibrate();
      checkQtr.calibrate();
    }
    driver.stop(50);
    loopCount++;
    delay(100);
  }
  
}

int readTof() {
  VL53L0X_RangingMeasurementData_t measure;
    
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    return measure.RangeMilliMeter;
  } else {
    return 999999;
  }
  
}

void dottedLine(){
  int correction;
  digitalWrite(A0,HIGH);

  if(checkQtr.pattern == 'T' && absss(frontQtr.error) < 1000 ){
    
    while(checkQtr.pattern == 'T'){
      checkQtr.read();
      frontQtr.read();
      driver.forward(230, 230);
    }
    while(frontQtr.pattern == 'T'){
      checkQtr.read();
      frontQtr.read();
      driver.forward(230, 230);
    }

  }
  
}

void botSetup(){
  while (! Serial) {
    delay(1);
  }
  
  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }

  driver.init(const_cast<int *>(leftPins), const_cast<int *>(rightPins));

  pinMode(leftEncoderPins[0], INPUT);
  pinMode(leftEncoderPins[1], INPUT);
  pinMode(rightEncoderPins[0], INPUT);
  pinMode(rightEncoderPins[1], INPUT);

  pinMode(buzzPin, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(leftEncoderPins[0]), countLeftOut1, RISING);
  attachInterrupt(digitalPinToInterrupt(leftEncoderPins[1]), countLeftOut1, RISING);
  attachInterrupt(digitalPinToInterrupt(rightEncoderPins[0]), countRightOut1, RISING);
  attachInterrupt(digitalPinToInterrupt(rightEncoderPins[1]), countRightOut1, RISING);
  
  pinMode(LED_BUILTIN,OUTPUT);

  pinMode(led1RedPin, OUTPUT);
  pinMode(led1GreenPin, OUTPUT);
  pinMode(led1BluePin, OUTPUT);

  pinMode(led2RedPin, OUTPUT);
  pinMode(led2GreenPin, OUTPUT);
  pinMode(led2BluePin, OUTPUT);

  Serial.begin(9600);

  autoCalibrate(2);
  delay(200);
  beep(100);
  beep();

  while(readTof() > 100){
    
  }
  beep();

}

void botLoop() {
  checkQtr.read();
  frontQtr.read();
  
  //dottedLine();
  digitalWrite(A0, LOW);

  int diff = checkQtr.position - frontQtr.position;
  int correction = 0;

  int baseSpeed = 138;
  if(absss(diff) < 1500 && absss(frontQtr.error) < 1500){
    setLED1('G');
    setLED2('G');
    baseSpeed = 230;
    correction = pid(frontQtr.error, 'f');

  }else{
    setLED1('R');
    setLED2('R');
    baseSpeed = 138;
    correction = pid(frontQtr.error, 's');

  }
  
  driver.applyLinePid(correction, baseSpeed);

}

void setup(){
  botSetup();
  
}

void loop(){
  botLoop();
  //driver.forward(255,255);
  
}
