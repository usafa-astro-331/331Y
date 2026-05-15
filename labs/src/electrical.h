#pragma once

#include "project_common.h"
// #include <HardwareSerial.h>

// extern HardwareSerial SerialX;
// extern HardwareSerial Xbee;

Adafruit_INA238 ina238 = Adafruit_INA238();

SFE_MAX1704X lipo; // SparkFun Thing Plus ESP32-WROOM onboard fuel gauge (I2C addr 0x36)


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

    // // Ground-station friendly, parseable response
    // SerialX.print("BAT,V=");
    // SerialX.print(v, 3);
    // SerialX.print("V, SOC=");
    // SerialX.print(soc, 1);
    // SerialX.print("%, CR=");
    // SerialX.print(crate, 3);
    // SerialX.println("%/hr");

    // Also mirror to USB serial for debugging
    Xbee.print("[BAT] V=");
    Xbee.print(v, 3);
    Xbee.print(" V, SOC=");
    Xbee.print(soc, 1);
    Xbee.print(" %, CR=");
    Xbee.print(crate, 3);
    Xbee.println(" %/hr");
}


/*---------------------------------------------------------------------------------------------*/
// IV_data Test:
/*---------------------------------------------------------------------------------------------*/
/**
* @brief Runs the test
* @return none
// */
inline void IV_data(){
    Xbee.println("ivtest");
  int test_point_count = 0;
  bool CSV_header_complete = false;
  neopixelWrite(RGB_BUILTIN, 0, 0, 25); // Set to blue (R=0, G=0, B=255)

  // sd_createDataFile(&dataFile, "electrical/IV_curve_"); // create data file on SD card
  if (!create_and_open_file(&dataFile, "electrical", "IV_curve_")) {
    Serials.println("[ERROR] Failed to create data file. Aborting test.");
    return;
  }

  while(Xbee.available()>0) Xbee.read(); // clear any characters in buffer
  Xbee.println("[INFO] Send any key to start. ***Send 'X' to stop test.***");
  while(!Xbee.available()){delay(10);} // Wait for user to start test
  while(Xbee.available()>0) Xbee.read(); // clear any characters in buffer
  while(true){

    if (user_has_typed_x()) {
      dataFile.close();
      sd.chdir();
      driver.setOutput(0);
      return;
    }

    if(millis() > timeNext_testPoint){ // Collect Test Point loop
      uint32_t startTime = millis();
      timeNext_testPoint += interval_testPoint; // Update time for next Test Point

      // Collect Test Point (each reading takes ~ 1ms):
      float testPoint_current_mA = 0.0;
      float testPoint_voltage_V = 0.0;
      for (int ii = 0; ii < num_samples_per_testpoint; ii++){ // sum X readings
        testPoint_current_mA += ina238.getCurrent_mA();
        testPoint_voltage_V += ina238.getBusVoltage_V() + (ina238.getShuntVoltage_mV() / 1000.0);
      }
      testPoint_current_mA /= num_samples_per_testpoint; // average readings
      testPoint_voltage_V /= num_samples_per_testpoint; // average readings

      logger.clear();

      logger.add("time", "ms", (int)millis());
      logger.add("I", "mA", testPoint_current_mA);
      logger.add("V", "V", testPoint_voltage_V);

      if (dataFile) {
        logger.logToCSV(dataFile);
        if (!CSV_header_complete) {
          logger.create_CSV_header(dataFile);
          CSV_header_complete = true;
        }

        if (!(test_point_count % serial_decimation)) {
          // print to serial sometimes
          logger.logToSerial(Serials);
          dataFile.flush();
        }
      }
      else {
        Serials.println("file error");
      }
      test_point_count++;
  }

      // // Print data to file:
      // dataFile.print(millis());
      // dataFile.print(",");
      // dataFile.print(testPoint_current_mA,6);
      // dataFile.print(",");
      // dataFile.println(testPoint_voltage_V,6);
      // dataFile.flush(); // save file
      //
      // //Print to Serial:
      // Xbee.print("Current(mA):");
      // Xbee.print(testPoint_current_mA,6);
      // Xbee.print(",Voltage(V):");
      // Xbee.println(testPoint_voltage_V,6);
      // // Xbee.print(",collectTime(ms):");
      // // Xbee.println(millis() - startTime); //~95 ms per test point
    } // end while(true)
  }// end function IV_data()
// }

inline void initINA238()
{
  if (!ina238.begin()) {
    Xbee.println("[ERROR] Couldn't find INA238 chip");
    while (1)
      ;
  }
  Xbee.println("[INFO] Found INA238 chip");
  // set shunt resistance and max current
  ina238.setShunt(0.015, 0.5); //

  ina238.setAveragingCount(INA2XX_COUNT_128);

  // set the time over which to measure the current and bus voltage
  ina238.setVoltageConversionTime(INA2XX_TIME_150_us);
    ina238.setCurrentConversionTime(INA2XX_TIME_150_us);


} // end function initINA238()

