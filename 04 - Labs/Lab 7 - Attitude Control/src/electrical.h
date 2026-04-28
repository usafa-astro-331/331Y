#pragma once

#include "main.h"
#include <HardwareSerial.h>

extern HardwareSerial SerialX;
extern HardwareSerial Xbee;

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
  neopixelWrite(RGB_BUILTIN, 0, 0, 25); // Set to blue (R=0, G=0, B=255)

  sd_createDataFile(&dataFile, "electrical/IV_curve_"); // create data file on SD card

  while(Xbee.available()>0) Xbee.read(); // clear any characters in buffer
  Xbee.println("[INFO] Send any key to start. ***Send 'X' to stop test.***");
  while(!Xbee.available()){delay(10);} // Wait for user to start test
  while(Xbee.available()>0) Xbee.read(); // clear any characters in buffer
  while(true){

    if(Xbee.available()>0){ // Check for user input
      char c = Xbee.read();
      switch(c){
        case 'X':
          while(Xbee.available()>0) Xbee.read(); // clear any characters in buffer
          dataFile.close();
          Xbee.println("[INFO] Test Complete.");
          neopixelWrite(RGB_BUILTIN, 0, 25, 0); // Set to green (R=0, G=255, B=0)
          return;
        default:
          Xbee.println("[CAUTION] Invalid Input, continuing test...");
          break;
      }
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

      // Print data to file:
      dataFile.print(millis());
      dataFile.print(",");
      dataFile.print(testPoint_current_mA,6);
      dataFile.print(",");
      dataFile.println(testPoint_voltage_V,6);
      dataFile.flush(); // save file

      //Print to Serial:
      Xbee.print("Current(mA):");
      Xbee.print(testPoint_current_mA,6);
      Xbee.print(",Voltage(V):");
      Xbee.println(testPoint_voltage_V,6);
      // Xbee.print(",collectTime(ms):");
      // Xbee.println(millis() - startTime); //~95 ms per test point
    }
  }
} // end function IV_data()

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
  uint16_t counts[] = {1, 4, 16, 64, 128, 256, 512, 1024};
  Xbee.print("[INFO] Averaging counts: ");
  Xbee.println(counts[ina238.getAveragingCount()]);

  // set the time over which to measure the current and bus voltage
  ina238.setVoltageConversionTime(INA2XX_TIME_150_us);
  Xbee.print("[INFO] Voltage conversion time: ");
  switch (ina238.getVoltageConversionTime()) {
  case INA2XX_TIME_50_us:
    Xbee.print("50");
    break;
  case INA2XX_TIME_84_us:
    Xbee.print("84");
    break;
  case INA2XX_TIME_150_us:
    Xbee.print("150");
    break;
  case INA2XX_TIME_280_us:
    Xbee.print("280");
    break;
  case INA2XX_TIME_540_us:
    Xbee.print("540");
    break;
  case INA2XX_TIME_1052_us:
    Xbee.print("1052");
    break;
  case INA2XX_TIME_2074_us:
    Xbee.print("2074");
    break;
  case INA2XX_TIME_4120_us:
    Xbee.print("4120");
    break;
  }
  Xbee.println(" uS");

  ina238.setCurrentConversionTime(INA2XX_TIME_150_us);
  Xbee.print("[INFO] Current conversion time: ");
  switch (ina238.getCurrentConversionTime()) {
  case INA2XX_TIME_50_us:
    Xbee.print("50");
    break;
  case INA2XX_TIME_84_us:
    Xbee.print("84");
    break;
  case INA2XX_TIME_150_us:
    Xbee.print("150");
    break;
  case INA2XX_TIME_280_us:
    Xbee.print("280");
    break;
  case INA2XX_TIME_540_us:
    Xbee.print("540");
    break;
  case INA2XX_TIME_1052_us:
    Xbee.print("1052");
    break;
  case INA2XX_TIME_2074_us:
    Xbee.print("2074");
    break;
  case INA2XX_TIME_4120_us:
    Xbee.print("4120");
    break;
  }
  Xbee.println(" uS");
} // end function initINA238()

