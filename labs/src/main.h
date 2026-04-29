#pragma once

// #include "dual_serial.h"
#include <Adafruit_INA238.h>
// auto  ina238 = Adafruit_INA238();

// #include "menu.h"
// #include "communication.h"
// #include "electrical.h"
// #include "wheel_speed.h"
// #include "att_determ.h"
#include <variant>
#include <vector>

extern HardwareSerial Xbee; // Serial object for communication with XBee
extern FsFile dataFile;   // data file object

// class TelemetryLogger;


// extern uint32_t timeNext_testPoint; // time of next test point (ms)
// extern const uint32_t interval_testPoint; // time interval between test points (ms)

extern ICM_20948_I2C imu_sensor;

// extern int sun_plusX, sun_minusX, sun_plusY, sun_minusY;
// extern float sun_X, sun_Y, sun_direction;
// extern float S_mag;
// extern int16_t n_sun_sensor_reads ; // number of readings to average for sun sensor test point
//
// extern float gyro_Z;
// extern float mag_X ;
// extern float mag_Y;


extern TB9051FTGMotorCarrier driver;
extern ESP32Encoder enc;


extern SFE_MAX1704X lipo; // SparkFun Thing Plus ESP32-WROOM onboard fuel gauge (I2C addr 0x36)


#include <Adafruit_INA238.h>

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

class TelemetryLogger {
public:
	// Define the variant type for easier reading
	using TeleValue = std::variant<int, uint32_t, float>;

	struct Entry {
		const char* label;
		TeleValue value;
	};

	// Add a data point to the current frame
	void add(const char* label, TeleValue value) {
		entries.push_back({label, value});
	}

	// Clear the entries (usually called at the start of a loop)
	void clear() {
		entries.clear();
	}

	// Print to Serial/Xbee with labels
	void logToSerial(Print& printer) const {
		for (const auto& e : entries) {
			printer.print(e.label);
			std::visit([&printer](auto&& val) {
				printer.print(val);
			}, e.value);
		}
		printer.println();
	}

	// Print to File/SD as CSV (values only)
	void logToCSV(Print& printer) {
		for (size_t i = 0; i < entries.size(); ++i) {
			std::visit([&printer](auto&& val) {
				printer.print(val);
			}, entries[i].value);

			if (i < entries.size() - 1) {
				printer.print(", ");
			}
		}
		printer.println();
	}

private:
	std::vector<Entry> entries;
};

// inline TelemetryLogger logger;