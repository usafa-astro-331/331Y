

/*---------------------------------------------------------------------------------------------*/
// Library includes:
/*---------------------------------------------------------------------------------------------*/
#include <Arduino.h>                                      // Main Arduino library
#include <Wire.h>                                         // libray for I2C communication
#include <SPI.h>                                          // SPI communication library
// #include <SdFat.h>                                        // SD Card library
#include "definitions.h"                                  // Project definitions (this directory)
#include "sd_functions.h"                                 //SD helper functions (this directory)
// #include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h> // MAX17048 fuel gauge
#include <Adafruit_BNO08x.h>                                    // Adafruit BNO08x library
#include <TB9051FTGMotorCarrier.h>                        // Pololu Motor Carrier Library
#include <Adafruit_INA238.h>
#include "PicoEncoder.h"
// #include <Adafruit_NeoPixel.h>

// Adafruit_NeoPixel strip(neopixel_count, neopixel_pin);

#include "communication.h"

#include "project_common.h"

// declare sd card and datafile
SdFs sd;
FsFile dataFile;   // data file object
FsFile fout;

// #include <SerialUART.h>
// extern SerialUART Xbee;
// extern SerialUART SerialX; // empty serial port, exists to prevent code errors from calls to Serial2
//
// TelemetryLogger logger;

#include "wheel_speed.h"
#include "att_determ.h"
#include "electrical.h"


/*---------------------------------------------------------------------------------------------*/
// Globals:
/*---------------------------------------------------------------------------------------------*/
// Objects:
// SdFile fout;

#include "menu.h"



Adafruit_BNO08x bno08x; // IMU object
// Motor Variables/Object
constexpr uint8_t pwm1Pin{MOTOR_PWM_1_PIN}; // PWM1
constexpr uint8_t pwm2Pin{MOTOR_PWM_2_PIN}; // PWM2
TB9051FTGMotorCarrier driver{ pwm1Pin, pwm2Pin };// Instantiate TB9051FTGMotorCarrier
PicoEncoder enc; // https://github.com/pmarques-dev/PicoEncoder
// // SFE_MAX1704X lipo; // SparkFun Thing Plus ESP32-WROOM onboard fuel gauge (I2C addr 0x36)
// // Motor Variables/Object

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
  // //neopixel setup
  // strip.begin();
  // strip.show(); // initializae all pixels to OFF
  //
    Wire.setClock(400000); // Uncomment for Fast Mode: 400 kHz

  Wire.begin(); // Initialize I2C communication
  //
  // pinMode(LED_BUILTIN, OUTPUT);
  // neopixelWrite(RGB_BUILTIN, 25, 0, 0); // Default to red (R=255, G=0, B=0)
  //
  // //----------------------------------------------
  // // Initialize Serial links
  // //----------------------------------------------
  // Xbee.setRX(XBEE_RX);
  // Xbee.setRX(XBEE_TX);
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
  // sd_init(SD_CS_PIN);
    // pinMode(SD_CS_PIN, OUTPUT);
    while (!sd.begin(SD_CS_PIN)) {
        Serials.println("[ERROR] SD card initialization failed. Card present?");
        delay(2000);
    }
    Serials.println("[INFO] SD Card Initialized.");


  //----------------------------------------------

  //----------------------------------------------
  // Initialize MAX17048 fuel gauge
  //----------------------------------------------
  // if (!lipo.begin(Wire)) // Uses I2C address 0x36)
  // {
  //   Serials.println("[WARN] MAX17048 not detected on I2C (0x36). Battery telemetry (cmd 4) will be unavailable.");
  // } else {
  //   lipo.quickStart();    // Improves initial SOC accuracy after boot. Returns 0 on success.
  //   Serials.println("[INFO] MAX17048 online.");
  // }
  //----------------------------------------------

  // //----------------------------------------------
  // // Initialize BNO085
  // //----------------------------------------------
  // if (!bno08x.begin_I2C()) {
  //   Serials.println("[CAUTION] IMU not found.");
  //   while (!bno08x.begin_I2C()); {delay(100); }
  // } else {
  //   Xbee.println("[INFO] IMU Initialized.");
  // }
  //----------------------------------------------

  //----------------------------------------------
  // Initialize Sun Sensor
  //----------------------------------------------
  // analogReadResolution(12);

  //----------------------------------------------
  // Initialize current Sensor
  //----------------------------------------------
  if (!initINA238())
      Serials.println("[ERROR] INA238 not detected.");


  //----------------------------------------------
  // Initialize Reaction Wheel
  //----------------------------------------------
  driver.enable(); // TB9051FTG Motor Driver
  driver.setOutput(0);
  enc.begin(ENCODER_PIN_A); // Motor Encoder
  //----------------------------------------------

  Serials.println("[INFO] SETUP COMPLETE.");
  // SerialX.println("[INFO] SETUP COMPLETE.SEND '1' FOR OPTIONS.");
  neopixelWrite(RGB_BUILTIN, 0, 25, 0); // Set to green (R=0, G=255, B=0)


  menu.load(main_menu,GET_MENU_SIZE(main_menu));
  menu.show();

} // end setup()

/////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN LOOP:
/////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

  // menu.run(10);
  delay(300);
    enc.update();
    Serials.println(enc.step);
    // Serial.println("loop");
  
} // end loop()

