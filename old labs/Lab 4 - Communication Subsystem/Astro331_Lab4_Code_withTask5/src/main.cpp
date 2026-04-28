/* Astro331_Lab4_Code_withTask5
* ===========================================================
* Created by: Lt Col Wyatt Harris, Spring 2026
* Description: Use this code IAW Astro 331 Lab 4 instructions.
* Project: Communication Subsystem Lab with Task 5 
* Libraries:
*	    greiman/SdFat@^2.3.1
*     sparkfun/SparkFun MAX1704x Fuel Gauge Arduino Library@^1.0.3
* =========================================================== */

/*---------------------------------------------------------------------------------------------*/
// Library includes:
/*---------------------------------------------------------------------------------------------*/
#include <Arduino.h>          // Main Arduino library
#include <Wire.h>             // libray for I2C communication
#include <SPI.h>              // SPI communication library
#include <SdFat.h>            // SD Card library
#include "definitions.h"     // Project definitions (this directory)
#include "sd_functions.h"     //SD helper functions (this directory)
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h> // MAX17048 fuel gauge

/*---------------------------------------------------------------------------------------------*/
// Globals:
/*---------------------------------------------------------------------------------------------*/
// Objects:
FsFile dataFile;   // data file object
HardwareSerial Xbee(2); // Serial object for communication with XBee 
SFE_MAX1704X lipo; // SparkFun Thing Plus ESP32-WROOM onboard fuel gauge (I2C addr 0x36)

// Variables:
uint32_t timeLastXbee; // time of next Xbee check
uint32_t interval_Xbee = 10; // time interval between Xbee checks (ms)
uint32_t timeLastHeartBeat; // time of last heartbeat (ms)
uint32_t interval_heartBeat = 2000; // interval between heartbeat (ms)

/*---------------------------------------------------------------------------------------------*/
// Function Prototypes (see defintiions after loop()):
/*---------------------------------------------------------------------------------------------*/
void get_command_from_ground_station();
void get_sat_rssi();
void send_battery_telemetry();

