#pragma once

#include "dual_serial.h"
#include <Adafruit_INA238.h>
// auto  ina238 = Adafruit_INA238();

#include "menu.h"
#include "communication.h"
#include "electrical.h"
#include "wheel_speed.h"
#include "att_determ.h"

extern HardwareSerial Xbee; // Serial object for communication with XBee
extern FsFile dataFile;   // data file object


extern uint32_t timeNext_testPoint; // time of next test point (ms)
extern const uint32_t interval_testPoint; // time interval between test points (ms)

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
	Serial.read();
	delay(100);
	int received_int = 0; // default to no command
	while (received_int==0)
	{
		// if (Xbee.available())
		// {
		// 	String received_string = Xbee.readStringUntil('\n');
		// 	received_string.trim();
		// 	if (received_string.length() == 0) return -1;
		// 	Serial.print("Received from XBee: ");
		// 	Serial.println(received_string);
		// 	received_int = received_string.toInt();
		// }
		// else
		if (Serial.available())
		{
			String received_string = Serial.readStringUntil('\n');
			received_string.trim();
			if (received_string.length() == 0) return -1;
			Serial.print("Received from Serial: ");
			Serial.println(received_string);
			received_int = received_string.toInt();
		}
	}
	return received_int;
}