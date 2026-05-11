#include "project_common.h"
#include <HardwareSerial.h>
#include "definitions.h"

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

///
/// @return int: user input
/// @return -1: error
int get_int_from_ground() {
    int timeout = millis() + SERIAL_TIMEOUT ;

    String received_String ="";

    while (millis() < timeout) {

        if (received_String.length() > 8) {
        Serials.println("Input too long: ");
            Serials.print(received_String);
            Serials.println("; Aborting.");
        return -98789;
        }

        switch (Xbee.peek()){

        case EOF: // no input
            break;

        case '-':
            if (received_String.length() == 0) { // entered '-' at beginning of number --> this is fine
                received_String += (char)Xbee.read();
            }
            else{         // entered '-' in middle of number
                Serials.print("Unknown entry: '");
                Serials.print(received_String);
                Serials.print("-'\n");
                Serials.println("Retry with integers only. 'Enter' when complete. 'X' to exit" );
                received_String = "";
            }   // end of if/else
            break; // end of case '-':

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            received_String += (char)Xbee.read();
            Serials.print(received_String.charAt(received_String.length() - 1)); //
            timeout = millis() + SERIAL_TIMEOUT;
            break;

        case '\n': case'\r': // entry complete
            while (Xbee.available()) {
                // clear Serial buffer
                Xbee.read();
            }
            Serials.print("\nReceived: ");
            Serials.println(received_String);
            return received_String.toInt();

        case 'x': case 'X':
            Serials.println("Aborting.");
            return -98789;

        default:
            Serials.print("Unknown entry: ");
            Serials.println((char)Xbee.read());
            delay(100);
            Serials.println(". Integers only. 'Enter' when complete. 'X' to abort" );
            break;
        } // end switch/case
    } // end while

    if (received_String.length() > 0) {
    Serials.print("Received: ");
    Serials.println(received_String);
    return received_String.toInt();
    }
    else {
        Serials.println("No input received. Aborting.");
        return -98789;
    }

} // end get_int_from_ground()

///
/// @return String: user input
/// @return "-1": error
bool get_command_from_ground() {
    int timeout = millis() + COMMAND_TIMEOUT ;

    String received_String;

    while (millis() < timeout) {

        if (received_String.length() > 8) {
        Serials.println("Input too long. Aborting.");
        return false;
        }

        switch (Xbee.peek()){
        case EOF: // no input
            break;

        default:
            received_String += Xbee.read();
            Serials.print(received_String.charAt(received_String.length() - 1));
            timeout = millis() + COMMAND_TIMEOUT;
            break;

        case '\n': case'\r': // entry complete
            // clear Serial buffer
            while (Xbee.available()) {
                Xbee.read();
            }

            if (received_String.length() > 0) {
                Serials.print("\nReceived: ");
                Serials.println(received_String);
            }
            return true;

        case 'x': case 'X':
            Serials.println("Aborting.");
            return false;


        } // end switch/case
    } // end while

    if (received_String.length() > 0) {
    Serials.print("Received: ");
    Serials.println(received_String);
    return true;
    }
    else {
        Serials.println("No input received. Aborting.");
        return false;
    }

} // end get_command_from_ground()

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