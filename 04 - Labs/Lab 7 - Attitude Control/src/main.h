#pragma once

#include "dual_serial.h"
#include <Adafruit_INA238.h>
auto ina238 = Adafruit_INA238();

#include "menu.h"

extern HardwareSerial Xbee; // Serial object for communication with XBee
extern FsFile dataFile;


extern uint32_t timeNext_testPoint; // time of next test point (ms)
extern uint32_t interval_testPoint; // time interval between test points (ms)

extern ICM_20948_I2C imu_sensor;

extern float sun_plusX, sun_minusX, sun_plusY, sun_minusY;
extern float sun_X, sun_Y, sun_direction;
extern float S_mag;
extern int16_t n_sun_sensor_reads ; // number of readings to average for sun sensor test point

float gyro_Z=0.0;
float mag_X ;
float mag_Y;


extern TB9051FTGMotorCarrier driver;
ESP32Encoder enc;

int get_command_from_ground_station();

void get_sat_rssi();

SFE_MAX1704X lipo; // SparkFun Thing Plus ESP32-WROOM onboard fuel gauge (I2C addr 0x36)


#include <Adafruit_INA238.h>
