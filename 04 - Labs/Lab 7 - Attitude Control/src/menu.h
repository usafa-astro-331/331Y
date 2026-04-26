#pragma once
#include "att_determ.h"
#include "../lib/SerialMenu/src/SerialMenu.hpp"
#include "communication.h"
#include "main.h"
#include "wheel_speed.h"
#include "electrical.h"
#include "sd_functions.h"
#include "zmodem.h"
#include "../lib/zmodem/zmodem.h"

// inline char* cmd = oneKbuf;

// static String cmd;


void folder_ls(const String& directory);
void transfer_file_from_directory(const String& directory_name);

inline char menu_buf[256];
extern char cmd;

//
// const SerialMenu& menu = SerialMenu::get();
//
// const SerialMenuEntry communication_menu[];
// const uint8_t communication_menu_size;
//
// const SerialMenuEntry main_menu[] = {
//     {"1: communication", false, '1',
//         [](){menu.load(communication_menu, communication_menu_size);
//             menu.show(); } },
//     {"2: electrical", false, '2', Serial.println("electrical")},
// };
// constexpr uint8_t main_menu_size = GET_MENU_SIZE(main_menu);
//
// const SerialMenuEntry communication_menu[] = {
//     {"0: return to main menu", false, '0',
//         [](){menu.load(main_menu, main_menu_size);
//             menu.show(); } },
//     {"1: do lab A", false, '2', Serial.println("do lab A")},
// };
// constexpr uint8_t communication_menu_size = GET_MENU_SIZE(communication_menu);
//
//

void pause_refresh();

class SerialMenu;
 SerialMenu& menu = SerialMenu::get();

    // Forward menu declarations; some are referenced before definition
    extern  SerialMenuEntry main_menu[];
    extern  SerialMenuEntry communication_menu[];
    extern  SerialMenuEntry electrical_menu[];
    extern  SerialMenuEntry att_determ_menu[];
    extern  SerialMenuEntry att_control_menu[];
    extern  SerialMenuEntry remote_sensing_menu[];
	extern	SerialMenuEntry file_menu[];

extern uint8_t main_menu_size;
extern uint8_t communication_menu_size;
extern uint8_t electrical_menu_size;
extern uint8_t att_determ_menu_size;
extern uint8_t att_control_menu_size;
extern uint8_t remote_sensing_menu_size;
extern uint8_t file_menu_size;


    // Definition of menu1:
    // A menu entry is defined with four fields.
    // -Text can be embedded directly or you can reference a string name
    // -Text in FLASH via PROGMEM is flagged as true, else flagged as false
    // -Declare the keypress assigned to a menu entry (converts to lowercase)
    // -Declare the callback as a lambda function or use a function pointer

inline SerialMenuEntry main_menu[] = {
        {"main menu:", false, ' ', [](){ menu.show(); }},
        {"1: communication", false, '1', [](){ menu.load(communication_menu, communication_menu_size); menu.show(); } },
        {"2: electrical", false, '2', [](){ menu.load(electrical_menu, electrical_menu_size); menu.show(); } },
        {"3: attitude determination", false, 'y', [](){ menu.load(att_determ_menu, att_determ_menu_size); menu.show(); } },
		{"4: attitude control", false, '4', [](){ menu.load(att_control_menu, att_control_menu_size); menu.show(); } },
		{"6: file menu", false, '6', [](){ menu.load(file_menu, file_menu_size); menu.show(); } },

        // {"5: remote sensing, false, 'y', [](){ menu.load(remote_sensing_menu); menu.show(); } },
        {" ",       false,  'z', [](){ menu.show(); } }
    };
inline uint8_t main_menu_size = GET_MENU_SIZE(main_menu);

inline SerialMenuEntry electrical_menu[] = {
	{"electrical menu", false, ' ', [](){ menu.show(); } },
	{"0: return to main menu", false, '0', [](){ menu.load(main_menu,main_menu_size); menu.show(); } },
	{"1: IV data", false, '1', [](){ IV_data(); pause_refresh();} },
	{"2: battery telemetry",false, '2', [](){ send_battery_telemetry(); pause_refresh();} },
	{" ", false, 'z', [](){ menu.show(); } },
   };
inline uint8_t electrical_menu_size = GET_MENU_SIZE(electrical_menu);

    SerialMenuEntry communication_menu[] = {
    {"communication menu", false, ' ', [](){ menu.show(); } },
    {"0: return to main menu", false, '0', [](){ menu.load(main_menu,main_menu_size); menu.show(); } },
    {"1: get RSSI",      false, '1', [](){ get_sat_rssi(); pause_refresh();} },
    {"2: toggle LED",false, '2', [](){ toggle_LED(); pause_refresh();} },
    {" ", false, 'z', [](){ menu.show(); } },
   };
