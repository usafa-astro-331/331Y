/**
 * @file main.cpp
 * @brief Main control program for KestrelSAT attitude determination and control system
 * 
 * @details This is firmware for the USAFA Astro 331 KestrelSAT, implementing:
 * - Lab 6 Attitude Determination capabilities using IMU (gyroscope, magnetometer) and sun sensors
 * - Lab 7 Attitude Control via reaction wheel motor with encoder feedback
 * - Ground station command interface via XBee radio and USB Serial
 * - Battery telemetry monitoring via MAX17048 fuel gauge
 * - Data logging to SD card for Lab 6 and Lab 7 experiments
 * 
 * @author Lt Col Wyatt Harris
 * @date Spring 2026
 * @version 2.0
 * 
 * @section hardware Hardware Requirements
 * - ESP32 microcontroller (SparkFun Thing Plus ESP32-WROOM)
 * - ICM-20948 9-DoF IMU (via I2C at 0x36)
 * - MAX17048 Fuel Gauge (via I2C at 0x36)
 * - Pololu TB9051FTG Motor Driver
 * - DC motor with quadrature encoder
 * - Sun sensor (4-channel analog inputs)
 * - XBee radio module (UART at 9600 baud)
 * - SD card module (SPI)
 * - WS2812 RGB LED and digital status LED
 * 
 * @section dependencies External Libraries
 * - greiman/SdFat@^2.3.1 - SD card file system
 * - sparkfun/SparkFun MAX1704x Fuel Gauge@^1.0.3 - Battery monitoring
 * - sparkfun/SparkFun 9DoF IMU (ICM 20948)@^1.3.2 - IMU sensor interface
 * - meelonusk/TB9051FTGMotorCarrier@^1.0.2 - Motor driver control
 * - madhephaestus/ESP32Encoder@^0.12.0 - Quadrature encoder reading
 * 
 * @section commands Ground Station Commands
 * | Cmd | Function |
 * |-----|----------|
 * | 0   | Stop reaction wheel |
 * | 1   | Display options menu |
 * | 2   | Query XBee RSSI |
 * | 3   | Toggle status LED |
 * | 4   | Get battery telemetry (V, SOC, charge rate) |
 * | 5   | Manually set reaction wheel throttle (-100 to 100%) |
 * | 6   | Run Lab 6 test (Attitude Determination) |
 * | 7   | Run Lab 7 Test A (tabletop static, RW torque measurement) |
 * | 8   | Run Lab 7 Test B (dynamic test with prescribed wheel speed) |
 * | 98  | SD Card: List files (USB Serial only) |
 * | 99  | SD Card: Print file menu (USB Serial only) |
 * 
 * @section testing Test Modes
 * - **Lab 6**: Collects IMU (gyro Z, mag X/Y) and sun sensor data at 50ms intervals
 * - **Lab 7A**: 15-second tabletop test with 100% motor step input from 3-10 seconds
 * - **Lab 7B**: 50-second dynamic test with prescribed motor speed profile (hold, ramp up/down cycles)
 * 
 * @section datalog Data Logging Format
 * All test data is logged to SD card in CSV format with:
 * - MCU timestamp (ms)
 * - Gyroscope Z-axis (deg/s)
 * - Magnetometer X, Y axes (µT)
 * - Sun direction (degrees)
 * - Sun sensor raw values (4 channels)
 * - Reaction wheel commanded and measured speed (RPM)
 * 
 * @section communication Communication
 * - **USB Serial**: 115200 baud for USB debugging/control
 * - **XBee UART**: 9600 baud for wireless ground station link
 * - **I2C**: Sensor communication at standard rate
 * - **SPI**: SD card access
 * 
 * @note Heartbeat signal sent every 2 seconds to indicate program status
 * @note Ground station commands checked every 10ms
 * @note RGB LED provides visual status: Red (startup), Green (idle), Orange (Lab 6), Cyan (Lab 7A), Magenta (Lab 7B)
 * 
 * @warning All test loops use blocking serial reads; may delay program execution if waiting for input
 * @warning Reaction wheel tests should be conducted with appropriate safety precautions
 * 
 * @see definitions.h for hardware pin assignments and constants
 * @see sd_functions.h for SD card utility functions
 */

