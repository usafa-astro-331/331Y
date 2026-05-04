#pragma once

// #include "dual_serial.h"
// #include <Adafruit_INA238.h>
// auto  ina238 = Adafruit_INA238();

// #include "menu.h"
// #include "communication.h"
// #include "electrical.h"
// #include "wheel_speed.h"
// #include "att_determ.h"
#include <variant>
#include <vector>

extern HardwareSerial Xbee; // Serial object for communication with XBee
// extern FsFile dataFile;   // data file object

class DualSerial;
extern DualSerial Serials;

class TelemetryLogger;
extern TelemetryLogger logger;


// extern uint32_t timeNext_testPoint; // time of next test point (ms)
// extern const uint32_t interval_testPoint; // time interval between test points (ms)

// extern ICM_20948_I2C imu_sensor;

// extern int sun_plusX, sun_minusX, sun_plusY, sun_minusY;
// extern float sun_X, sun_Y, sun_direction;
// extern float S_mag;
// extern int16_t n_sun_sensor_reads ; // number of readings to average for sun sensor test point
//
// extern float gyro_Z;
// extern float mag_X ;
// extern float mag_Y;


// extern TB9051FTGMotorCarrier driver;
// extern ESP32Encoder enc;
//
//
// extern SFE_MAX1704X lipo; // SparkFun Thing Plus ESP32-WROOM onboard fuel gauge (I2C addr 0x36)


// #include <Adafruit_INA238.h>

inline int get_command_from_ground_station(){
	// Serial.read();
	// delay(100);
	int received_int = 0; // default to no command
	// while (received_int==0)
	// {
		// if (SerialX.available())
		// {
		// 	String received_string = SerialX.readStringUntil('\n');
		// 	received_string.trim();
		// 	if (received_string.length() == 0) return -1;
		// 	Serial.print("Received from XBee: ");
		// 	Serial.println(received_string);
		// 	received_int = received_string.toInt();
		// }
		// else
		if (Xbee.available())
		{
			String received_string = Xbee.readStringUntil('\n');
			// received_string.trim();
			delay(10);
			if (received_string.length() == 0) return -1;
			Xbee.print("Received from Serial: ");
			Xbee.println(received_string);
			received_int = received_string.toInt();
		}
	// }
	return received_int;
}



/* TelemetryLogger class
 *
 * It's complicated to hold and print both ints and floats.
 * in the same place. This handles some of that logic.
 *
 * USAGE:
 * #include "main.h"
 * TelemetryLogger logger
 *
 * logger.clear() // clears all fields
 * logger.add("label", "unit", value)
 * logger.add("label2", "unit2", value2)
 * ⋮
 *
 * logger.create_CSV_header(dataFile)
 *		writes this to dataFile (header on first row of CSV)
 *			"label_unit, label2_unit2, …,"
 *
 *	logger.logToCSV(dataFile)
 *		writes this to dataFile:
 *			"value, value2, …,"
 *
 *	logger.logToSerial(Serial)
 *		writes this to Serial:
 *			"label:value, label2:value2, …,"
 *
 * REQUIRES:
 * - >=c++17 for:
 *		- variant
 *		- vector
 *
 *	Lt Col Jordan Firth
 *	2026 May
*/
class TelemetryLogger {
public:
	// Define the variant type for easier reading
	using TeleValue = std::variant<int, uint32_t, float>;

	struct Entry {
		const char* label;
		const char* unit;
		TeleValue value;
	};

	// Add a data point to the current frame
	void add(const char* label, const char* unit, TeleValue value) {
		entries.push_back({label, unit, value});
	}

	// Clear the entries (usually called at the start of a loop)
	void clear() {
		entries.clear();
	}

	// Print to Serial/Xbee with labels
	void logToSerial(Print& printer) {
		for (const auto& e : entries) {
			printer.print(e.label); printer.print(":");
			std::visit([&printer](auto&& val) {
				printer.print(val); printer.print(", ");
			}, e.value);
		}
		printer.println();
	}

	// Print to File/SD as CSV (values only)
	void logToCSV(Print& printer) {
		for  (const auto& e : entries) {
			std::visit([&printer](auto&& val){
				printer.print(val); printer.print(", ");
			// }, entries[ii].value);
			}, e.value);
		}
		printer.println();
	}

	// Print to File/SD as CSV (values only)
	void create_CSV_header(Print& printer) {
		for (const auto& e : entries) {
				printer.print(e.label);
				printer.print("_");
				printer.print(e.unit);
				printer.print(", ");
			}
		printer.println();
	}

private:
	std::vector<Entry> entries;
};

/* DualSerial class
 * creates a serial object that sends text simultaneously
 * to two serial ports
 *
 * USAGE
 * DualSerial Serials(Serial, Serial2)
 *
 * Serials.print(anything);
 *
 *
*/
class DualSerial : public Print {
public:
	DualSerial(HardwareSerial& s1, HardwareSerial& s2) : serial1(s1), serial2(s2) {}

	virtual size_t write(uint8_t c) {
		serial1.write(c);
		return serial2.write(c);
	}

	  private:
	HardwareSerial& serial1;
	HardwareSerial& serial2;
};

/// function user_has_typed_x()
/// @return true if user has typed X
///
/// @brief test to exit function early
inline bool user_has_typed_x() {
	switch (tolower((unsigned char) Xbee.peek()) ) {

	case 'x': // stop if user types 'X' or 'x'
		Xbee.read();
		Serials.print("[CAUTION] Test Canceled Early. File closed.");

		return true;
		// break;

	case EOF: // if no input (peek returns end-of-file), do nothing
		return false;
		// break;

	default:
		Serials.printf("[CAUTION] Invalid Input (%c) continuing test...", Xbee.read() );

	} // end switch
	return false;
} // end has_user_typed_x()
