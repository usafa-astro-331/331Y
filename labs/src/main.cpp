

/*---------------------------------------------------------------------------------------------*/
// Library includes:
/*---------------------------------------------------------------------------------------------*/
#include <Arduino.h>                                      // Main Arduino library
#include <Wire.h>                                         // libray for I2C communication
#include <SPI.h>                                          // SPI communication library
#include "definitions.h"                                  //SD helper functions (this directory)
#include <Adafruit_BNO08x.h>                                    // Adafruit BNO08x library
#include <TB9051FTGMotorCarrier.h>                        // Pololu Motor Carrier Library
#include <Adafruit_INA238.h>
#include "PicoEncoder.h"
#include "communication.h"

#include "project_common.h"

#include "sd_functions.h"

// declare sd card and datafile
SdFs sd;
FsFile dataFile;   // data file object
FsFile fout;

#include "wheel_speed.h"
#include "att_determ.h"
#include "electrical.h"
#include "menu.h"
Adafruit_BNO08x bno08x; // IMU object
// Motor Variables/Object
constexpr uint8_t pwm1Pin{MOTOR_PWM_1_PIN}; // PWM1
constexpr uint8_t pwm2Pin{MOTOR_PWM_2_PIN}; // PWM2
TB9051FTGMotorCarrier driver{ pwm1Pin, pwm2Pin };// Instantiate TB9051FTGMotorCarrier


// Variables:
uint32_t timeLastCheckForCommand; // time of next Xbee check
uint32_t interval_CheckForCommand = 10; // time interval between Xbee/Serial checks (ms)
uint32_t timeLastHeartBeat; // time of last heartbeat (ms)
uint32_t interval_heartBeat = 500; // interval between heartbeat (ms)
// void initINA238();
/////////////////////////////////////////////////////////////////////////////////////////////////
// SETUP:
/////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  //
  //
    Wire.setClock(100000); // bno086 messes with i2c past 100 kHz

  Wire.begin(); // Initialize I2C communication

  Xbee.begin(57600);  // Begin MCU <> XBee communication
  // Xbee.setTimeout(20);
  Serial.begin(9600); // Begin Serial communication with computer
  // // Wait for user to open Serial monitor (up to 3 seconds)
  // for (int i = 0; i < 300 && !Serial; i++) {
  delay(100);
  // }

    while (!Serial) {
        delay(100);
        toggle_LED();

    }

  Serials.print("[INFO] KestrelSAT online \npress 1 for options\n\n");
  // ----------------------------------------------


    // BNO085 setup ////////
    // Try to initialize!
    // inline void initialize_bno08x(void){
    if (!bno08x.begin_I2C()) {
        Serial.println("Failed to find BNO08x chip");
    } else {
        Serial.println("BNO08x Found!");
        setReports();
    }
    // }   // end BNO085 IMU setup ////////



  //----------------------------------------------
  // Initialize SD Card
  //----------------------------------------------
    while (!sd.begin(SD_CS_PIN)) {
        Serials.println("[ERROR] SD card initialization failed. Card present?");
        delay(2000);
    }
    Serials.println("[INFO] SD Card Initialized.");


    menu.load(main_menu,GET_MENU_SIZE(main_menu));
    menu.show();
} // end setup()

/////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN LOOP:
/////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

  menu.run(10);
  delay(10);
    // Serial.println("loop");
  
} // end loop()

