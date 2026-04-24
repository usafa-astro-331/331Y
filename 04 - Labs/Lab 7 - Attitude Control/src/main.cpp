

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
#include <Adafruit_INA238.h>


// #include "communication.h"
  HardwareSerial Xbee(2);
#include "main.h"

#include "wheel_speed.h"
#include "att_determ.h"
#include "electrical.h"

#include "menu.h"

/*---------------------------------------------------------------------------------------------*/
// Globals:
/*---------------------------------------------------------------------------------------------*/
// Objects:
FsFile dataFile;   // data file object
// SdFile fout;

// SFE_MAX1704X lipo; // SparkFun Thing Plus ESP32-WROOM onboard fuel gauge (I2C addr 0x36)
// ICM_20948_I2C imu_sensor; // IMU object
// Motor Variables/Object
constexpr uint8_t pwm1Pin{MOTOR_PWM_1_PIN}; // PWM1
constexpr uint8_t pwm2Pin{MOTOR_PWM_2_PIN}; // PWM2
TB9051FTGMotorCarrier driver{ pwm1Pin, pwm2Pin };// Instantiate TB9051FTGMotorCarrier
// ESP32Encoder enc;

// Variables:
uint32_t timeLastCheckForCommand; // time of next Xbee check
uint32_t interval_CheckForCommand = 10; // time interval between Xbee/Serial checks (ms)
uint32_t timeLastHeartBeat; // time of last heartbeat (ms)
uint32_t interval_heartBeat = 500; // interval between heartbeat (ms)

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

void initINA238();

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
  // Initialize Serial link with XBee
  //----------------------------------------------
  Xbee.begin(9600,SERIAL_8N1, XBEE_RX, XBEE_TX);  // Begin MCU <> XBee communication
  Xbee.setTimeout(20);
  Xbee.print("[INFO] KestrelSAT online \npress 1 for options\n\n");
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
  // Initialize current Sensor
  //----------------------------------------------
  initINA238();

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


  menu.load(main_menu,GET_MENU_SIZE(main_menu));
  menu.show();


}
/////////////////////////////////////////////////////////////////////////////////////////////////

char heartbeats[] = {'|','/','-','\\'};
int heartbeat_num = 0;
/////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN LOOP:
/////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  //----------------------------------------------
  // Check for data from ground station
  // ----------------------------------------------
  // if (timeLastCheckForCommand + interval_CheckForCommand < millis()) { // periodic Xbee send
  //   timeLastCheckForCommand = millis();
  //   process_main_menu();
  // }
  //
  // if (timeLastHeartBeat + interval_heartBeat < millis()) { // periodic heartbeat to indicate program is alive
  //   timeLastHeartBeat = millis();
  //
  //   // Serial.println("[INFO] Send '1' for Options")
  char heartbeats[] = {'|','/','-','\\'};
  int heartbeat_num = 0;
  heartbeat_num = (heartbeat_num + 1) % sizeof(heartbeats);
  Serial.printf("\r%c", heartbeats[heartbeat_num]);
  //   // Serial.printf("count:%lld,dc:%lld,rpm:%.2f\n", (long long)c, (long long)dc, rpm);
  //   // Serial.print("\h")
  //   // Serial.print(heartbeats[heartbeat_num]);
  //   Serial.printf("\r%c", heartbeats[heartbeat_num]);
  //
  //   // Xbee.println("*");
  //   // neopixelWrite(RGB_BUILTIN, 0, 25, 0); // Set to green (R=0, G=255, B=0)
  //   // // serial_print_twice(Xbee, Serial);
  //   // serial_print_twice(Xbee, Serial, "test");
  //
  // }

  menu.run(100);
  delay(100);


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
// void process_main_menu() {
//   if (!Xbee.available() && !Serial.available()) return;
//
//   // int received_int = get_command_from_ground_station();
//
//   switch (received_int) {
//     case 0:
//       driver.setOutput(0);
//       Xbee.println("Motor Stopped.");
//       Serial.println("Motor Stopped.");
//       break;
//
//     case 1:
//     serial_print_twice(Xbee, Serial, "0 - Stop reation wheel\n");
//     serial_print_twice(Xbee, Serial, "1 - Print Options Menu\n");
//     serial_print_twice(Xbee, Serial, "2 - Get RSSI\n");
//     serial_print_twice(Xbee, Serial, "3 - Toggle LED\n");
//     serial_print_twice(Xbee, Serial, "4 - Get Battery State (V, SOC, dSOC/dt) \n");
//     serial_print_twice(Xbee, Serial, "5 - Set Motor Throttle Percent (-100...100)\n");
//     serial_print_twice(Xbee, Serial, "6 - Lab 6: Run Test\n");
//     serial_print_twice(Xbee, Serial, "7 - Lab 7: Run Test A\n");
//     serial_print_twice(Xbee, Serial, "8 - Lab 7: Run Test B\n");
//     serial_print_twice(Xbee, Serial, "9 - Stream RW speed\n");
//     serial_print_twice(Xbee, Serial, "98 - SD Card: List Files (USB SERIAL ONLY)\n");
//     serial_print_twice(Xbee, Serial, "99 - SD Card: Print File Menu (USB SERIAL ONLY)\n\n");
//       break;
//
//     case 2:
//       get_sat_rssi();
//       break;
//
//         case 3:
//     digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
//     serial_print_twice(Xbee, Serial, digitalRead(LED_BUILTIN) ? "LED ON" : "LED OFF");
//     break;
//
//     case 4:
//       send_battery_telemetry();
//       break;
//
//     case 5:
//       manual_set_RW_speed();
//       break;
//
//     case 6:
//       lab6_run_test();
//       break;
//
//     case 7:
//       lab7_run_test_A();
//       break;
//
//     case 8:
//       lab7_run_test_B();
//       break;
//
//     case 9:
//       stream_RW_speed();
//       break;
//
//     case 10:
//       IV_data();
//       break;
//
//     case 98:
//       sd_listFiles("/", 0);
//       break;
//
//     case 99:
//       sd_printFileMenu();
//       break;
//
//     default:
//       serial_print_twice(Xbee, Serial, "[CAUTION] Invalid input from ground station, ignoring.");
//       break;
//   }
//   return;
// }//end function process_main_menu()

