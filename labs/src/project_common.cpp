#include "project_common.h"
#include <HardwareSerial.h>
#include "definitions.h"
#include "sd_functions.h"
#include "SdFat.h"

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


// File-scope objects (NOT in the header)
// static SdFat sd;
SdFs sd;
SdFile fout;


// --- Global Functions ---

///
/// @return int: user input
/// @return -1: error
int get_int_from_ground() {

    // clear serial buffer
    while (Xbee.available()) {Xbee.read();}

    int timeout = millis() + COMMAND_TIMEOUT ;

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
                Serials.print(received_String.charAt(received_String.length() - 1));
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
            timeout = millis() + COMMAND_TIMEOUT;
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
    static bool first_key = false;
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
        if (!first_key) {

        first_key = true;
        Serials.printf("[CAUTION] Invalid Input (%c) continuing test...", (char)Xbee.read());
        return false;
    }
    }
    return false;
}

bool create_and_open_file(FsFile *dataFile2, const String& directory, const String& filename_preamble) {
    sd.chdir(); // change to root ("/")

    if (!sd.exists(directory)) {
        Serials.println("[INFO] Creating directory: " + directory);
        // mkdir("/"+directory);
        if (!sd.mkdir(directory)) {
            Serials.println("[ERROR] could not create directory.");
            return false;
        }

    }

    if (!sd.chdir(directory)) {
        Serials.println("[ERROR] could not change directory.");
        sd.chdir();
        return false;
    }
    // change_directory("/"+directory);

    char filename[40] ;
    int fileNumber = 1;

    do {
        // We use .c_str() here because snprintf expects a const char*
        snprintf(filename, sizeof(filename),
                 "%s%03d.csv",
                 filename_preamble.c_str(),
                 fileNumber);
        fileNumber++;
    } while (sd.exists(filename) && fileNumber <= 999);

    if (fileNumber > 999) {
        Serials.println("[ERROR] Maximum file number exceeded (999).");
        sd.chdir();
        return false;
    }

    Serials.print("[INFO] Creating file: ");
    Serials.println(filename);

    *dataFile2 = sd.open(filename, FILE_WRITE);

    if (!*dataFile2) {
        Xbee.println("[ERROR] could not create file.");
        sd.chdir();
        return false;
    }

    return true;
} // end create_and_open_file()