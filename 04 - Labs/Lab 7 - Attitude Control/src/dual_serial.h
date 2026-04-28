//
// Created by jordan on 2026-04-06.
//

# pragma once

#include <HardwareSerial.h>

// #endif //LAB_7_ATTITUDE_CONTROL_DUAL_SERIAL_H

// void print_both(const HardwareSerial& xbee, const HardwareSerial& serial);
inline void serial_print_twice(HardwareSerial& a, HardwareSerial& b, String message)
{
    a.print(message);
    b.print(message);
}