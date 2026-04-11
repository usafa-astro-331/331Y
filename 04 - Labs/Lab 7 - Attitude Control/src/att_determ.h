#pragma once

#include "main.h"

/*---------------------------------------------------------------------------------------------*/
// Run Lab 6 Test:
/*---------------------------------------------------------------------------------------------*/
/**
 * @brief Runs the test for Lab 6 - Attitude Determination
 *
 * Collects IMU (gyroscope, magnetometer) and sun sensor data at regular intervals
 * and logs all readings to an SD card file. Test continues until user sends 'X'.
 *
 * @note Data is written to a CSV file with the following columns:
 *       mcu time (ms), gyro Z (dps), mag X (uT), mag Y (uT), sun direction (deg),
 *       sun_plusX, sun_plusY, sun_minusX, sun_minusY
 *
 * @return none
 */
inline void lab6_run_test() {

  char file_name[40];
  if(sd_createDataFile(&dataFile, "Lab6_test")){
    // write header row:
    dataFile.println("mcu time (ms),gyro Z (dps),mag X (uT),mag Y (uT),sun direction (deg),sun_plusX,sun_plusY,sun_minusX,sun_minusY");
    dataFile.flush();
    dataFile.getName(file_name, sizeof(file_name));
    Xbee.print("[INFO] Data file created successfully: ");
    Xbee.println(file_name);
    Serial.print("[INFO] Data file created successfully: ");
    Serial.println(file_name);
  } else {
    Xbee.println("[ERROR] Failed to create data file. Aborting test.");
    Serial.println("[ERROR] Failed to create data file. Aborting test.");
    return;
  }
  Serial.println("[INFO] Ready to start Lab 6 test, send any key to begin (send 'X' to stop test)...");
  Xbee.println("[INFO] Ready to start Lab 6 test, send any key to begin (send 'X' to stop test)...");

  while(!Serial.available() && !Xbee.available()){} // wait for user to send any key to start test
  delay(100); // small delay to ensure serial buffer is fully received
  while(Serial.available()) Serial.read(); // clear serial buffer
  while(Xbee.available()) Xbee.read(); // clear Xbee buffer

  timeNext_testPoint = millis();
  int test_point_count = 0;
  neopixelWrite(RGB_BUILTIN, 255, 165, 0); // Set to orange (R=255, G=165, B=0)
  while(true){

    if(Serial.available() > 0 || Xbee.available() > 0) { // Check for user input from USB or XBee
      char c = (Serial.available() > 0) ? Serial.read() : Xbee.read();
      if(c == 'X' || c == 'x') { // If user sent 'X', stop the test
        Serial.print("[INFO] Test Complete. File ");
        Serial.print(file_name);
        Xbee.print("[INFO] Test Complete. File: ");
        Xbee.print(file_name);
        if(dataFile.close()) {
          Serial.println(" closed.");
          Xbee.println(" closed.");
        } else {
          Serial.println(" failed to close.");
          Xbee.println(" failed to close.");
        }
        return;
      } else {
        Serial.println("[CAUTION] Invalid Input, continuing test...");
        Xbee.println("[CAUTION] Invalid Input, continuing test...");
      }
    }

    uint32_t timeNow = millis();
    if(timeNow > timeNext_testPoint){ // Collect Test Point loop
      test_point_count++;
      timeNext_testPoint += interval_testPoint; // Update time for next Test Point

      // Collect IMU Test Point:
      imu_sensor.getAGMT();
      gyro_Z = imu_sensor.gyrZ();
      mag_X = imu_sensor.magX();
      mag_Y = imu_sensor.magY();

      // Collect Sun Sensor Test Point:
      // Average readings for each analog channel
      sun_plusX = 0.0;
      sun_minusX = 0.0;
      sun_plusY = 0.0;
      sun_minusY = 0.0;
      for (int i = 0; i < n_sun_sensor_reads; i++) {
        sun_plusX += analogRead(SUN_SENSOR_PLUS_X_PIN);
        sun_minusX += analogRead(SUN_SENSOR_MINUS_X_PIN);
        sun_plusY += analogRead(SUN_SENSOR_PLUS_Y_PIN);
        sun_minusY += analogRead(SUN_SENSOR_MINUS_Y_PIN);
      }
      sun_plusX /= n_sun_sensor_reads;
      sun_minusX /= n_sun_sensor_reads;
      sun_plusY /= n_sun_sensor_reads;
      sun_minusY /= n_sun_sensor_reads;

      // ////////////* find sun direction *////////////////////////////////////////
      // // // uncomment sun_plusX & sun_plusY lines to calculate sun direction
      // // // (highlight them, CTRL-/)
      S_mag = sun_plusX + sun_minusX + sun_plusY + sun_minusY;
      // sun_X = ;
      // sun_Y = ;
      sun_X = (sun_plusX - sun_minusX);
      sun_Y = (sun_plusY - sun_minusY);
      sun_direction = (atan2(sun_Y, sun_X) * RAD_TO_DEG);
      if (sun_direction < 0) {
        sun_direction += 360; // Adjust to range 0-360
      }
      /////////////////////////////////////////////////////////////////////////////

      // Print data to .csv file:
      dataFile.print(timeNow);
      dataFile.print(",");
      dataFile.print(gyro_Z,4);
      dataFile.print(",");
      dataFile.print(mag_X,4);
      dataFile.print(",");
      dataFile.print(mag_Y,4);
      dataFile.print(",");
      dataFile.print(sun_direction,4);
      dataFile.print(",");
      dataFile.print(sun_plusX);
      dataFile.print(",");
      dataFile.print(sun_plusY);
      dataFile.print(",");
      dataFile.print(sun_minusX);
      dataFile.print(",");
      dataFile.println(sun_minusY);

      // Print data to USB & XBee serial:
      String test_point_string;
      test_point_string += "t:";
      test_point_string += timeNow;
      test_point_string += ",gZ:";
      test_point_string += gyro_Z;
      test_point_string += ",mX:";
      test_point_string += mag_X;
      test_point_string += ",mY:";
      test_point_string += mag_Y;
      test_point_string += ",sDir:";
      test_point_string += sun_direction;
      test_point_string += ",s+X:";
      test_point_string += sun_plusX;
      test_point_string += ",s+Y:";
      test_point_string += sun_plusY;
      test_point_string += ",s-X:";
      test_point_string += sun_minusX;
      test_point_string += ",s-Y:";
      test_point_string += sun_minusY;
      test_point_string += "\n";

      //Print to USB Serial:
      Serial.print(test_point_string);
      //Print to XBee:
      if(test_point_count % 10 == 0){
        dataFile.flush(); // save file every 10 test points
        Xbee.print("tp:");
        Xbee.println(test_point_count);
        // Xbee.print(test_point_string);
      }
    }
  }
}
