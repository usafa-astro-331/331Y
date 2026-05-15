#pragma once
#include "att_determ.h"

// int get_command_from_ground_station();

#include "project_common.h"
#include <vector>
#include <cmath>
#include <variant>
#include <numeric>
#include <iterator>

#include <HardwareSerial.h>

// extern HardwareSerial SerialX;

extern FsFile dataFile;   // data file object
extern TB9051FTGMotorCarrier driver;

ESP32Encoder enc;

ICM_20948_I2C imu_sensor; // IMU object

extern const uint32_t interval_testPoint;
extern const uint32_t serial_interval;
extern uint32_t timeNext_testPoint;

// uint32_t serial_decimation = interval_testPoint/serial_interval;
// extern uint32_t serial_decimation; // only print every 5th point to serial

int sun_plusX, sun_minusX, sun_plusY, sun_minusY;
int sun_X, sun_Y;
float sun_direction = 0.0;
int S_mag;
int n_sun_sensor_reads = 5; // number of readings to average for sun sensor test point

float gyro_Z = 0.0;
float mag_X = 0.0;
float mag_Y = 0.0;

// function prototypes
float set_speed_test_B(uint32_t);

void manual_set_RW_speed();
void full_speed_step_input();
void lab7_run_test_B();
void stream_RW_speed();
float set_wheel_speed(uint32_t t_ms, uint32_t t0_ms, bool* COMPLETE);

bool TEST_COMPLETE = true;







/*---------------------------------------------------------------------------------------------*/
// Lab 7: open loop attitude control
/*---------------------------------------------------------------------------------------------*/
/**
 * @brief Runs Lab 7 Test B - Reaction wheel control and sensor data collection test.
 *
 * This function performs a comprehensive test of the spacecraft's attitude determination
 * and control system by collecting sensor data while commanding a reaction wheel motor.
 * The test runs for 40 seconds and logs all measurements to an SD card file.
 * The reaction wheel is commanded with a speed profile defined in set_speed_test_B().
 *
 * @return void
 *
 * @note
 * - Test duration is fixed at 40 seconds
 * - Test points are collected at intervals defined by interval_testPoint
 * - XBee data transmission is decimated (every 10th test point)
 * - Motor command speed is calculated as: -speed_pwm * 1000 * MOTOR_VOLTAGE / 12
 * - Sun direction is calculated from 4-quadrant sun sensor using atan2 function
 * - Data is flushed to SD card every 10 test points for data safety
 *
 * @see set_speed_test_B()
 * @see sd_createDataFile()
 * @see IMU sensor getAGMT(), gyrZ(), magX(), magY() methods
 * @see Encoder getCount() method
 */
inline void open_loop_att_control() {
  TEST_COMPLETE = false;
  bool CSV_header_complete = false;


  const uint32_t t0_ms = millis();
  if (!create_and_open_file(&dataFile, "att_control", "open_loop_")) {
    Serials.println("[ERROR] Failed to create data file. Aborting test.");
    dataFile.close();
    sd.chdir();
    return;
  }

  Serials.println("[INFO] Ready to start Lab 7 test B, send any key to begin (wait for test to complete or send 'X' to abort)...");
  if (!get_command_from_ground()) {
    Serials.println("[ERROR] Failed to receive command from ground. Aborting test.");
    dataFile.close();
    sd.chdir();
    return;
  }

  neopixelWrite(RGB_BUILTIN, 25, 0, 25); // Set to magenta (R=255, G=0, B=255)


  timeNext_testPoint = millis();
  while (!user_has_typed_x() && !TEST_COMPLETE) {

    // Record test point:
    static uint32_t test_point_count = 0;
    if (millis() > timeNext_testPoint) {         // Collect Test Point loop
      timeNext_testPoint += interval_testPoint;  // Update time for next Test Point
      test_point_count++;
      uint32_t time = millis() - t0_ms;

      float speedx = set_wheel_speed(millis(), t0_ms, &TEST_COMPLETE);
      driver.setOutput(speedx);

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
        sun_plusX += analogRead(SUN_PX_PIN);
        sun_minusX += analogRead(SUN_NX_PIN);
        sun_plusY += analogRead(SUN_PY_PIN);
        sun_minusY += analogRead(SUN_NY_PIN);
      }
      sun_plusX /= n_sun_sensor_reads;
      sun_minusX /= n_sun_sensor_reads;
      sun_plusY /= n_sun_sensor_reads;
      sun_minusY /= n_sun_sensor_reads;

      // ////////////* find sun direction ////////////////////////////////////////
      // // // uncomment sun_plusX & sun_plusY lines to calculate sun direction
      // // // (highlight them, CTRL-/)
      S_mag = sun_plusX + sun_minusX + sun_plusY + sun_minusY;
      sun_X = (sun_plusX - sun_minusX) / S_mag;
      sun_Y = (sun_plusY - sun_minusY) / S_mag;
      sun_direction = (atan2(sun_Y, sun_X) * RAD_TO_DEG);
      if (sun_direction < 0) {
        sun_direction += 360; // Adjust to range 0-360
      }
      /////////////////////////////////////////////////////////////////////////////

      // Get measured reaction wheel speed:
      static int64_t lastCount = 0;
      static uint32_t timeLastEncMeas = 0;
      uint32_t timeNow = millis();
      int64_t c = enc.getCount();
      int64_t dc = c - lastCount;
      float dt_s = (timeNow - timeLastEncMeas) / 1000.0f;
      float rev = (float)dc / ((float)CT_PER_REV * 10.0);
      float w_RW_meas = (rev / dt_s) * 60.0f;
      lastCount = c;
      timeLastEncMeas = timeNow;

      logger.clear();

      logger.add("time", "ms", time);
      logger.add("gyro_Z", "dps", gyro_Z);
      logger.add("mag_X", "uT", mag_X);
      logger.add("mag_Y", "uT", mag_Y);
      logger.add("spX", "count", sun_plusX);
      logger.add("spY", "count", sun_plusY);
      logger.add("snX", "count", sun_minusX);
      logger.add("snY", "count", sun_minusY);
      logger.add("RW_cmd", "RPM", speedx* 1000.0f * MOTOR_VOLTAGE / 12.0f);
      logger.add("RW_meas", "RPM", w_RW_meas);

  // uint8_t ii = 0;
      if (dataFile) {
        if (!CSV_header_complete) {
          logger.create_CSV_header(dataFile);
          CSV_header_complete = true;
        }
        logger.logToCSV(dataFile);


        if (!(test_point_count % serial_decimation)) {
          // print to serial sometimes
          logger.logToSerial(Serials);
          dataFile.flush();
        }
      }
        else {
          Serials.println("file error");
        }
    }

}

  dataFile.close();
  sd.chdir();
  driver.setOutput(0);
  return;


} // end function open_loop_att_control()

