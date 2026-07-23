#pragma once

// #include <Adafruit_ADS1X15.h>
// extern Adafruit_ADS1015 ads;



#include <Arduino.h>
// #include <variant>
// #include <vector>

// #include "FsLib/FsFile.h"
#include <SdFat.h>

extern SdFs sd;
extern FsFile dataFile;

// Configuration
// inline constexpr uint32_t serial_decimation = 5;

// External instances (Defined in the .cpp)
class DualSerial;
extern DualSerial Serials;

/**
 * DualSerial class
 * Sends text simultaneously to two serial ports.
 */
class DualSerial : public Print {
public:
	DualSerial(HardwareSerial& s1, HardwareSerial& s2);
	virtual size_t write(uint8_t c) override;

private:
	HardwareSerial& serial1;
	HardwareSerial& serial2;
};


bool create_and_open_file(FsFile *dataFile, const String& directory, const String& filename_preamble);

