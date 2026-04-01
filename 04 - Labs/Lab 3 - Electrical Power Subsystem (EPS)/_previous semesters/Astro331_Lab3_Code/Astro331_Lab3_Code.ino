/* Astro331_Lab3_Code.ino
* ===========================================================
* Created by: Lt Col Wyatt Harris, Fall 2025
* Description: Use this code IAW Astro 331 Lab 3 instructions.
* Project: Electrical Power Subsystem (EPS) Lab
* Install Requirements:
*    Board Definition:
*	    Rapsberry Pi Pico by Earle F. Philhower, III v5.0.0
*
*    Libraries (allow install of dependencies):
*	    RTClib v2.1.4	
*	    Adafruit INA219 v1.2.3
* =========================================================== */

/*---------------------------------------------------------------------------------------------*/
// Library includes:
/*---------------------------------------------------------------------------------------------*/
#include <Wire.h>             // libray for I2C communication
#include "RTClib.h"           // Date/time functions using a PCF8523 RTC connected via I2C 
#include <SD.h>               // SD Card library
#include <Adafruit_INA219.h>  //Adafruit library to use INA219 current/voltage sensor
#include "sd_functions.h"     //SD helper functions (this directory)

/*---------------------------------------------------------------------------------------------*/
// Configuration:
/*---------------------------------------------------------------------------------------------*/
#define SD_CS_PIN 17  // Chip select pin for the microSD card on PiCowbell

/*---------------------------------------------------------------------------------------------*/
// Type Definitions:
/*---------------------------------------------------------------------------------------------*/
struct electrical_measurement // data structure to hold measurements from INA219
{
  float current_mA; 
  float voltage_V; 
  float power_mW;  
};
/* Example:
electrical_measurement data;
Serial.println(data.voltage_V);
*/

/*---------------------------------------------------------------------------------------------*/
// Globals:
/*---------------------------------------------------------------------------------------------*/
// Objects:
RTC_PCF8523 rtc;  // Clock object to get real local time
File dataFile;    // data file object to log data to SD card
Adafruit_INA219 ina219; // object to use INA219 sensor

// Variables:
uint32_t timeNext_testPoint; // time of next test point (ms)
uint32_t interval_testPoint = 100; // time interval between test points (ms)

const int num_samples_per_testpoint = 50; // number of samples per testpoint to average over

/*---------------------------------------------------------------------------------------------*/
// Function Prototypes (see defintiions after loop()):
/*---------------------------------------------------------------------------------------------*/
void displayMenu();
void runTest();
electrical_measurement INA219reading();

/////////////////////////////////////////////////////////////////////////////////////////////////
// SETUP:
/////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200); // Begin Serial communication with computer
  while (!Serial) {delay(10);} // Wait for user to open Serial monitor before proceeding

  //----------------------------------------------
  // Initialize Local-Time Clock
  //----------------------------------------------
  if (!rtc.begin()) {
    while (1){Serial.print("Couldn't find RTC"); delay(1000);}
  } else {Serial.println("[INFO] Clock Initialized.");}
  // If required, reset RTC time:
  if (rtc.lostPower()) {
    Serial.println("[INFO] RTC is NOT initialized, setting the local time.");
    // Set RTC to compile time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  //----------------------------------------------

  //----------------------------------------------
  // Initialize SD Card
  //----------------------------------------------
  if (!SD.begin(SD_CS_PIN)) {
      while (1) { Serial.println("[ERROR] SD card initialization failed. Card present?"); delay(2000); }
  } else {Serial.println("[INFO] SD Card Initialized.");}
  //----------------------------------------------

  //----------------------------------------------
  // Initialize INA219 Voltage/Current Sensor
  // ----------------------------------------------
  if (!ina219.begin()) {	
      while (1) { Serial.println("[ERROR] Failed to find INA219. "); delay(2000); }
  } else {Serial.println("[INFO] INA219 Initialized.");}
  ina219.setCalibration_32V_1A(); // Set Voltage/Current range
  // ----------------------------------------------

  Serial.println("[INFO] SETUP COMPLETE.");
  
  displayMenu();
}
/////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN LOOP:
/////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  
  if(Serial.available()>0){ // if user sends character, parse it
    char c = Serial.read();
    switch(c){
      case 'S': // run test
        //--------
        Serial.println("[INFO] Beginning test.");        
        runTest();
        //--------
        break;
      case 'L': // list files
        //--------
        Serial.println("[INFO] Listing files on SD card...");
        sd_listFiles(SD.open("/"), 0);
        //--------
        break;
      case 'P': // print file selected by user
        //--------
        sd_printFileMenu();
        //--------
        break;
      default:
        Serial.println("[CAUTION] Invalid Input.");
        break;
    }
    displayMenu();
  }

}
/////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCITON DEFINITIONS:
/////////////////////////////////////////////////////////////////////////////////////////////////