/*---------------------------------------------------------------------------------------------*/
// Library includes:
/*---------------------------------------------------------------------------------------------*/
#include <Arduino.h>                                      // Main Arduino library
#include <Wire.h>                                         // libray for I2C communication
#include <SPI.h>                                          // SPI communication library
#include <SdFat.h>                                        // SD Card library
#include "definitions.h"                                  // Project definitions (this directory)
#include "sd_functions.h"                                 //SD helper functions (this directory)
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h> // MAX17048 fuel gauge
#include <ICM_20948.h>                                    // Sparkfun IMU library
#include <TB9051FTGMotorCarrier.h>                        // Pololu Motor Carrier Library
#include <ESP32Encoder.h>                                 // Motor encoder library to measure wheel speed

#include "communication.h"
  HardwareSerial Xbee(2);
#include "main.h"

#include "wheel_speed.h"
#include "att_determ.h"
#include "electrical.h"

/*---------------------------------------------------------------------------------------------*/
// Globals:
/*---------------------------------------------------------------------------------------------*/
// Objects:
FsFile dataFile;   // data file object

// SFE_MAX1704X lipo; // SparkFun Thing Plus ESP32-WROOM onboard fuel gauge (I2C addr 0x36)
ICM_20948_I2C imu_sensor; // IMU object
// Motor Variables/Object
constexpr uint8_t pwm1Pin{MOTOR_PWM_1_PIN}; // PWM1
constexpr uint8_t pwm2Pin{MOTOR_PWM_2_PIN}; // PWM2
TB9051FTGMotorCarrier driver{ pwm1Pin, pwm2Pin };// Instantiate TB9051FTGMotorCarrier
// ESP32Encoder enc;

// Variables:
uint32_t timeLastCheckForCommand; // time of next Xbee check
uint32_t interval_CheckForCommand = 10; // time interval between Xbee/Serial checks (ms)
uint32_t timeLastHeartBeat; // time of last heartbeat (ms)
uint32_t interval_heartBeat = 3000; // interval between heartbeat (ms)

float sun_plusX, sun_minusX, sun_plusY, sun_minusY;
float sun_X = 0.0, sun_Y = 0.0, sun_direction = 0.0;
float S_mag;
int16_t n_sun_sensor_reads = 5; // number of readings to average for sun sensor test point

uint32_t timeNext_testPoint; // time of next test point (ms)
uint32_t interval_testPoint = 50; // time interval between test points (ms)

// float gyro_Z = 0.0;
// float mag_X = 0.0;
// float mag_Y = 0.0;

