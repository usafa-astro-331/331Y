#pragma once

#include <iostream>
#include <vector>
#include <unordered_set>

/* ATTITUDE DETERMINATION

 **/

#include "project_common.h"
// #include "zmodem.h"
#include <Adafruit_BNO08x.h>

void setReports(void);

// inline constexpr uint32_t serial_decimation = 5; // only print every 5th point to serial

uint32_t timeNext_testPoint;        // time of next test point (ms)
const uint32_t interval_testPoint = 50;  // time interval between test points (ms)
const uint32_t serial_interval = 250; // time interval between serial-printed test points (ms)

extern float gyro_Z, mag_X, mag_Y;
 extern Adafruit_BNO08x bno08x;
inline sh2_SensorValue_t sensorValue;

extern int sun_plusX, sun_minusX, sun_plusY, sun_minusY;

extern int n_sun_sensor_reads;

extern int S_mag, sun_X, sun_Y;

extern float sun_direction;
// extern float gyro_Z, mag_X, mag_Y;
// extern Adafruit_BNO08x bno08x;
// inline sh2_SensorValue_t sensorValue;
//
// extern int sun_plusX, sun_minusX, sun_plusY, sun_minusY;
//
// extern int n_sun_sensor_reads;
//
// extern int S_mag, sun_X, sun_Y;
//
// extern float sun_direction;


/**
 * @brief Runs the test for Lab 6 - Attitude Determination
 *
 * Collects IMU (gyroscope, magnetometer) and sun sensor data at regular intervals
 * and logs all readings to an SD card file. Test continues until user sends 'X'.
 *
 * @note Data is written to a CSV file with column headers
 *
 * @return none
 */
inline void attitude_sensors() {
  bool CSV_header_complete = false;

    // setup accel variables ////////

    std::vector<float> gyros = {0, 0, 0};  // holds moving sums of 3 accels
    float gyro_x, gyro_y, gyro_z ;       // 3 acceleration variables
    std::vector<float> mags = {0, 0, 0};  // holds moving sums of 3 accels
    float mag_x, mag_y, mag_z ;       // 3 acceleration variables

    // float sensor_reads = 0.0f;              // num sensor reads in last cycle
    bool mag_reads = false;
    float gyro_reads = 0.0f;


  if (!create_and_open_file(&dataFile, "att_de", "attitude_sensors_")) {
    Serials.println("[ERROR] Failed to create data file. Aborting test.");
    return;
  }

  char filename[40];
    dataFile.getName(filename, sizeof(filename));
  Serials.print("[INFO] Data file created successfully: ");
  Serials.println(filename);


  //
  // char file_name[40];
  // if(sd_createDataFile(&dataFile, "att_determ/attitude_sensors_")){
  //
  //   dataFile.getName(file_name, sizeof(file_name));
  //   Serials.printf("[INFO] Data file created successfully: %c \n", file_name);
  // } else {
  //   Serials.println("[ERROR] Failed to create data file. Aborting test.");
  //   return;
  // }
  Serials.println("[INFO] Ready to start attitude sensor test. 'Enter' to begin. 'X' to stop test)...");

  if (!get_command_from_ground()) {
    Serials.println("[ERROR] Failed to receive command from ground. Aborting test.");
    return;
  }


  timeNext_testPoint = millis();
  uint16_t test_point_count = 0;
  neopixelWrite(RGB_BUILTIN, 25, 16, 0); // Set to orange (R=255, G=165, B=0)

  while(!user_has_typed_x()){
    uint32_t timeNow = millis();

        if (bno08x.getSensorEvent(&sensorValue)) {
            // new data available
            switch (sensorValue.sensorId) {

            case SH2_GYROSCOPE_CALIBRATED:
                gyros.at(2) += sensorValue.un.gyroscope.z;
                gyro_reads++;
                break;

            case SH2_MAGNETIC_FIELD_CALIBRATED:
                mags.at(0) = sensorValue.un.magneticField.x;
                mags.at(1) = sensorValue.un.magneticField.y;
                mag_reads = true;
                break;
            default:
                ;
            } // end switch
            delay(1);
        } // end while (!bno08x...)

      if(timeNow > timeNext_testPoint){ // save test point to file
          test_point_count++;
          timeNext_testPoint += interval_testPoint;

        // calculate data from accumulated values

        if (gyro_reads==0) {
            gyro_x = NAN; gyro_y = NAN; gyro_z = NAN;
        }
        else{
            gyro_x =  gyros.at(0) /gyro_reads;
            gyro_y =  gyros.at(1) /gyro_reads;
            gyro_z =  gyros.at(2) /gyro_reads;
        }

        if (!mag_reads){
            mag_x = NAN; mag_y = NAN; mag_z = NAN;
        }
        else {
            mag_x =  mags.at(0);
            mag_y =  mags.at(1);
            mag_z =  mags.at(2);
        }


      // Collect Sun Sensor Test Point:
      // Average readings for each analog channel
      sun_plusX = 0;
      sun_minusX = 0;
      sun_plusY = 0;
      sun_minusY = 0;
      for (int i = 0; i < n_sun_sensor_reads; i++) {

          // sun_plusY += ads.readADC_SingleEnded(0);
          // sun_plusX += ads.readADC_SingleEnded(1);
          // sun_minusY += ads.readADC_SingleEnded(2);
          // sun_minusX += ads.readADC_SingleEnded(3);

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
       logger.add( "gZ",  "dps",   gyro_z);
       logger.add( "mX", "uT",    mag_x);
       logger.add( "mY",  "uT",   mag_y);
       logger.add( "sDir", "count",  sun_direction);
       logger.add( "spX", "count",   sun_plusX);
       logger.add( "spY", "count",   sun_plusY);
       logger.add( "snX", "count",   sun_minusX);
       logger.add( "snY", "count",   sun_minusY);


        logger.logToCSV(dataFile);
        if (!CSV_header_complete) {
          logger.create_CSV_header(dataFile);
          CSV_header_complete = true;
        }


        if (!(test_point_count % serial_decimation)) {
          logger.logToSerial(Serials);
          dataFile.flush();

        } // print to serial sometimes


        // reset data accumulators and counts to zero
        mag_reads = false;
        gyro_reads = 0.0f;

        gyros = {0.0, 0.0, 0.0};
        mags = {0.0, 0.0, 0.0};
    } // end if (timeNow>timeNext_testPoint)

    } // end of while(!user_has_typed_x())

  sd.chdir();

  Serials.println("[INFO] Attitude sensor test complete.");
  dataFile.close();
  return ;

  } // end of lab6

// setReports() ////////
// for the BNO085
// Here is where you define the sensor outputs you want to receive
inline void setReports(void) {
    Serials.println("Setting desired reports");
    // Set report rate to 20000us (20ms) which is 50Hz.
    // This ensures we get at least one update per 25ms logging interval.

    if (! bno08x.enableReport(SH2_GYROSCOPE_CALIBRATED, 10000) ) {
        Serials.println("Could not enable rotation vector");
    }
    if (! bno08x.enableReport(SH2_MAGNETIC_FIELD_CALIBRATED, 10000) ) {
        Serials.println("Could not enable magnetic vector");
    }

}
// end function setReports() ////////