/////////////////////////////////////////////////////////////////////////////////////////////////
// SETUP:
/////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200); // Begin Serial communication with computer
  while (!Serial) {delay(10);} // Wait for user to open Serial monitor before proceeding
  Serial.println("[INFO] Hello World!");

  Wire.begin(); // Initialize I2C communication

  // Initialize built-in RGB LED (WS2812) and STAT LED
  // #define RGB_BUILTIN  2
  pinMode(RGB_BUILTIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  neopixelWrite(RGB_BUILTIN, 255, 0, 0); // Set to green (R=0, G=255, B=0)

  //----------------------------------------------
  // Initialize SD Card
  //----------------------------------------------
  sd_init(SD_CS_PIN);
  //----------------------------------------------

  //----------------------------------------------
  // Initialize MAX17048 fuel gauge
  //----------------------------------------------
  if (!lipo.begin(Wire)) // Uses I2C address 0x36) 
  {
    Serial.println("[WARN] MAX17048 not detected on I2C (0x36). Battery telemetry (cmd 4) will be unavailable.");
  } else {
    lipo.quickStart();    // Improves initial SOC accuracy after boot. Returns 0 on success.
    Serial.println("[INFO] MAX17048 online.");
  }
  //----------------------------------------------

  //----------------------------------------------
  // Initialize Serial link with XBee
  //----------------------------------------------
  Xbee.begin(9600,SERIAL_8N1, XBEE_RX, XBEE_TX);  // Begin MCU <> XBee communication
  Xbee.setTimeout(20);
  Xbee.println("[INFO] KestrelSAT online \npress 1 for options");
  // ----------------------------------------------

  Serial.println("[INFO] SETUP COMPLETE.");
  neopixelWrite(RGB_BUILTIN, 0, 255, 0); // Set to green (R=0, G=255, B=0)
  
}
/////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN LOOP:
/////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  //----------------------------------------------
  // Check for data from ground station
  // ----------------------------------------------
  if (timeLastXbee + interval_Xbee < millis()) { // periodic Xbee send
    timeLastXbee = millis();
    get_command_from_ground_station();
  }

  if (timeLastHeartBeat + interval_heartBeat < millis()) { // periodic Xbee send
    timeLastHeartBeat = millis();
    Serial.print(".");
    Xbee.println(".");
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCITON DEFINITIONS:
/////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Process and execute commands received from the ground station.
 *
 * Reads a command string from Xbee (typically connected to an XBee or
 * other ground station link), parses the command into an integer code, and
 * executes the corresponding action. Supported commands:
 *   - 1 : Print a help menu to Xbee
 *   - 2 : Query and return the satellite radio RSSI via get_sat_rssi()
 *   - 3 : Toggle the onboard LED on/off and report the new state
 *   - 4 : Execute the runTest() function
 *
 * @note Command strings are trimmed of leading/trailing whitespace before
 *       parsing. Commands are expected to be integers at the start of the
 *       string. Unrecognized commands are ignored.
 *
 * @warning This function uses blocking serial reads (Xbee.readString()) 
 *          and may delay program execution if no command is received.
 *
 * @see get_sat_rssi(), runTest()
 */
void get_command_from_ground_station() {
  if (!Xbee.available()) return;

  String what_to_do = Xbee.readStringUntil('\n');
  what_to_do.trim();
  if (what_to_do.length() == 0) return;

  Serial.print("Received: ");
  Serial.println(what_to_do);

  int do_it = what_to_do.toInt();

  switch (do_it) {
    case 1:
      Xbee.print("1 help \n");
      Xbee.print("2 get RSSI \n");
      Xbee.print("3 toggle LED \n");
      Xbee.print("4 battery telemetry (V, SOC, dSOC/dt) \n");
      break;

    case 2:
      get_sat_rssi();
      break;

    case 3:
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      Xbee.println(digitalRead(LED_BUILTIN) ? " LED on" : " LED off");
      break;

    case 4:
      send_battery_telemetry();
      break;
  }
  return;
}//end function get_command_from_ground_station()

/**
 * @brief Query the connected XBee radio for received signal strength (RSSI).
 *
 * This function places the XBee module into command mode, issues the ATDB
 * command to request the RSSI of the last received packet, parses the hex
 * response into a decimal value, and prints the result over both Serial and
 * Xbee in dBm.
 *
 * @note The function uses blocking delays and loops; it will not return until
 *       the XBee responds correctly with "OK\r".
 *
 * @warning Assumes that Xbee is connected to an XBee radio and already
 *          initialized at the correct baud rate.
 *
 * @post After the function runs, the XBee is returned to data mode using the
 *       ATCN command.
 *
 * @see Digi XBee AT Command Reference for details on ATDB and ATCN commands.
 */
void get_sat_rssi() {
  Xbee.println(" standby for RSSI");
  
  // put the radio in command mode:
  bool not_done = true;
  String ok_response = "OK\r";   //The﻿response we expect.
  String response = String("");  //Create an empty string
  Serial.println("Starting get_sat_rssi()");
  
  // Read the text of the response into the response variable
  while (not_done) {  // As long as we did not get a response from the XBee
    response = String("");
    delay(1100);
    Xbee.print("+++");  // Put the XBee 3 into 'Command Mode'
    // Serial.print("+++");   // Put the XBee 3 into 'Command Mode'

    // delay(1100);  // Wait for the XBee to finish
    while (response.length() < ok_response.length()) {
      if (Xbee.available() > 0) {

        response += (char)Xbee.read();  // Read a single character at a time
      }
    }
    not_done = !response.equals(ok_response);  // Set the not_done flag to the opposite of the result of equality check
  }
  // Serial.println(response);
      

  // If we got the right response, configure the radio and return true.   
  Xbee.print("ATDB\r");  // destination high and destination low addresses set to 0 means all messages will only go
  delay(100);               // Wait for the XBee
  response = String("");
  while (Xbee.available() > 0) {
    response += (char)Xbee.read();  //Read a single character at a time
    
  }
  Serial.println(response);
  Xbee.print("ATCN\r");  // Switch back to data mode

  String response2 = response; 
  uint32_t dec_response = strtoul(response2.c_str(), NULL, 16);

  Xbee.print("RSSI: -");
  Xbee.print(dec_response);
  Xbee.println(" dBm");
  Serial.println("sat rssi sent");
} // end function get_rssi()

/**
 * @brief Send MAX17048 battery telemetry over the XBee link.
 *
 * Reads:
 *  - Battery voltage (V)
 *  - State of Charge (SOC, %)
 *  - Change / discharge rate (dSOC/dt, %/hr; negative = discharging)
 *
 * Output format (single line):
 *   BAT,V=<volts>,SOC=<percent>,CR=<percent_per_hr>
 */
void send_battery_telemetry() {

  const float v = lipo.getVoltage();
  const float soc = lipo.getSOC();
  const float crate = lipo.getChangeRate(); // %/hr (positive=charging, negative=discharging)

  // Ground-station friendly, parseable response
  Xbee.print("BAT,V=");
  Xbee.print(v, 3);
  Xbee.print("V, SOC=");
  Xbee.print(soc, 1);
  Xbee.print("%, CR=");
  Xbee.print(crate, 3);
  Xbee.println("%/hr");

  // Also mirror to USB serial for debugging
  Serial.print("[BAT] V=");
  Serial.print(v, 3);
  Serial.print(" V, SOC=");
  Serial.print(soc, 1);
  Serial.print(" %, CR=");
  Serial.print(crate, 3);
  Serial.println(" %/hr");
} 