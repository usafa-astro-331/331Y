#pragma once

/* ATTITUDE DETERMINATION
 *
 * CONNECTIONS:
 * sparkfun 9DOF IMU via QWIIC/I2C
 *    black: GND
 *    red: 3.3V
 *    blue: SDA
 *    yellow: SCL
 *
 * 4x phototransistor sun sensors via voltage divider
 *
 *   3V3        (phototransistor)
 *    |        ↙
 *    └-----(*)-----┬-----<1kΩ>-----┐
 *             sensor pin‡          |     ‡see include/definitions.h for pin assignments
 *                                 GND
 *
 *
 **/

#include "project_common.h"

// inline constexpr uint32_t serial_decimation = 5; // only print every 5th point to serial

uint32_t timeNext_testPoint;        // time of next test point (ms)
const uint32_t interval_testPoint = 50;  // time interval between test points (ms)
const uint32_t serial_interval = 250; // time interval between serial-printed test points (ms)

extern float gyro_Z, mag_X, mag_Y;
extern ICM_20948_I2C imu_sensor;

extern int sun_plusX, sun_minusX, sun_plusY, sun_minusY;

extern int n_sun_sensor_reads;

extern int S_mag, sun_X, sun_Y;

extern float sun_direction;

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
  bool CSV_header_complete = false;

  char file_name[40];
  if(sd_createDataFile(&dataFile, "att_determ/Lab6_test")){
    // write header row:
    dataFile.getName(file_name, sizeof(file_name));
    Serials.printf("[INFO] Data file created successfully: %c \n", file_name);
  } else {
    Serials.println("[ERROR] Failed to create data file. Aborting test.");
    return;
  }
  Serials.println("[INFO] Ready to start Lab 6 test, send any key to begin (send 'X' to stop test)...");

  int start = get_command_from_ground_station();

  timeNext_testPoint = millis();
  int test_point_count = 0;
  neopixelWrite(RGB_BUILTIN, 25, 16, 0); // Set to orange (R=255, G=165, B=0)
  while(true){

    if (user_has_typed_x()) {
      dataFile.close();
      return;
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
      sun_plusX = 0;
      sun_minusX = 0;
      sun_plusY = 0;
      sun_minusY = 0;
      for (int i = 0; i < n_sun_sensor_reads; i++) {
        sun_plusX += analogRead(SUN_PX_PIN);
        sun_minusX += analogRead(SUN_NX_PIN);
        sun_plusY += analogRead(SUN_PY_PIN);
        sun_minusY += analogRead(SUN_NY_PIN);
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

      // Print data to USB & XBee serial:
      logger.clear();

       logger.add("time", "ms", timeNow);
       logger.add( "gZ",  "dps",   gyro_Z);
       logger.add( "mX", "uT",    mag_X);
       logger.add( "mY",  "uT",   mag_Y);
       logger.add( "sDir", "count",  sun_direction);
       logger.add( "spX", "count",   sun_plusX);
       logger.add( "spY", "count",   sun_plusY);
       logger.add( "snX", "count",   sun_minusX);
       logger.add( "snY", "count",   sun_minusY);


      uint8_t ii = 0;
      if (dataFile) {
        logger.logToCSV(dataFile);
        if (!CSV_header_complete) {
          logger.create_CSV_header(dataFile);
          CSV_header_complete = true;
        }


        if (!(test_point_count % serial_decimation)) {
          logger.logToSerial(Serials);
          dataFile.flush();
        } // print to serial sometimes
        else {
          Serials.println("file error");
        }
      } // end if (dataFile)
    } // end if (timeNow>timeNext_testPoint)

    } // end of while(true)
  } // end of lab6

