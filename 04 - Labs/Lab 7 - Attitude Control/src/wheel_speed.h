#pragma once

int get_command_from_ground_station();

#include "main.h"
#include <vector>
#include <cmath>
#include <variant>
#include <string>

extern FsFile dataFile;   // data file object
extern TB9051FTGMotorCarrier driver;

ESP32Encoder enc;

ICM_20948_I2C imu_sensor; // IMU object

// uint32_t timeLastCheckForCommand; // time of next Xbee check
// uint32_t interval_CheckForCommand; // time interval between Xbee/Serial checks (ms)
// uint32_t timeLastHeartBeat; // time of last heartbeat (ms)
// uint32_t interval_heartBeat; // interval between heartbeat (ms)

int sun_plusX, sun_minusX, sun_plusY, sun_minusY;
int sun_X, sun_Y;
float sun_direction = 0.0;
int S_mag;
int n_sun_sensor_reads = 5; // number of readings to average for sun sensor test point

extern uint32_t timeNext_testPoint; // time of next test point (ms)
extern uint32_t interval_testPoint; // time interval between test points (ms)

float gyro_Z = 0.0;
float mag_X = 0.0;
float mag_Y = 0.0;


// function prototypes
float set_speed_test_B(uint32_t);

void manual_set_RW_speed();
void lab7_run_test_A();
void lab7_run_test_B();
void stream_RW_speed();
float set_wheel_speed(int t_ms, uint32_t t0_ms);

/*---------------------------------------------------------------------------------------------*/
// Lab 7: Run Test B
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

struct Var
{
  const char* label;
  std::variant<int, uint32_t, float> value;
};

inline void printVar(const Var& v)
{
  Serial.print(v.label);
  Serial.print(":");

  std::visit([](auto&& val)
  {
    Serial.print(val);
    Serial.print(" ");
  }, v.value);
}