// linear interpolation function for wheel speed
inline float lerp(float start_value, float end_value, float fraction){
  return start_value * (1.0f - fraction) + (end_value * fraction);
}

// calculate wheel speed based on time
inline float set_wheel_speed(const uint32_t t_ms, const uint32_t t0_ms, bool* COMPLETE)
{
  uint32_t current_time = t_ms - t0_ms;

  std::vector<int> dtimes;
  std::vector<float> speeds;
  dtimes = { 0, 1000, 2000,   10000,  2500,  2500, 10000, 2500,   2500, 10000, 1000, 5000 };
  speeds = { 0,   0,   0.6,    0.6,   1.0f,   0.6,  0.6,   0.2,   0.6,   0.6,  0,    0 };
  //         ________/----------------/‾\------------------\_/-----------------\_______

  std::vector<uint32_t> times(dtimes.size());
  std::partial_sum(dtimes.begin(), dtimes.end(), times.begin());

  // find which time step we're in
  const auto current_interval_time =
    std::lower_bound(times.begin(), times.end(), current_time);

  // index of current time step
  const int step_index = std::distance(times.begin(), current_interval_time)-1;

  uint32_t total_step_time = times[step_index+1] - times[step_index];
  uint32_t time_into_step = current_time - times[step_index];
  if (total_step_time<=0) {total_step_time=1;} // prevent /0 in next step
    float fractional_step =  (float)(time_into_step) / (float)total_step_time;

  const float wheel_speed =
    lerp((float)speeds[step_index], (float)speeds[step_index+1], fractional_step);

  if (current_time > times.back() ) *COMPLETE = true;

  return wheel_speed;

}


inline void stream_RW_speed()
{
  Serials.println("Ready to stream RW Motor speed, send any key to start. Send 'X' to stop.");
  Xbee.read();
  delay(100);
  while(!Xbee.available() ){} // wait for user to send any key to start test
  delay(100); // small delay to ensure serial buffer is fully received

  while(true){
    // Check for User Input:
    if (user_has_typed_x()) {
      dataFile.close();
      driver.setOutput(0);
      return;
    }

    static uint32_t timeLastEncMeas = millis();
    static int64_t lastCount = 0;
    #define ENC_SAMPLE_MS 50

    uint32_t now = millis();
    if (now - timeLastEncMeas >= ENC_SAMPLE_MS) {
      int64_t c = enc.getCount();
      int64_t dc = c - lastCount;

      float dt_s = (now - timeLastEncMeas) / 1000.0f;

      // If you use full-quad (x4), make sure CT_PER_REV reflects *counts per rev after decoding*
      float rev = (float)dc / ((float)CT_PER_REV * 10.0);
      float rpm = (rev / dt_s) * 60.0f;

      Serials.printf("count:%lld,dc:%lld,rpm:%.2f\n", (long long)c, (long long)dc, rpm);

      lastCount = c;
      timeLastEncMeas = now;
    }
  }
} //end stream_RWspeed()


