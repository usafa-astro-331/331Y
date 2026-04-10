#pragma once

extern HardwareSerial Xbee; // Serial object for communication with XBee
extern FsFile dataFile;


extern uint32_t timeNext_testPoint; // time of next test point (ms)
extern uint32_t interval_testPoint; // time interval between test points (ms)

extern ICM_20948_I2C imu_sensor;

extern float sun_plusX, sun_minusX, sun_plusY, sun_minusY;
extern float sun_X, sun_Y, sun_direction;
extern float S_mag;
extern int16_t n_sun_sensor_reads ; // number of readings to average for sun sensor test point

extern float gyro_Z;
extern float mag_X ;
extern float mag_Y;


extern TB9051FTGMotorCarrier driver;

extern ESP32Encoder enc;