inline void lab7_run_test_B()
{
  if(sd_createDataFile(&dataFile, "Lab7_testB")){
    // write header row:
    dataFile.println("mcu time(ms),gyro_Z(deg/s),mag_X(uT),mag_Y(uT),sun_direction(deg),sun_plusX(count),sun_plusY(count),sun_minusX(count),sun_minusY(count),w_RW_cmd(RPM),w_RW_meas(RPM)");
    dataFile.flush();
    char file_name[40];
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
  Serial.println("[INFO] Ready to start Lab 7 test B, send any key to begin (wait for test to complete or send 'X' to abort)...");
  Xbee.println("[INFO] Ready to start Lab 7 test B, send any key to begin (wait for test to complete or send 'X' to abort)...");

  while(!Serial.available() && !Xbee.available()){} // wait for user to send any key to start test
  delay(100); // small delay to ensure serial buffer is fully received
  while(Serial.available()) Serial.read(); // clear serial buffer
  while(Xbee.available()) Xbee.read(); // clear Xbee buffer

  neopixelWrite(RGB_BUILTIN, 25, 0, 25); // Set to magenta (R=255, G=0, B=255)
  static uint32_t t0;
  t0 = millis();
  timeNext_testPoint = millis();
  while (true) { // test loop

    // Check for User Input:
    char c;
    bool newUserInput = false;
    if (Serial.available() > 0) {  // Check for user input from USB
      c = Serial.read();
      newUserInput = true;
    }
    if (Xbee.available() > 0) {  // Check for user input from USB
      c = Xbee.read();
      newUserInput = true;
    }
    if(newUserInput){
      newUserInput = false;
      switch (c) {
        case 'X':
        case 'x':
          dataFile.close();
          Serial.print("[CAUTION] Test Canceled Early. File closed.");
          Xbee.print("[CAUTION] Test Canceled Early. File closed.");
          driver.setOutput(0);
          return;
        default:
          Serial.println("[CAUTION] Invalid Input, continuing test...");
          Xbee.println("[CAUTION] Invalid Input, continuing test...");
          break;
      }
    }

    // Record test point:
    int test_point_count = 0;
    if (millis() > timeNext_testPoint) {         // Collect Test Point loop
      timeNext_testPoint += interval_testPoint;  // Update time for next Test Point
      test_point_count++;
      uint32_t time = millis() - t0;

      // // Set RW Motor Speed:
      // float speed_pwm = set_speed_test_B(t0);
      float speed_pwm = 2.0;

      float speed2 = set_wheel_speed(millis(), t0);

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

      // Get commanded reaction wheel speed:
      float w_RW_cmd = -speed_pwm * 1000.0 * MOTOR_VOLTAGE / 12.0;
      // Get measured reaction wheel speed:
      static int64_t lastCount = 0;
      static uint32_t timeLastEncMeas = 0;
      uint32_t timeNow = millis();
      int64_t c = enc.getCount();
      int64_t dc = c - lastCount;
      float dt_s = (timeNow - timeLastEncMeas) / 1000.0f;
      float rev = (float)dc / ((float)CPR * 10.0);
      float w_RW_meas = (rev / dt_s) * 60.0f;
      lastCount = c;
      timeLastEncMeas = timeNow;

      // using Cell = std::variant<int, uint32_t, float>;
      // std::vector<Cell> data{
      //   time, gyro_Z, mag_X, mag_Y, sun_direction,
      //   sun_plusX, sun_plusY, sun_minusX, sun_minusY,
      //   w_RW_cmd, speed2, w_RW_meas
      // };


      Var vars[] = {
        {"time", time},
        {"sun_px", sun_plusX},
        {"RW_speed", w_RW_meas},
      };

  static int ii = 0;
      for (const auto& v : vars){
      printVar(v);
        Serial.println(ii);
        ii++;
      }

      // for (int ii=0; ii<data.size(); ii++)
      // {
        // csv
        // dataFile.print(std::to_string(data[ii]));
        // std::string temp;
        // std::visit([](auto&& arg) { temp = arg; }, data[ii]);
        // auto string_lambda = [](auto&& arg){return arg;};
        // std::string temp;
        // temp = std::visit(string_lambda, data[ii]);
        // dataFile.print(temp);
        // dataFile.print(", ");



      // // Print data to .csv file:
      // dataFile.print(time);
      // dataFile.print(",");
      // dataFile.print(gyro_Z);
      // dataFile.print(",");
      // dataFile.print(mag_X);
      // dataFile.print(",");
      // dataFile.print(mag_Y);
      // dataFile.print(",");
      // dataFile.print(sun_direction);
      // dataFile.print(",");
      // dataFile.print(sun_plusX);
      // dataFile.print(",");
      // dataFile.print(sun_plusY);
      // dataFile.print(",");
      // dataFile.print(sun_minusX);
      // dataFile.print(",");
      // dataFile.print(sun_minusY);
      // dataFile.print(",");
      // dataFile.print(w_RW_cmd);
      // dataFile.print(",");
      // dataFile.print(speed2);
      // dataFile.print(",");
      // dataFile.println(w_RW_meas);
      //
      // dataFile.flush();  // save file

      // Print data to USB & XBee serial:
      String test_point_string;
      test_point_string += "t:";
      test_point_string += time;
      test_point_string += ",gyro_Z:";
      test_point_string += gyro_Z;
      test_point_string += ",mag_X:";
      test_point_string += mag_X;
      test_point_string += ",mag_Y:";
      test_point_string += mag_Y;
      test_point_string += ",sun_direction:";
      test_point_string += sun_direction;
      test_point_string += ",sun_plusX:";
      test_point_string += sun_plusX;
      test_point_string += ",sun_plusY:";
      test_point_string += sun_plusY;
      test_point_string += ",sun_minusX:";
      test_point_string += sun_minusX;
      test_point_string += ",sun_minusY:";
      test_point_string += sun_minusY;
      test_point_string += ",w_RW_cmd:";
      test_point_string += w_RW_cmd;
      test_point_string += ",w_RW_meas:";
      test_point_string += w_RW_meas;
      test_point_string += ",speed2:";
      test_point_string += speed2;

      test_point_string += "\n";
      //Print to USB Serial:
      // Serial.print(test_point_string);
      //Print to XBee:
      if(test_point_count % 10 == 0){
        dataFile.flush(); // save file every 10 test points
        Xbee.print("tp:");
        Xbee.println(test_point_count);
        // Xbee.print(test_point_string);
      }
      // Serial.println(millis() - time + t0);
    }

    // End test if complete:
    if (millis() - t0 > 40e3){
      dataFile.close();
      Serial.print("[INFO] Test B Complete. File closed.");
      Xbee.print("[INFO] Test B Complete. File closed.");
      return;
    }


}

} // end function lab7_run_test_B()

// linear interpolation function for wheel speed
inline float lerp(float a, float b, float f){
  return a * (1.0 - f) + (b * f);
}

// calculate wheel speed based on time
inline float set_wheel_speed(int t_ms, uint32_t t0_ms)
{
  float current_time = t_ms - t0_ms;

  static std::vector<float> intervals, times;
  intervals = times = {
    0, 1e3, 2e3, 10e3, 2.5e3, 2.5e3, 10e3, 2.5e3, 2.5e3, 10e3, 1e3, 1e3
  };

  static std::vector<float> speeds = {
    0, 0, 0.6, 0.6, 0.7, 0.6, 0.6, 0.5, 0.6, 0.6, 0, 0
  };

  for (size_t ii = 1 < times.size(); ++ii;)
  {
    times[ii] += times[ii-1];
  }

  // find which time step we're in
  const auto current_interval_time =
    std::lower_bound(times.begin(), times.end(), current_time);

  // index of current time step
  const int step_index = std::distance(times.begin(), current_interval_time);

  float fractional_step =
    (current_time - times[step_index]) / (times[step_index+1] - times[step_index]);

  const float wheel_speed =
    lerp(speeds[step_index], speeds[step_index+1], fractional_step);

  return wheel_speed;

}


/*---------------------------------------------------------------------------------------------*/
// Set Wheel Speed:
/*---------------------------------------------------------------------------------------------*/
/**
 * @brief Sets the reaction wheel motor speed according to a prescribed test profile for Lab 7 Test B.
 *
 * Implements a dynamic speed profile over 50 seconds:
 *  - 0-10s: Hold at base speed (0.6)
 *  - 10-15s: Hold at base speed (0.6)
 *  - 15-17.5s: Ramp up to base + ramp speed
 *  - 17.5-20s: Ramp back down to base speed
 *  - 20-25s: Hold at base speed
 *  - 25-27.5s: Ramp down to 0
 *  - 27.5-30s: Ramp back up to base speed
 *  - 30-35s: Hold at base speed
 *  - 35s+: Turn off wheel
 *
 * @return (float) commanded PWM motor speed (0.0 to 1.0)
 */

  inline float set_speed_test_B(uint32_t t0) {

  static uint32_t elapsed;
  static uint32_t t;
  static float base_speed = 0.6;
  static float ramp_speed = 0.4;
  float motor_PWM_cmd = 0.0;

  t = millis() - t0;

  if (t < 10e3) {  // hold still at half speed (10 sec)
    motor_PWM_cmd = base_speed;
    neopixelWrite(RGB_BUILTIN, 255, 255, 0); // Set to yellow (R=255, G=255, B=0)
  }

  else if (t < 15e3) {  // hold still at half speed (5 sec)
    motor_PWM_cmd = base_speed;
    neopixelWrite(RGB_BUILTIN, 0, 0, 255); // Set to blue (R=0, G=0, B=255)
  }

  else if (t < 17.5e3) {  // ramp up (2.5 sec)
    elapsed = t - 15e3;
    motor_PWM_cmd = base_speed + ramp_speed * elapsed / 2.5e3;
  }

  else if (t < 20e3) {  // ramp back down to half speed (2.5 sec)
    elapsed = t - 17.5e3;
    motor_PWM_cmd = base_speed + ramp_speed - (ramp_speed * elapsed / 2.5e3);
  }

  else if (t < 25e3) {  // hold new position (5 sec)
    motor_PWM_cmd = base_speed;
  }

  else if (t < 27.5e3) {  // ramp down (2.5 sec)
    elapsed = t - 25e3;
    motor_PWM_cmd = base_speed - (ramp_speed * elapsed / 2.5e3);

  } else if (t < 30e3) {  // ramp back up to half speed  (5 sec)
    elapsed = t - 27.5e3;
    motor_PWM_cmd = base_speed - ramp_speed + (ramp_speed * elapsed / 2.5e3);
  }

  else if (t < 35e3) {  // hold new position (5 sec)
    motor_PWM_cmd = base_speed;
    neopixelWrite(RGB_BUILTIN, 255, 0, 255); // Set to magenta (R=255, G=0, B=255)

  }

  else if (t > 35e3) {  // turn off wheel
    motor_PWM_cmd = 0;
    driver.setOutput(0);
    neopixelWrite(RGB_BUILTIN, 255, 255, 255); // Set to white (R=255, G=255, B=255)

  }

  driver.setOutput(motor_PWM_cmd);
  Serial.println(motor_PWM_cmd);

  return motor_PWM_cmd;

}  // end set_speed()


inline void stream_RW_speed()
{
  Serial.println("Ready to stream RW Motor speed, send any key to start. Send 'X' to stop.");
  Xbee.println("Ready to stream RW Motor speed, send any key to start. Send 'X' to stop.");
  Serial.read();
  delay(100);
  while(!Serial.available() && !Xbee.available()){} // wait for user to send any key to start test
  delay(100); // small delay to ensure serial buffer is fully received

  while(true){
    // Check for User Input:
    char c;
    bool newUserInput = false;
    if (Serial.available() > 0) {  // Check for user input from USB
      c = Serial.read();
      newUserInput = true;
    }
    // if (Xbee.available() > 0) {  // Check for user input from USB
    //   c = Xbee.read();
    //   newUserInput = true;
    // }
    if(newUserInput){
      newUserInput = false;
      switch (c) {
        case 'X':
        case 'x':
          return;
      }
    }

    static uint32_t timeLastEncMeas = millis();
    static int64_t lastCount = 0;
    #define ENC_SAMPLE_MS 50

    uint32_t now = millis();
    if (now - timeLastEncMeas >= ENC_SAMPLE_MS) {
      int64_t c = enc.getCount();
      int64_t dc = c - lastCount;

      float dt_s = (now - timeLastEncMeas) / 1000.0f;

      // If you use full-quad (x4), make sure CPR reflects *counts per rev after decoding*
      float rev = (float)dc / ((float)CPR * 10.0);
      float rpm = (rev / dt_s) * 60.0f;

      Serial.printf("count:%lld,dc:%lld,rpm:%.2f\n", (long long)c, (long long)dc, rpm);
      Xbee.printf("count:%lld,dc:%lld,rpm:%.2f\n", (long long)c, (long long)dc, rpm);

      lastCount = c;
      timeLastEncMeas = now;
    }
  }
} //end stream_RWspeed()


/*---------------------------------------------------------------------------------------------*/
// Lab 7: Run Test A
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
inline void lab7_run_test_A() {

  if(sd_createDataFile(&dataFile, "Lab7_testA")){
    // write header row:
    dataFile.println("mcu time(ms),gyro_Z(deg/s),mag_X(uT),mag_Y(uT),sun_direction(deg),sun_plusX(count),sun_plusY(count),sun_minusX(count),sun_minusY(count),w_RW_cmd(RPM),w_RW_meas(RPM)");
    dataFile.flush();
    char file_name[40];
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
  Serial.println("[INFO] Ready to start Lab 7 test A, send any key to begin (wait for test to complete or send 'X' to abort)...");
  Xbee.println("[INFO] Ready to start Lab 7 test A, send any key to begin (wait for test to complete or send 'X' to abort)...");

  while(!Serial.available() && !Xbee.available()){} // wait for user to send any key to start test
  delay(100); // small delay to ensure serial buffer is fully received
  while(Serial.available()) Serial.read(); // clear serial buffer
  while(Xbee.available()) Xbee.read(); // clear Xbee buffer

  timeNext_testPoint = millis();
  bool newUserInput = false;
  float speed_pwm = 0.0;
  uint32_t t0 = millis();
  neopixelWrite(RGB_BUILTIN, 0, 255, 255); // Set to cyan (R=0, G=255, B=255)
  while (true) { //test loop

    // Check for User Input:
    char c;
    if (Serial.available() > 0) {  // Check for user input from USB
      c = Serial.read();
      newUserInput = true;
    }
    if (Xbee.available() > 0) {  // Check for user input from USB
      c = Xbee.read();
      newUserInput = true;
    }
    if(newUserInput){
      newUserInput = false;
      switch (c) {
        case 'X':
        case 'x':
          dataFile.close();
          Serial.print("[CAUTION] Test Canceled Early. File closed.");
          Xbee.print("[CAUTION] Test Canceled Early. File closed.");
          driver.setOutput(0);
          return;
        default:
          Serial.println("[CAUTION] Invalid Input, continuing test...");
          Xbee.println("[CAUTION] Invalid Input, continuing test...");
          break;
      }
    }

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
      float rev = (float)dc / ((float)CPR * 10.0);
      float w_RW_meas = (rev / dt_s) * 60.0f;
      lastCount = c;
      timeLastEncMeas = timeNow;

      // Print data to .csv file:
      dataFile.print(time);
      dataFile.print(",");
      dataFile.print(gyro_Z);
      dataFile.print(",");
      dataFile.print(mag_X);
      dataFile.print(",");
      dataFile.print(mag_Y);
      dataFile.print(",");
      dataFile.print(sun_direction);
      dataFile.print(",");
      dataFile.print(sun_plusX);
      dataFile.print(",");
      dataFile.print(sun_plusY);
      dataFile.print(",");
      dataFile.print(sun_minusX);
      dataFile.print(",");
      dataFile.print(sun_minusY);
      dataFile.print(",");
      dataFile.print(w_RW_cmd);
      dataFile.print(",");
      dataFile.println(w_RW_meas);

      dataFile.flush();  // save file

      // Print data to USB & XBee serial:
      String test_point_string;
      test_point_string += "t:";
      test_point_string += time;
      test_point_string += ",w_RW_cmd:";
      test_point_string += w_RW_cmd;
      test_point_string += ",w_RW_meas:";
      test_point_string += w_RW_meas;

      test_point_string += "\n";
      //Print to USB Serial:
      Serial.print(test_point_string);
      //Print to XBee:
      // Xbee.print(test_point_string);

      // Serial.println(millis() - time + t0);
    }

    // End test if complete:
    if (millis() - t0 > 15000){
      dataFile.close();
      Serial.print("[INFO] Test A Complete. File closed.");
      Xbee.print("[INFO] Test A Complete. File closed.");
      return;
    }
  }
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
  Serial.println("Enter RW Motor Throttle Percent (-100 to 100):");
  Xbee.println("Enter RW Motor Throttle Percent (-100 to 100):");
  while(!Serial.available() && !Xbee.available()){} // wait for user to send any key to start test
  delay(100); // small delay to ensure serial buffer is fully received

  int rw_speed_int = get_command_from_ground_station();
  if (rw_speed_int>100) rw_speed_int = 100;
  if (rw_speed_int<-100) rw_speed_int = -100;
  const float rw_speed = float(rw_speed_int) / 100.0;

  Serial.println("Setting motor speed to: ");
  Serial.println(rw_speed);
  Xbee.println("Setting motor speed to: ");
  Xbee.println(rw_speed);

  delay(500);

  driver.setOutput(rw_speed);

  while(Serial.available()) Serial.read(); // clear serial buffer
  while(Xbee.available()) Xbee.read(); // clear Xbee buffer
}