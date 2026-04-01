/*---------------------------------------------------------------------------------------------*/
// Set Wheel Speed:
/*---------------------------------------------------------------------------------------------*/
/**
* @brief Runs a Prescribed Wheel Speed
* @return (float) commanded PWM motor speed
*/
float set_speed_test_B() {
  static uint32_t elapsed; 
  static uint32_t t;
  static float base_speed = 0.6;
  static float ramp_speed = 0.4;
  float motor_PWM_cmd = 0.0;

  t = millis() - t0;

  if (t < 10e3) {  // hold still at half speed (10 sec)
    motor_PWM_cmd = base_speed;
    digitalWrite(LED_PIN, LOW);
  }

  else if (t < 15e3) {  // hold still at half speed (5 sec)
    motor_PWM_cmd = base_speed;
    digitalWrite(LED_PIN, HIGH);
  }

  else if (t < 17.5e3) {  // ramp up (2.5 sec)
    elapsed = t - 15e3;
    motor_PWM_cmd = base_speed + ramp_speed * elapsed / 2.5e3;
    digitalWrite(LED_PIN, LOW);
  }

  else if (t < 20e3) {  // ramp back down to half speed (2.5 sec)
    elapsed = t - 17.5e3;
    motor_PWM_cmd = base_speed + ramp_speed - (ramp_speed * elapsed / 2.5e3);
  }

  else if (t < 25e3) {  // hold new position (5 sec)
    motor_PWM_cmd = base_speed;
    digitalWrite(LED_PIN, HIGH);
  }

  else if (t < 27.5e3) {  // ramp down (2.5 sec)
    elapsed = t - 25e3;
    motor_PWM_cmd = base_speed - (ramp_speed * elapsed / 2.5e3);
    digitalWrite(LED_PIN, LOW);

  } else if (t < 30e3) {  // ramp back up to half speed  (5 sec)
    elapsed = t - 27.5e3;
    motor_PWM_cmd = base_speed - ramp_speed + (ramp_speed * elapsed / 2.5e3);
  }

  else if (t < 35e3) {  // hold new position (5 sec)
    motor_PWM_cmd = base_speed;
    digitalWrite(LED_PIN, HIGH);
  }

  else if (t > 35e3) {  // turn off wheel
    motor_PWM_cmd = 0;
    digitalWrite(LED_PIN, LOW);
    driver.setOutput(0);

  }

  driver.setOutput(motor_PWM_cmd);

  return motor_PWM_cmd;
}  // end set_speed()