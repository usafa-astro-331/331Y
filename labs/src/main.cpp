

/*---------------------------------------------------------------------------------------------*/
// Library includes:
/*---------------------------------------------------------------------------------------------*/
#include <Arduino.h>                                      // Main Arduino library
#include <Wire.h>                                         // libray for I2C communication
#include <SPI.h>                                          // SPI communication library
#include "definitions.h"                                  // Project definitions (this directory)

#include "project_common.h"

// declare sd card and datafile
SdFs sd;
FsFile dataFile;   // data file object
FsFile fout;

/////////////////////////////////////////////////////////////////////////////////////////////////
// SETUP:
/////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  //
  //
    Wire.setClock(400000); // Uncomment for Fast Mode: 400 kHz

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

    }

  Serials.print("[INFO] KestrelSAT online \npress 1 for options\n\n");
  // ----------------------------------------------

    while (!sd.begin(SD_CS_PIN)) {
        Serials.println("[ERROR] SD card initialization failed. Card present?");
        delay(2000);
    }
    Serials.println("[INFO] SD Card Initialized.");

    if (!create_and_open_file(&dataFile, "att", "att1")) {
        Serials.println("failed here");;
    }
    else{ Serials.println("success here"); }

    sd.rmdir("/attde");


} // end setup()

/////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN LOOP:
/////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

  // menu.run(10);
  delay(10);
    // Serial.println("loop");
  
} // end loop()

