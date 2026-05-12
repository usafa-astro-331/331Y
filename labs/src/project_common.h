#pragma once

#include <Arduino.h>
#include <variant>
#include <vector>

// #include "FsLib/FsFile.h"
#include <SdFat.h>

// Configuration
inline constexpr uint32_t serial_decimation = 5;

// External instances (Defined in the .cpp)
extern HardwareSerial Xbee;
class DualSerial;
extern DualSerial Serials;
class TelemetryLogger;
extern TelemetryLogger logger;

extern FsFile dataFile;   // data file object

/**
 * TelemetryLogger class
 * Handles mixed data types (int, float) for SD and Serial logging.
 */
class TelemetryLogger {
public:
	using TeleValue = std::variant<int, uint32_t, float>;

	struct Entry {
		const char* label;
		const char* unit;
		TeleValue value;
	};

	void add(const char* label, const char* unit, TeleValue value);
	void clear();
	void logToSerial(Print& printer);
	void logToCSV(Print& printer);
	void create_CSV_header(Print& printer);

private:
	std::vector<Entry> entries;
};

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

// Global functions

int get_int_from_ground();

bool get_command_from_ground();

bool user_has_typed_x();

bool create_and_open_file(FsFile *dataFile, const String& directory, const String& filename_preamble);

