#include "project_common.h"
#include <HardwareSerial.h>

// Initialize Global Objects
// (Note: Ensure Serial1/Serial2 match your actual hardware wiring)
HardwareSerial Xbee(2);
DualSerial Serials(Serial, Xbee);
TelemetryLogger logger;

// --- TelemetryLogger Implementation ---

void TelemetryLogger::add(const char* label, const char* unit, TeleValue value) {
    entries.push_back({label, unit, value});
}

void TelemetryLogger::clear() {
    entries.clear();
}

void TelemetryLogger::logToSerial(Print& printer) {
    for (const auto& e : entries) {
        printer.print(e.label);
        printer.print(":");
        std::visit([&printer](auto&& val) {
            printer.print(val);
            printer.print(", ");
        }, e.value);
    }
    printer.println();
}

void TelemetryLogger::logToCSV(Print& printer) {
    for (const auto& e : entries) {
        std::visit([&printer](auto&& val){
            printer.print(val);
            printer.print(", ");
        }, e.value);
    }
    printer.println();
}

void TelemetryLogger::create_CSV_header(Print& printer) {
    for (const auto& e : entries) {
        printer.print(e.label);
        printer.print("_");
        printer.print(e.unit);
        printer.print(", ");
    }
    printer.println();
}

// --- DualSerial Implementation ---

DualSerial::DualSerial(HardwareSerial& s1, HardwareSerial& s2)
    : serial1(s1), serial2(s2) {}

size_t DualSerial::write(uint8_t c) {
    serial1.write(c);
    return serial2.write(c);
}

// --- Global Functions ---

int get_command_from_ground_station() {
    int received_int = 0;
    if (Xbee.available()) {
        String received_string = Xbee.readStringUntil('\n');
        delay(10);
        if (received_string.length() == 0) return -1;

        Xbee.print("Received from Serial: ");
        Xbee.println(received_string);
        received_int = received_string.toInt();
    }
    return received_int;
}

bool user_has_typed_x() {
    if (Xbee.available() == 0) return false;

    switch (tolower(Xbee.peek())) {
        case 'x':
            Xbee.read(); // Clear the character from buffer
            Serials.print("[CAUTION] Test Canceled Early. File closed.");
            return true;

        case EOF: // if no input (peek returns end-of-file), do nothing
            return false;
            // break;

        default:
            // Read and report invalid input
            Serials.printf("[CAUTION] Invalid Input (%c) continuing test...", Xbee.read());
            return false;
    }
}