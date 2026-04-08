//
// Created by jordan on 2026-04-06.
//

#ifndef LAB_7_ATTITUDE_CONTROL_DUAL_SERIAL_H
#define LAB_7_ATTITUDE_CONTROL_DUAL_SERIAL_H
#include <HardwareSerial.h>

#endif //LAB_7_ATTITUDE_CONTROL_DUAL_SERIAL_H

// void print_both(const HardwareSerial& xbee, const HardwareSerial& serial);
inline void serial_print_double(HardwareSerial& a, HardwareSerial& b, const char message)
{
    a.println(message);
    b.println(message);
}