/*---------------------------------------------------------------------------------------------*/
// Display Menu:
/*---------------------------------------------------------------------------------------------*/
/**
* @brief Displays Main Menu
* @return none
*/
void displayMenu(){
  Serial.println("\n--------------------------");
  Serial.println("[MENU]");
  Serial.println("  Send 'S' to Start test.");
  Serial.println("  Send 'L' to List files on SD card.");
  Serial.println("  Send 'P' to Print file from SD.");
  Serial.println("--------------------------\n");
} // end function displayMenu()


/*---------------------------------------------------------------------------------------------*/
// Run Test:
/*---------------------------------------------------------------------------------------------*/
/**
* @brief Runs the test
* @return none
*/
void runTest(){
  sd_createDataFile(rtc, &dataFile);
  
  Serial.println("[INFO] Send any key to start. ***Send 'X' to stop test.***");
  while(!Serial.available()){delay(10);} // Wait for user to start test

  while(true){
    
    if(Serial.available()>0){ // Check for user input
      char c = Serial.read();
      switch(c){
        case 'X':
          dataFile.close();
          Serial.println("[INFO] Test Complete.");
          return;
        default:
          Serial.println("[CAUTION] Invalid Input, continuing test...");
          break;
      }
    }
    
    if(millis() > timeNext_testPoint){ // Collect Test Point loop
      timeNext_testPoint += interval_testPoint; // Update time for next Test Point

      // Collect Test Point (each reading takes ~ 1ms):
      electrical_measurement ina219data;
      float testPoint_current_mA = 0.0; 
      float testPoint_voltage_V = 0.0; 
      for (int ii = 0; ii < num_samples_per_testpoint; ii++){ // sum X readings
        ina219data = INA219reading();
        testPoint_current_mA += ina219data.current_mA;
        testPoint_voltage_V += ina219data.voltage_V;
      }
      testPoint_current_mA /= num_samples_per_testpoint; // average readings 
      testPoint_voltage_V /= num_samples_per_testpoint; // average readings

      // Print data to file:
      dataFile.print(millis());
      dataFile.print(",");
      dataFile.print(testPoint_current_mA,6);
      dataFile.print(",");
      dataFile.println(testPoint_voltage_V,6);
      dataFile.flush(); // save file

      //Print to Serial:
      Serial.print("Current(mA):");
      Serial.print(testPoint_current_mA,6);
      // Serial.print(',');
      Serial.print(" Voltage(V):");
      Serial.println(testPoint_voltage_V,6);
    }
  }
} // end function runTest()

/*---------------------------------------------------------------------------------------------*/
// Take INA219 Reading:
/*---------------------------------------------------------------------------------------------*/
/**
 * @brief Reads voltage and current measurements from the INA219 sensor.
 * 
 * This function queries the INA219 for shunt and bus voltage, computes the
 * total voltage (bus + shunt), and retrieves the current. It then stores
 * the results in an `electrical_measurement` struct and returns it.
 * 
 * @return electrical_measurement 
 * A struct containing:
 *   - voltage_V : Computed voltage in volts
 *   - current_mA: Measured current in milliamps
 */
electrical_measurement INA219reading(){
  electrical_measurement data; //creates empty object 'data' of type electrical_measurement

  float shuntvoltage;
  float busvoltage ;
  float solar_panel_voltage ;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  //power_mW = ina219.getPower_mW();
  data.voltage_V = busvoltage + (shuntvoltage / 1000);
  data.current_mA = ina219.getCurrent_mA();
  
  return data;
  
} // end function INA219reading()




