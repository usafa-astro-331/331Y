/*
* ===========================================================
* Created by: Lt Col Wyatt Harris, Fall 2025
* Description: Use this code IAW Astro 331 Lab 6 instructions.
* Project: Attitude Determination Lab
* Install Requirements:

* =========================================================== */


/*---------------------------------------------------------------------------------------------*/
// Library includes:
/*---------------------------------------------------------------------------------------------*/
#include <Arduino.h>
#include <Wire.h>                     // libray for I2C communication
#include <SD.h>                       // SD Card library
#include <ICM_20948.h>                // IMU Library
#include <TB9051FTGMotorCarrier.h>    // Motor controller Library

#include "CONFIG.h"
#include "sd_functions.h"             //SD helper functions




/*---------------------------------------------------------------------------------------------*/
// Type Definitions:
/*---------------------------------------------------------------------------------------------*/
File dataFile;                      // data file object to log data to SD card
ICM_20948_I2C myICM;                // IMU object

// Motor Variables/Object
// static constexpr uint8_t pwm2Pin{GPIO_NUM_32}; // PWM2
// static constexpr uint8_t pwm1Pin{GPIO_NUM_33}; // PWM1
static constexpr uint8_t pwm2Pin{6}; // PWM2
static constexpr uint8_t pwm1Pin{10}; // PWM1
static TB9051FTGMotorCarrier driver{ pwm1Pin, pwm2Pin };// Instantiate TB9051FTGMotorCarrier


/*---------------------------------------------------------------------------------------------*/
// Globals:
/*---------------------------------------------------------------------------------------------*/
uint32_t prevPrintTime;
uint32_t prevBlinkTime;


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
  // Initialize Serial link with XBee
  //----------------------------------------------
  //
  //----------------------------------------------

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
  // if (!SD.begin(SD_CS_PIN)) {
  //   while (1) {
  //     Serial.println("[ERROR] SD card initialization failed. Card present?");
  //     delay(2000);
  //   }
  // } else {
  //   Serial.println("[INFO] SD Card Initialized.");
  // }
  //----------------------------------------------

  //----------------------------------------------
  // Initialize Reaction Wheel
  //----------------------------------------------
  // encoder.begin(ENCODER_PIN_A);
  // period_start_us = micros();
  driver.enable(); // ----- TB9051FTG Motor Carrier
  driver.setOutput(0.2);
  //----------------------------------------------

  Serial.println("[INFO] SETUP COMPLETE.");

  digitalWrite(LED_PIN, HIGH);
}
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN LOOP:
/////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  
  if (Serial.available()) {
    float speed_pwm = Serial.parseFloat();   // waits for integer input
    Serial.print("You entered: ");
    Serial.println(speed_pwm);

    driver.setOutput(speed_pwm);
  }
  
  // Serial.println(encoder.getCount());
  // delay(10);
  
  
  // bool newUserInput = false;
  // // Check for User Input:
  // char c;
  // if (Serial.available() > 0) {  // Check for user input from USB
  //   c = Serial.read();
  //   newUserInput = true;
  // }
  // if (Serial1.available() > 0) {  // Check for user input from USB
  //   c = Serial1.read();
  //   newUserInput = true;
  // }
  // if(newUserInput){
  //   newUserInput = false;
  //   switch (c) {
  //     case 'A':  // run test
  //       //--------
  //       Serial.println("[INFO] Beginning test A.");
  //       Serial1.println("[INFO] Beginning test A.");
  //       runTestA();
  //       //--------
  //       break;
  //     case 'B':  // run test
  //       //--------
  //       Serial.println("[INFO] Beginning test B.");
  //       Serial1.println("[INFO] Beginning test B.");
  //       runTestB();
  //       //--------
  //       break;
  //     default:
  //       Serial.println("[CAUTION] Invalid Input.");
  //       Serial1.println("[CAUTION] Invalid Input.");
  //       break;
  //   }
  // }

  // if (millis() - prevPrintTime > 5000) {  // Cycle LED and remind user of instructions
  //   prevPrintTime = millis();
  //   Serial.println("[OPTIONS] SEND 'A' TO START TEST A, SEND 'B' TO START TEST B, SEND 'X' TO STOP TEST.");
  //   Serial1.println("[OPTIONS] SEND 'A' TO START TEST A, SEND 'B' TO START TEST B, SEND 'X' TO STOP TEST.");
  // }

  if (millis() - prevBlinkTime > 500){ //Blink LED to indicate funcitoning
    prevBlinkTime = millis();
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    // Serial1.println("."); 
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////


