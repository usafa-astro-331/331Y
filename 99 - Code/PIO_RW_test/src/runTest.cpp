#include <Arduino.h>
#include "CONFIG.h"

/*---------------------------------------------------------------------------------------------*/
// Run Test A:
/*---------------------------------------------------------------------------------------------*/
/**
* @brief Runs Test A - Tabletop static test. Initiates a step input at 100% speed 3" into test. 
*         Used to determine RW torque. 
* @return none
*/
void runTestA() {
// //   sd_createDataFile(rtc, &dataFile);
//   digitalWrite(LED_PIN, HIGH); 
//   static uint32_t timeNext_testPoint = millis();
//   bool newUserInput = false;
//   float speed_pwm = 0.0;

//   static uint32_t t0 = millis();
//   while (true) {
    
//     // Check for User Input:
//     char c;
//     if (Serial.available() > 0) {  // Check for user input from USB
//       c = Serial.read();
//       newUserInput = true;
//     }
//     if (Serial1.available() > 0) {  // Check for user input from USB
//       c = Serial1.read();
//       newUserInput = true;
//     }
//     if(newUserInput){
//       newUserInput = false;
//       switch (c) {
//         case 'X':
//         //   dataFile.close();
//           Serial.print("[CAUTION] Test Canceled Early. File closed.");
//           Serial1.print("[CAUTION] Test Canceled Early. File closed.");
//         //   driver.setOutput(0);
//           return;
//         default:
//           Serial.println("[CAUTION] Invalid Input, continuing test...");
//           Serial1.println("[CAUTION] Invalid Input, continuing test...");
//           break;
//       }
//     }

//     // Set RW Motor Speed:
//     if ((millis() - t0) > 3000 && (millis() - t0) < 10000){
//       speed_pwm = 1.0;
//       driver.setOutput(speed_pwm);
//     } else if (millis() - t0 > 10000) {
//       speed_pwm = 0.0;
//       driver.setOutput(speed_pwm);
//     }

//     // Calibrate encoder speed:
//     while ((int)(micros() - period_start_us) < 10000)
//       encoder.autoCalibratePhases();
//     period_start_us += 10000;
//     encoder.update();

//     // Record test point:
//     if (millis() > timeNext_testPoint) {         // Collect Test Point loop
//       timeNext_testPoint += interval_testPoint;  // Update time for next Test Point
//       uint32_t time = millis() - t0;

//       // Collect IMU Test Point:
//       myICM.getAGMT();
//       gyro_Z = -myICM.gyrZ();  //this is negative to convert measurment to KestrelSAT body frame
//       mag_X = myICM.magX();
//       mag_Y = myICM.magY();

//       #if USE_SUN_SENSOR
//         // Collect Sun Sensor Test Point:
//         sun_plusX = ads1015.readADC_SingleEnded(0);   //Sun+X = adc0
//         sun_plusY = ads1015.readADC_SingleEnded(1);   //Sun+Y = adc1
//         sun_minusX = ads1015.readADC_SingleEnded(2);  //Sun-X = adc2
//         sun_minusY = ads1015.readADC_SingleEnded(3);  //Sun-Y = adc3
//       ////////////* find sun direction *////////////////////////////////////////
//       // uncomment sun_plusX & sun_plusY lines to calculate sun direction
//       // (highlight them, CTRL-/)
//       sun_X = sun_plusX - sun_minusX; // you fill in here--remember to end line with ;
//       sun_Y = sun_plusY - sun_minusY; // you fill in here--remember to end line with ;
//       float sun_direction = (atan2(-sun_Y*1.0, -sun_X*1.0) * RAD_TO_DEG +180.0); //  *1.0 converts from int to float;  + 180 adjusts range to 0--360 */
//       //////////////////////////////////////////////////////////////////////////
//       #endif

//       // Get commanded and measured reaction wheel speed:
//       w_RW_cmd = -speed_pwm * 1000.0 * MOTOR_VOLTAGE / 12.0;
//       w_RW_meas = encoder.speed/64/64*60/10;

//       // Print data to .csv file:
//     //   dataFile.print(time);
//     //   dataFile.print(",");
//     //   dataFile.print(gyro_Z);
//     //   dataFile.print(",");
//     //   dataFile.print(mag_X);
//     //   dataFile.print(",");
//     //   dataFile.print(mag_Y);
//     //   dataFile.print(",");
//     //   dataFile.print(sun_direction);
//     //   dataFile.print(",");
//     //   dataFile.print(sun_plusX);
//     //   dataFile.print(",");
//     //   dataFile.print(sun_plusY);
//     //   dataFile.print(",");
//     //   dataFile.print(sun_minusX);
//     //   dataFile.print(",");
//     //   dataFile.print(sun_minusY);
//     //   dataFile.print(",");
//     //   dataFile.print(w_RW_cmd);
//     //   dataFile.print(",");
//     //   dataFile.println(w_RW_meas);

//     //   dataFile.flush();  // save file

//       // Print data to USB & XBee serial:
//       String test_point_string;
//       test_point_string += "t:";
//       test_point_string += time;
//       test_point_string += "gyro_Z:";
//       test_point_string += gyro_Z;
//       test_point_string += ",mag_X:";
//       test_point_string += mag_X;
//       test_point_string += ",mag_Y:";
//       test_point_string += mag_Y;
//       #if USE_SUN_SENSOR
//         test_point_string += ",sun_direction:";
//         test_point_string += sun_direction;
//         test_point_string += ",sun_plusX:";
//         test_point_string += sun_plusX;
//         test_point_string += ",sun_plusY:";
//         test_point_string += sun_plusY;
//         test_point_string += ",sun_minusX:";
//         test_point_string += sun_minusX;
//         test_point_string += ",sun_minusY:";
//         test_point_string += sun_minusY;
//       #endif
//       test_point_string += ",w_RW_cmd:";
//       test_point_string += w_RW_cmd;
//       test_point_string += ",w_RW_meas:";
//       test_point_string += w_RW_meas;

//       test_point_string += "\n";
//       //Print to USB Serial:
//       Serial.print(test_point_string);
//       //Print to XBee:
//       // Serial1.print(test_point_string);

//       // Serial.println(millis() - time + t0);
//     }

//     // End test if complete:
//     if (millis() - t0 > 15000){
//     //   dataFile.close();
//       Serial.print("[INFO] Test A Complete. File closed.");
//     //   Serial1.print("[INFO] Test A Complete. File closed.");
//       return;
//     }
//   }
}  // end function runTest()