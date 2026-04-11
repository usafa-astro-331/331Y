#pragma once

#include "main.h"

// Adafruit_INA238 ina238 = Adafruit_INA238();

constexpr int num_samples_per_testpoint = 40; // number of samples per testpoint to average over


/*---------------------------------------------------------------------------------------------*/
// Get Battery Information:
/*---------------------------------------------------------------------------------------------*/
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
inline void send_battery_telemetry() {

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


/*---------------------------------------------------------------------------------------------*/
// IV_data Test:
/*---------------------------------------------------------------------------------------------*/
/**
* @brief Runs the test
* @return none
// */
inline void IV_data(){
    Serial.println("ivtest");
  neopixelWrite(RGB_BUILTIN, 0, 0, 255); // Set to blue (R=0, G=0, B=255)

  float current = 0.0;
    current = ina238.getCurrent_mA();

  //
  // sd_createDataFile(&dataFile, "Lab3_IV_curve"); // create data file on SD card
  //
  // while(Serial.available()>0) Serial.read(); // clear any characters in buffer
  // Serial.println("[INFO] Send any key to start. ***Send 'X' to stop test.***");
  // while(!Serial.available()){delay(10);} // Wait for user to start test
  // while(Serial.available()>0) Serial.read(); // clear any characters in buffer
  // while(true){
  //
  //   // if(Serial.available()>0){ // Check for user input
  //   //   char c = Serial.read();
  //   //   switch(c){
  //   //     case 'X':
  //   //       while(Serial.available()>0) Serial.read(); // clear any characters in buffer
  //   //       dataFile.close();
  //   //       Serial.println("[INFO] Test Complete.");
  //   //       neopixelWrite(RGB_BUILTIN, 0, 255, 0); // Set to green (R=0, G=255, B=0)
  //   //       return;
  //   //     default:
  //   //       Serial.println("[CAUTION] Invalid Input, continuing test...");
  //   //       break;
  //   //   }
  //   // }
  //
  //   if(millis() > timeNext_testPoint){ // Collect Test Point loop
  //     uint32_t startTime = millis();
  //     timeNext_testPoint += interval_testPoint; // Update time for next Test Point
  //
  //     // Collect Test Point (each reading takes ~ 1ms):
  //     float testPoint_current_mA = 0.0;
  //     float testPoint_voltage_V = 0.0;
  //     for (int ii = 0; ii < num_samples_per_testpoint; ii++){ // sum X readings
  //       testPoint_current_mA += ina238.getCurrent_mA();
  //       // testPoint_voltage_V += ina238.getBusVoltage_V() + (ina238.getShuntVoltage_mV() / 1000.0);
  //     }
  //     // testPoint_current_mA /= num_samples_per_testpoint; // average readings
  //     // testPoint_voltage_V /= num_samples_per_testpoint; // average readings
  //     //
  //     // // Print data to file:
  //     // dataFile.print(millis());
  //     // dataFile.print(",");
  //     // dataFile.print(testPoint_current_mA,6);
  //     // dataFile.print(",");
  //     // dataFile.println(testPoint_voltage_V,6);
  //     // dataFile.flush(); // save file
  //     //
  //     // //Print to Serial:
  //     // Serial.print("Current(mA):");
  //     // Serial.print(testPoint_current_mA,6);
  //     // Serial.print(",Voltage(V):");
  //     // Serial.println(testPoint_voltage_V,6);
  //     // // Serial.print(",collectTime(ms):");
  //     // // Serial.println(millis() - startTime); //~95 ms per test point
    // }
  // }
} // end function IV_data()
