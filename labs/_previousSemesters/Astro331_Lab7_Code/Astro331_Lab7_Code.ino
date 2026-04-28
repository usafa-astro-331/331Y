/* Astro331_Lab6_Code.ino
* ===========================================================
* Created by: Lt Col Wyatt Harris, Fall 2025
* Description: Use this code IAW Astro 331 Lab 6 instructions.
* Project: Attitude Determination Lab
* Install Requirements:
*    Board Definition:
*	    Rapsberry Pi Pico by Earle F. Philhower, III v5.0.0
*
*    Libraries (allow install of dependencies):
*	    RTClib v2.1.4	
*	    Adafruit ADS1X15 v2.6.0
*     SparkFun ICM-20948 Arduino Library v1.3.2
*     PicoEncoder v1.1.1
*     TB9051FTGMotorCarrier v1.0.2
* =========================================================== */

/*---------------------------------------------------------------------------------------------*/
// Options:
/*---------------------------------------------------------------------------------------------*/
#define USE_SUN_SENSOR    0   // 1 to use sun sensor, 0 to turn off
#define MOTOR_VOLTAGE     5.0 // Voltage provided to Motor Controller

/*---------------------------------------------------------------------------------------------*/
// Library includes:
/*---------------------------------------------------------------------------------------------*/
#include <Wire.h>              // libray for I2C communication
#include "RTClib.h"            // Date/time functions using a PCF8523 RTC connected via I2C
#include <SD.h>                // SD Card library
#if USE_SUN_SENSOR
  #include <Adafruit_ADS1X15.h>  // ADC Library
#endif
#include <ICM_20948.h>         // IMU Library
#include "sd_functions.h"      //SD helper functions (this directory)
// #include "pio_encoder.h"       //Motor encoder library
#include "PicoEncoder.h"
#include <TB9051FTGMotorCarrier.h>

/*---------------------------------------------------------------------------------------------*/
// Configuration:
/*---------------------------------------------------------------------------------------------*/
#define SD_CS_PIN 17  // Chip select pin for the microSD card on PiCowbell
#define LED_PIN   15  // Led GPIO pin
#define ENCODER_PIN_A 10 // GPIO pin connected to Encoder Pin A
#define  ENCODER_PIN_B = 11 // GPIO pin connected to Encoder Pin B (must be consecutive w/Pin A)

/*---------------------------------------------------------------------------------------------*/
// Type Definitions:
/*---------------------------------------------------------------------------------------------*/
const float RAD2DEG = 57.2957795f;

/*---------------------------------------------------------------------------------------------*/
// Globals:
/*---------------------------------------------------------------------------------------------*/
// Objects:
RTC_PCF8523 rtc;                    // Clock object to get real local time
File dataFile;                      // data file object to log data to SD card
#if USE_SUN_SENSOR
  Adafruit_ADS1015 ads1015;         // ADC Object
#endif
ICM_20948_I2C myICM;                // IMU object
PicoEncoder encoder;                // Motor Encoder Object

// Motor Variables/Object
static constexpr uint8_t pwm2Pin{8}; // PWM2
static constexpr uint8_t pwm1Pin{9}; // PWM1
static TB9051FTGMotorCarrier driver{ pwm1Pin, pwm2Pin };// Instantiate TB9051FTGMotorCarrier


// Variables:
uint32_t timeNext_testPoint;        // time of next test point (ms)
uint32_t interval_testPoint = 50;  // time interval between test points (ms)

uint32_t prevPrintTime;
uint32_t prevBlinkTime;

float gyro_Z = 0.0; // deg/s
float mag_X = 0.0;  // uT
float mag_Y = 0.0;  // uT

int16_t sun_plusX, sun_plusY, sun_minusX, sun_minusY;  // phototransistor ADC values
float sun_X, sun_Y;
float sun_direction = 0.0;

uint32_t period_start_us; // timer variable for encoder calibration

float w_RW_cmd; // commanded RW speed (RPM)
float w_RW_meas; // measured RW speed (RPM)

uint32_t t0; // Test start time (msec)

/*---------------------------------------------------------------------------------------------*/
// Function Prototypes (see defintiions after loop()):
/*---------------------------------------------------------------------------------------------*/
// void runTest();

