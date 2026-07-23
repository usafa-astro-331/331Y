#include "project_common.h"
#include <HardwareSerial.h>

#include "definitions.h"
#include "SdFat.h"

extern SdFs sd;
extern FsFile dataFile;


// Initialize Global Objects
// (Note: Ensure Serial1/Serial2 match your actual hardware wiring)

DualSerial Serials(Serial, Xbee);


// --- DualSerial Implementation ---

DualSerial::DualSerial(HardwareSerial& s1, HardwareSerial& s2)
    : serial1(s1), serial2(s2) {}

size_t DualSerial::write(uint8_t c) {
    serial1.write(c);
    return serial2.write(c);
}



bool create_and_open_file(FsFile* dataFile2, const String& directory, const String& filename_preamble) {


    Serials.println(directory); Serials.println(filename_preamble);


    sd.chdir(); // change to root ("/")

    if (sd.exists("folder1")) {
        if (sd.rmdir("folder1")) {
            Serials.println("folder1 removed");
        }
        else {Serials.println("remove1 failed");}
    }

        if (sd.mkdir("folder1")) {
            Serials.println("folder1 created");
        }
        else {
            Serials.println("mkdir1 failed");
        }

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