/*---------------------------------------------------------------------------------------------*/
// Lab 7: full speed step input
/*---------------------------------------------------------------------------------------------*/
/**
 * @brief Runs Test A - Tabletop static test. Initiates a step input at 100% speed 3 seconds into test.
 *         Used to determine reaction wheel torque. Test duration is 15 seconds.
 * @details
 *   - 0-3s: Motor off (speed_pwm = 0)
 *   - 3-10s: Motor at full speed (speed_pwm = 1.0)
 *   - 10-15s: Motor off (speed_pwm = 0)
 * @return void
 */
inline void full_speed_step_input() {
  TEST_COMPLETE = false;
  int test_point_count = 0;
  bool CSV_header_complete = false;

  if (!create_and_open_file(&dataFile, "att_control", "full_speed_")) {
    Serials.println("[ERROR] Failed to create data file. Aborting test.");
    dataFile.close();

    sd.chdir();
    return;
  }
  //
  // char file_name[40];
  // dataFile.getName(file_name, sizeof(file_name));
  // Serials.print("[INFO] Data file created successfully: ");
  // Serials.println(file_name);

  Serials.println("[INFO] Ready to start Lab 7 test A, send any key to begin (wait for test to complete or send 'X' to abort)...");

  if (!get_command_from_ground()) {
    Serials.println("[ERROR] Failed to receive command from ground. Aborting test.");
    dataFile.close();

    sd.chdir();
    return;
  }

  timeNext_testPoint = millis();

  float speed_pwm = 0.0;
  uint32_t t0 = millis();
  neopixelWrite(RGB_BUILTIN, 0, 25, 25); // Set to cyan (R=0, G=255, B=255)
  while (!user_has_typed_x() && !(millis() - t0 > 15000)) { //test loop


    // Set RW Motor Speed:
    if ((millis() - t0) > 3000 && (millis() - t0) < 10000){
      speed_pwm = 1.0;
      driver.setOutput(speed_pwm);
    } else if (millis() - t0 > 10000) {
      speed_pwm = 0.0;
      driver.setOutput(speed_pwm);
    }

    // Record test point:
    if (millis() > timeNext_testPoint) {          // Collect Test Point loop
      timeNext_testPoint += interval_testPoint;  // Update time for next Test Point
      uint32_t time = millis() - t0;

      // Get commanded reaction wheel speed:
      float w_RW_cmd = -speed_pwm * 1000.0 * MOTOR_VOLTAGE / 12.0;
      // Get measured reaction wheel speed:
      static int64_t lastCount = 0;
      static uint32_t timeLastEncMeas = 0;
      uint32_t timeNow = millis();
      int64_t c = enc.getCount();
      int64_t dc = c - lastCount;
      float dt_s = (timeNow - timeLastEncMeas) / 1000.0f;
      float rev = (float)dc / ((float)CT_PER_REV * 10.0);
      float w_RW_meas = (rev / dt_s) * 60.0f;
      lastCount = c;
      timeLastEncMeas = timeNow;

      // Print data to SD & XBee serial:
      logger.clear();

      logger.add("time", "ms", time);
      logger.add("RW_cmd", "RPM", w_RW_cmd);
      logger.add("RW_meas", "RPM", w_RW_meas);

      uint8_t ii = 0;
      if (dataFile) {
        if (!CSV_header_complete){logger.create_CSV_header(dataFile); CSV_header_complete = true;}
        logger.logToCSV(dataFile);
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

    } // end if (millis() > timeNext_testPoint)

    } // end while (!user_has_typed_x())

  dataFile.close();

  sd.chdir();

  driver.setOutput(0);
  Serials.print("[INFO] Test A Complete. File closed.");
  return;

}  // end function lab7_run_test_A()


/*---------------------------------------------------------------------------------------------*/
// Manually Set Reaction Wheel Speed:
/*---------------------------------------------------------------------------------------------*/
/**
 * @brief Prompts user to manually set the reaction wheel motor speed
 *
 * Requests a throttle percentage from the user (-100 to 100) via Serial or XBee,
 * validates the input, and applies it to the motor driver.
 *
 * @note Input values outside the range [-100, 100] are clamped to the limits.
 *
 * @return none
 */
inline void manual_set_RW_speed(){
  Serials.println("Enter RW Motor Throttle Percent (-100 to 100):");
  // while(!Xbee.available() ){} // wait for user to send any key to start test
  // delay(100); // small delay to ensure serial buffer is fully received

  int rw_speed_int = get_int_from_ground();
  if (rw_speed_int == -98789) {delay(500); return; }
  if (rw_speed_int>100) rw_speed_int = 100;
  if (rw_speed_int< -100) rw_speed_int = -100;
  const float rw_speed = float(rw_speed_int) / 100.0;

  Serials.print("Setting motor speed to: ");
  Serials.println(rw_speed);


  driver.setOutput(rw_speed);
  delay(750);


  return void();
}