inline uint8_t communication_menu_size = GET_MENU_SIZE(communication_menu);

inline SerialMenuEntry att_determ_menu[] = {
	{"attitude determination menu", false, ' ', [](){ menu.show(); } },
	{"0: return to main menu", false, '0', [](){ menu.load(main_menu,main_menu_size); menu.show(); } },
	{"1: run test",false, '1', [](){ lab6_run_test(); pause_refresh();} },
	{" ", false, 'z', [](){ menu.show(); } },
   };
inline uint8_t att_determ_menu_size = GET_MENU_SIZE(att_determ_menu);

inline SerialMenuEntry att_control_menu[] = {
	{"attitude control menu", false, ' ', [](){ menu.show(); } },
	{"0: return to main menu", false, '0', [](){ menu.load(main_menu,main_menu_size); menu.show(); } },
	{"1: set manual RW speed",      false, '1', [](){ manual_set_RW_speed(); pause_refresh();} },
	{"2: stream RW speed",false, '2', [](){ stream_RW_speed(); pause_refresh();} },
	{"3: run test A",false, '3', [](){ lab7_run_test_A(); pause_refresh();} },
	{"4: run test B",false, '4', [](){ lab7_run_test_B(); pause_refresh();} },
	{" ", false, 'z', [](){ menu.show(); } },
   };
inline uint8_t att_control_menu_size = GET_MENU_SIZE(att_control_menu);


inline SerialMenuEntry file_menu[] = {
	{"file menu", false, ' ', [](){ menu.show(); } },
	{"0: return to main menu", false, '0', [](){ menu.load(main_menu,main_menu_size); menu.show(); } },

	{"1: communication files",      false, '1', [](){ manual_set_RW_speed(); pause_refresh();} },
	{"2: electrical files",false, '2', [](){ transfer_file_from_directory("electrical"); pause_refresh();} },
	{"3: attitude determination files",false, '3', [](){ lab7_run_test_A(); pause_refresh();} },
	{"4: attitude control files",false, '4', [](){ lab7_run_test_B(); pause_refresh();} },
	{"z: manage files",false, '4', [](){ lab7_run_test_B(); pause_refresh();} },
	{" ", false, 'x', [](){ menu.show(); } },
   };
inline uint8_t file_menu_size = GET_MENU_SIZE(att_control_menu);


inline void pause_refresh()
{
    Serial.print("Press any key to return to menu");

    // wait for input
    while (!Serial.available());
    Serial.read();

    // send clear-screen sequence
    byte clear_screen[] = {0x1B, 0x5B, 0x32, 0x4A, 0x1B, 0x5B, 0x48, 0x0D};
    Serial.write(clear_screen, sizeof(clear_screen));

    // display menu
    menu.show();
}

inline void folder_ls(const String& directory) {
	change_directory(directory);
	directory_listing();
	change_directory("/");
}

inline void transfer_file_from_directory(const String& directory_name) {
	change_directory(directory_name);

	directory_listing();

//
//   if (fileCount > 0) {
	// Clear any existing serial input buffer
	    while (Serial.available()) {
	      Serial.read();
	    }
     Serial.println("[REQUEST] Enter the file number to print.");
//
//
     // Wait for user input with timeout
     unsigned long timeout = millis() + 15000; // 15 second timeout
     while (!Serial.available() && millis() < timeout) {
       delay(10);
     }
//
     if (millis() >= timeout) {
       Serial.println("[CAUTION] Input timeout, returning to Menu.");
       return;
     }
//
     int choice = Serial.parseInt();   // read number user typed

     // Clear remaining characters in buffer
     while (Serial.available()) {
       Serial.read();
     }
//
     // if (choice > 0 && choice <= fileCount) {
       Serial.print("[INFO] You picked file #");  Serial.println(choice);

	FsFile dirsz;

	dirsz.openCwd();
	dirsz.rewindDirectory();

	int filenum = 1;
	while (fout.openNext(&dirsz)) {
		if (filenum < choice) {
			filenum++;
			fout.close();
		}
		else {
			fout.getName(zfile_name, 256);
			wcs(zfile_name);
			break;
		}
	}
	dirsz.close();
	saybibi();
	Serial.println("transfer complete");


       // if (!fout.open(zfile_name, O_READ)) {
       //   Serial.print("file open failed");



	change_directory("/");
}


