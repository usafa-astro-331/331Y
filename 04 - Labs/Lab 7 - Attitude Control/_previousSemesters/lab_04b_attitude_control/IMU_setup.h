// IMU sensor connects with I2C via a QWIIC cable

    // Black = GND.
    // Red = 3.3V.
    // Blue = SDA.
    // Yellow = SCL.


// INA219 pin = Arduino pin

// SDA = SDA
// SCL = SCL
// if not using QWIIC connection, see datasheet for power and ground connections

/* magnetometer range values 
ICM 20948 magnetometers drift quickly

For calibration:
- use 1.0 initially
- then spin satellite and observe mag values
- update mag values below
- re-upload code  
*/

float x_max = 1.0; 
float x_min = 1.0; 
float y_max = 1.0;
float y_min = 1.0; 

// calculate magnetometer calibration--DO NOT CHANGE
float x_range = (x_max - x_min)/2; 
float x_bias = x_max-x_range; 
float y_range = (y_max - y_min)/2; 
float y_bias = y_max-y_range; 

float magx, magy, heading; 