/////////////////////////////////////////////////////////////////////////////////////////////////
// SETUP:
/////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {

  pinMode(LED_PIN, OUTPUT);

  Wire.begin();
  Wire.setClock(400000);

  Serial.begin(115200);  // Begin Serial communication with computer
  // while (!Serial) {delay(10);} // Wait for user to open Serial monitor before proceeding

  //----------------------------------------------
  // Initialize Serial link wiht XBee
  //----------------------------------------------
  // Begin Serial1 for MCU <> XBee communication:
  Serial1.setTX(12);    // set TX pin of MCU
  Serial1.setRX(13);    // set RX pin of MCU
  Serial1.begin(9600);  // 2nd serial port to XBee radio
  // clear terminal screen:
  Serial1.write(27);     // ESC command
  Serial1.print("[2J");  // clear screen command
  Serial1.write(27);
  Serial1.print("[H");  // cursor to home command
  Serial1.println("[INFO] KestrelSAT ONLINE.");

  //----------------------------------------------
  // Initialize Local-Time Clock
  //----------------------------------------------
  if (!rtc.begin()) {
    while (1) {
      Serial.print("Couldn't find RTC");
      delay(1000);
    }
  } else {
    Serial.println("[INFO] Clock Initialized.");
  }
  // If required, reset RTC time:
  if (rtc.lostPower()) {
    Serial.println("[INFO] RTC is NOT initialized, setting the local time.");
    // Set RTC to compile time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  //----------------------------------------------

  //----------------------------------------------
  // Initialize ADS1015 for Sun Sensor
  //----------------------------------------------
  #if USE_SUN_SENSOR
    ads1015.begin();
  #endif
  //----------------------------------------------

  //----------------------------------------------
  // Initialize ICM20948
  //----------------------------------------------
  if (myICM.begin(Wire, 1) != ICM_20948_Stat_Ok) {
    Serial.println("[CAUTION] IMU not found.");
    while (1)
      ;
  } else {
    Serial.println("[INFO] IMU Initialized.");
  }
  //----------------------------------------------

  //----------------------------------------------
  // Initialize SD Card
  //----------------------------------------------
  if (!SD.begin(SD_CS_PIN)) {
    while (1) {
      Serial.println("[ERROR] SD card initialization failed. Card present?");
      delay(2000);
    }
  } else {
    Serial.println("[INFO] SD Card Initialized.");
  }
  //----------------------------------------------

  //----------------------------------------------
  // Initialize Reaction Wheel
  //----------------------------------------------
  encoder.begin(ENCODER_PIN_A);
  period_start_us = micros();
  driver.enable(); // ----- TB9051FTG Motor Carrier
  driver.setOutput(0);
  //----------------------------------------------

  Serial.println("[INFO] SETUP COMPLETE.");
  Serial1.println("[INFO] SETUP COMPLETE.");

  digitalWriteFast(LED_PIN, HIGH);
}
/////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN LOOP:
/////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // Serial.println(encoder.getCount());
  // delay(10);
  bool newUserInput = false;
  // Check for User Input:
  char c;
  if (Serial.available() > 0) {  // Check for user input from USB
    c = Serial.read();
    newUserInput = true;
  }
  if (Serial1.available() > 0) {  // Check for user input from USB
    c = Serial1.read();
    newUserInput = true;
  }
  if(newUserInput){
    newUserInput = false;
    switch (c) {
      case 'A':  // run test
        //--------
        Serial.println("[INFO] Beginning test A.");
        Serial1.println("[INFO] Beginning test A.");
        runTestA();
        //--------
        break;
      case 'B':  // run test
        //--------
        Serial.println("[INFO] Beginning test B.");
        Serial1.println("[INFO] Beginning test B.");
        runTestB();
        //--------
        break;
      default:
        Serial.println("[CAUTION] Invalid Input.");
        Serial1.println("[CAUTION] Invalid Input.");
        break;
    }
  }

  if (millis() - prevPrintTime > 5000) {  // Cycle LED and remind user of instructions
    prevPrintTime = millis();
    Serial.println("[OPTIONS] SEND 'A' TO START TEST A, SEND 'B' TO START TEST B, SEND 'X' TO STOP TEST.");
    Serial1.println("[OPTIONS] SEND 'A' TO START TEST A, SEND 'B' TO START TEST B, SEND 'X' TO STOP TEST.");
  }

  if (millis() - prevBlinkTime > 500){ //Blink LED to indicate funcitoning
    prevBlinkTime = millis();
    digitalWriteFast(LED_PIN, !digitalRead(LED_PIN));
    Serial1.println("."); 
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCITON DEFINITIONS:
/////////////////////////////////////////////////////////////////////////////////////////////////

