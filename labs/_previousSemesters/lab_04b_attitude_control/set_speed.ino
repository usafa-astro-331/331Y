float set_speed() {
  t = millis() - t0;

  if (t - t0 < 10e3) {  // hold still at half speed (10 sec)
    throttlePWM = 0.5;
  }

  else if (t - t0 < 15e3) {  // hold still at half speed (5 sec)
    throttlePWM = 0.5;
    digitalWrite(A0, HIGH);
  }

  else if (t - t0 < 17.5e3) {  // ramp up (2.5 sec)
    elapsed = t - 15e3;
    throttlePWM = 0.5 + 0.1 * elapsed / 2.5e3;
    digitalWrite(A0, LOW);
  }

  else if (t - t0 < 20e3) {  // ramp back down to half speed (2.5 sec)
    elapsed = t - 17.5e3;
    throttlePWM = 0.6 - 0.1 * elapsed / 2.5e3;
  }

  else if (t - t0 < 25e3) {  // hold new position (5 sec)
    throttlePWM = 0.5;
    digitalWrite(A0, HIGH);
  }

  else if (t - t0 < 27.5e3) {  // ramp down (2.5 sec)
    elapsed = t - 25e3;
    throttlePWM = 0.5 - .1 * elapsed / 2.5e3;
    digitalWrite(A0, LOW);
  } else if (t - t0 < 30e3) {  // ramp back up to half speed  (5 sec)
    elapsed = t - 27.5e3;
    throttlePWM = .4 + .1 * elapsed / 2.5e3;
  }

  else if (t - t0 < 35e3) {  // hold new position (5 sec)
    throttlePWM = 0.5;
    digitalWrite(A0, HIGH);
  }

  else if (t - t0 < 40e3) {  // turn off wheel
    throttlePWM = 0;
    digitalWrite(A0, LOW);
    driver.setOutput(0);

  }

  else {
    // while (1) {};
  }

  driver.setOutput(throttlePWM);

  return throttlePWM;
}  // end set_speed()