/*---------------------------------------------------------------------------------------------*/
// Function Prototypes (see defintiions after loop()):
/*---------------------------------------------------------------------------------------------*/
// int get_command_from_ground_station();
void process_main_menu();
// void get_sat_rssi();
// void send_battery_telemetry();
// void manual_set_RW_speed();
// void lab6_run_test();
// void lab7_run_test_A();
// void lab7_run_test_B();
// float set_speed_test_B(uint32_t t0);
// void stream_RWspeed();

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
  neopixelWrite(RGB_BUILTIN, 25, 0, 0); // Default to red (R=255, G=0, B=0)

  //----------------------------------------------
  // Initialize Serial link with XBeea
  //----------------------------------------------
  Xbee.begin(9600,SERIAL_8N1, XBEE_RX, XBEE_TX);  // Begin MCU <> XBee communication
  Xbee.setTimeout(20);
  Xbee.println("[INFO] KestrelSAT online \npress 1 for options");
  // ----------------------------------------------

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
  // Initialize ICM20948
  //----------------------------------------------
  if (imu_sensor.begin(Wire, 1) != ICM_20948_Stat_Ok) {
    Serial.println("[CAUTION] IMU not found.");
    while (1);
  } else{
    // // 1. Set to Continuous Mode for consistent sampling
    // imu_sensor.setSampleMode((ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), ICM_20948_Sample_Mode_Continuous);
    // // 2. Configure Sample Rate Divider (formula: 1125 / (1 + divider) Hz)
    // ICM_20948_smplrt_t mySmplrt;
    // mySmplrt.a = 1; // Accel divider 1: 1125 / (1+1) = ~562.5 Hz
    // mySmplrt.g = 1; // Gyro divider 1: 1125 / (1+1) = ~562.5 Hz
    // imu_sensor.setSampleRate(ICM_20948_Internal_Acc, mySmplrt);
    // imu_sensor.setSampleRate(ICM_20948_Internal_Gyr, mySmplrt);    
    Serial.println("[INFO] IMU Initialized.");
  }
  //----------------------------------------------

  //----------------------------------------------
  // Initialize Sun Sensor
  //----------------------------------------------
  analogReadResolution(12);

  //----------------------------------------------
  // Initialize Reaction Wheel
  //----------------------------------------------
  driver.enable(); // TB9051FTG Motor Driver
  driver.setOutput(0);
  enc.attachFullQuad(ENCODER_PIN_A, ENCODER_PIN_B); // Motor Encoder
  enc.clearCount();
  //----------------------------------------------

  Serial.println("[INFO] SETUP COMPLETE.");
  Xbee.println("[INFO] SETUP COMPLETE.SEND '1' FOR OPTIONS.");
  neopixelWrite(RGB_BUILTIN, 0, 25, 0); // Set to green (R=0, G=255, B=0)
  
}
/////////////////////////////////////////////////////////////////////////////////////////////////

char8_t heartbeats[6] = {'⣾','⣽','⣻','⣟','⣯','⣷'};
int heartbeat_num = 0;
/////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN LOOP:
/////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  //----------------------------------------------
  // Check for data from ground station
  // ----------------------------------------------
  if (timeLastCheckForCommand + interval_CheckForCommand < millis()) { // periodic Xbee send
    timeLastCheckForCommand = millis();
    process_main_menu();
  }

  if (timeLastHeartBeat + interval_heartBeat < millis()) { // periodic heartbeat to indicate program is alive
    timeLastHeartBeat = millis();

    // Serial.println("[INFO] Send '1' for Options")
    heartbeat_num = (heartbeat_num + 1) % sizeof(heartbeats);
    // Serial.printf("count:%lld,dc:%lld,rpm:%.2f\n", (long long)c, (long long)dc, rpm);
    // Serial.print("\h")
    // Serial.print(heartbeats[heartbeat_num]);
    Serial.printf("\r%c", heartbeats[heartbeat_num]);

    // Xbee.println("*");
    // neopixelWrite(RGB_BUILTIN, 0, 25, 0); // Set to green (R=0, G=255, B=0)
    // // serial_print_twice(Xbee, Serial);
    // serial_print_twice(Xbee, Serial, "test");

  }

}
/////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCITON DEFINITIONS:
/////////////////////////////////////////////////////////////////////////////////////////////////

/*---------------------------------------------------------------------------------------------*/
// Get Command from Ground Station:
/*---------------------------------------------------------------------------------------------*/
/**
 * @brief Gets command received from the ground station (USB or XBee).
 *
 * Reads a command string from USB or XBee and parses the command into 
 * an integer.
 *
 * @note Command strings are trimmed of leading/trailing whitespace before
 *       parsing. Commands are expected to be integers at the start of the
 *       string. Unrecognized commands are ignored.
 * 
 * @returns int of command received
 *
 * @warning This function uses blocking serial reads (readStringUntil())
 *          and may delay program execution if no command is received.
 *
 */
int get_command_from_ground_station()
{
  int received_int = 0; // default to no command
  if (Xbee.available())
  {
    String received_string = Xbee.readStringUntil('\n');
    received_string.trim();
    if (received_string.length() == 0) return -1;
    Serial.print("Received from XBee: ");
    Serial.println(received_string);
    received_int = received_string.toInt();
  }
  else if (Serial.available())
  {
    String received_string = Serial.readStringUntil('\n');
    received_string.trim();
    if (received_string.length() == 0) return -1;
    Serial.print("Received from Serial: ");
    Serial.println(received_string);
    received_int = received_string.toInt();
  }
  return received_int;
}

/*---------------------------------------------------------------------------------------------*/
// Process Main Menu:
/*---------------------------------------------------------------------------------------------*/
/**
 * @brief Process and execute commands received from the ground station.
 *
 * Reads a command integer from the ground station (USB Serial or XBee),
 * parses it, and executes the corresponding action. Supported commands:
 *
 * - 0: Stop reaction wheel motor
 * - 1: Print options menu
 * - 2: Get satellite RSSI
 * - 3: Toggle LED
 * - 4: Get battery telemetry (voltage, SOC, charge rate)
 * - 5: Manually set reaction wheel speed (throttle %)
 * - 6: Run Lab 6 - Attitude Determination test
 * - 7: Run Lab 7 - Test A (tabletop static test)
 * - 8: Run Lab 7 - Test B
 * - 98: SD Card - List files (USB Serial only)
 * - 99: SD Card - Print file menu (USB Serial only)
 *
 * @note Command strings are trimmed of leading/trailing whitespace before
 *       parsing. Commands are expected to be integers. Unrecognized commands
 *       are ignored with a caution message.
 *
 * @return void
 *
 */
void process_main_menu() {
  if (!Xbee.available() && !Serial.available()) return;

  int received_int = get_command_from_ground_station();

  switch (received_int) {
    case 0:
      driver.setOutput(0);
      Xbee.println("Motor Stopped.");
      Serial.println("Motor Stopped.");
      break;
    
    case 1:
    serial_print_twice(Xbee, Serial, "0 - Stop reation wheel\n");
    serial_print_twice(Xbee, Serial, "1 - Print Options Menu\n");
    serial_print_twice(Xbee, Serial, "2 - Get RSSI\n");
    serial_print_twice(Xbee, Serial, "3 - Toggle LED\n");
    serial_print_twice(Xbee, Serial, "4 - Get Battery State (V, SOC, dSOC/dt) \n");
    serial_print_twice(Xbee, Serial, "5 - Set Motor Throttle Percent (-100...100)\n");
    serial_print_twice(Xbee, Serial, "6 - Lab 6: Run Test\n");
    serial_print_twice(Xbee, Serial, "7 - Lab 7: Run Test A\n");
    serial_print_twice(Xbee, Serial, "8 - Lab 7: Run Test B\n");
    serial_print_twice(Xbee, Serial, "9 - Stream RW speed\n");
    serial_print_twice(Xbee, Serial, "98 - SD Card: List Files (USB SERIAL ONLY)\n");
    serial_print_twice(Xbee, Serial, "99 - SD Card: Print File Menu (USB SERIAL ONLY)\n");
      break;

    case 2:
      get_sat_rssi();
      break;

        case 3:
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    serial_print_twice(Xbee, Serial, digitalRead(LED_BUILTIN) ? "LED ON" : "LED OFF");
    break;

    case 4:
      send_battery_telemetry();
      break;

    case 5:
      manual_set_RW_speed();
      break;

    case 6:
      lab6_run_test();
      break;

    case 7:
      lab7_run_test_A();
      break;

    case 8:
      lab7_run_test_B();
      break;

    case 9:
      stream_RW_speed();
      break;

    case 10:
      IV_data();
      break;

    case 98:
      sd_listFiles("/", 0);
      break;

    case 99:
      sd_printFileMenu();
      break;

    default:
      serial_print_twice(Xbee, Serial, "[CAUTION] Invalid input from ground station, ignoring.");
      break;
  }
  return;
}//end function process_main_menu()

