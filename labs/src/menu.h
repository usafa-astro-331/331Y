#pragma once
#include "att_determ.h"
#include "SerialMenu/SerialMenu.hpp"
#include "communication.h"
// #include "project_common.h"
#include "wheel_speed.h"
#include "electrical.h"
// #include "sd_functions.h"
#include "zmodem.h"

void folder_ls(const String& directory);
void transfer_file_from_directory(const String& directory_name);
void metrology_example();
void pause_refresh();

inline char menu_buf[256];
extern char cmd;

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
        {"\nmain menu:", false, ' ', [](){ menu.show(); }},
        {"1: communication", false, '1', [](){ menu.load(communication_menu, communication_menu_size); menu.show(); } },
        {"2: electrical", false, '2', [](){ menu.load(electrical_menu, electrical_menu_size); menu.show(); } },
        {"3: attitude determination", false, '3', [](){ menu.load(att_determ_menu, att_determ_menu_size); menu.show(); } },
		{"4: attitude control", false, '4', [](){ menu.load(att_control_menu, att_control_menu_size); menu.show(); } },
		{"6: retrieve files", false, '6', [](){ menu.load(file_menu, file_menu_size); menu.show(); } },

        // {"5: remote sensing, false, 'y', [](){ menu.load(remote_sensing_menu); menu.show(); } },
        {" ",       false,  'z', [](){ menu.show(); } }
    };
inline uint8_t main_menu_size = GET_MENU_SIZE(main_menu);

inline SerialMenuEntry electrical_menu[] = {
	{"\nelectrical menu", false, ' ', [](){ menu.show(); } },
	{"1: IV data", false, '1', [](){ IV_data(); pause_refresh();} },
	{"2: battery telemetry",false, '2', [](){ send_battery_telemetry(); pause_refresh();} },
	{"0: return to main menu", false, '0', [](){ menu.load(main_menu,main_menu_size); menu.show(); } },
	{" ", false, 'z', [](){ menu.show(); } },
   };
inline uint8_t electrical_menu_size = GET_MENU_SIZE(electrical_menu);

inline SerialMenuEntry communication_menu[] = {
    {"\ncommunication menu", false, ' ', [](){ menu.show(); } },
	{"1: ADC example",      false, '1', [](){ metrology_example(); pause_refresh();} },
	{"2: get RSSI",      false, '2', [](){ get_sat_rssi(); pause_refresh();} },
    {"3: toggle LED",false, '3', [](){ toggle_LED(); pause_refresh();} },
    {"0: return to main menu", false, '0', [](){ menu.load(main_menu,main_menu_size); menu.show(); } },
    {" ", false, 'z', [](){ menu.show(); } },
   };
inline uint8_t communication_menu_size = GET_MENU_SIZE(communication_menu);

inline SerialMenuEntry att_determ_menu[] = {
	{"\nattitude determination menu", false, ' ', [](){ menu.show(); } },
	{"1: run test",false, '1', [](){ attitude_sensors(); pause_refresh();} },
	{"0: return to main menu", false, '0', [](){ menu.load(main_menu,main_menu_size); menu.show(); } },
	{" ", false, 'z', [](){ menu.show(); } },
   };
inline uint8_t att_determ_menu_size = GET_MENU_SIZE(att_determ_menu);

inline SerialMenuEntry att_control_menu[] = {
	{"\nattitude control menu", false, ' ', [](){ menu.show(); } },
	{"1: set manual RW speed",      false, '1', [](){ manual_set_RW_speed(); pause_refresh();} },
	{"2: stream RW speed",false, '2', [](){ stream_RW_speed(); pause_refresh();} },
	{"3: run test A",false, '3', [](){ full_speed_step_input(); pause_refresh();} },
	{"4: run test B",false, '4', [](){ open_loop_att_control(); pause_refresh();} },
	{"0: return to main menu", false, '0', [](){ menu.load(main_menu,main_menu_size); menu.show(); } },
	{" ", false, 'z', [](){ menu.show(); } },
   };
inline uint8_t att_control_menu_size = GET_MENU_SIZE(att_control_menu);


inline SerialMenuEntry file_menu[] = {
	{"\nfile menu", false, ' ', [](){ menu.show(); } },
	{"1: communication files",      false, '1', [](){ transfer_file_from_directory("communication"); pause_refresh();} },
	{"2: electrical files",false, '2', [](){ transfer_file_from_directory("electrical"); pause_refresh();} },
	{"3: attitude determination files",false, '3', [](){ transfer_file_from_directory("att_determ"); pause_refresh();} },
	{"4: attitude control files",false, '4', [](){ transfer_file_from_directory("att_control"); pause_refresh();} },
	// {"z: manage files",false, '4', [](){ lab7_run_test_B(); pause_refresh();} },
	{"0: return to main menu", false, '0', [](){ menu.load(main_menu,main_menu_size); menu.show(); } },
{" ", false, 'x', [](){ menu.show(); } },
   };
inline uint8_t file_menu_size = GET_MENU_SIZE(att_control_menu);


inline void pause_refresh()
{
    Xbee.print("Press any key to return to menu");

    // wait for input
    while (!Xbee.available()) {}
    Xbee.read();

    // send clear-screen sequence
    byte clear_screen[] = {0x1B, 0x5B, 0x32, 0x4A, 0x1B, 0x5B, 0x48, 0x0D};
    Xbee.write(clear_screen, sizeof(clear_screen));

    // display menu
    menu.show();
}

inline void folder_ls(const String& directory) {
	change_directory(directory);
	directory_listing();
	change_directory("/");
}

inline void transfer_file_from_directory(const String& directory_name) {
	if (!sd.chdir(directory_name)) {
		Serials.printf("[ERROR] directory %c not found \n", directory_name.c_str());
		return;
	}

	directory_listing();

     Serials.println("[REQUEST] Enter the file number to print.");

	int choice = get_int_from_ground();

	if (choice==-98789) {
		Serials.println("no selection/invalid selection. Aborting");
		sd.chdir();
		delay(500);
		return;
	}

       Serials.print("[INFO] You picked file #");  Serials.println(choice);

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
			ZSERIAL.print(F("rz\n"));
			ZSERIAL.flush();
			sendzrqinit();
			delay(200);
			wcs(zfile_name);
			fout.close();
			break;
		}
	}
	dirsz.close();
	saybibi();
	Xbee.println("transfer complete");

	sd.chdir();
	delay(500);
} // end function transfer files from directory()

/**
 * @brief ADC-based metrology example
 *
 * This function continuously reads the ADC pin and logs data to the serial interface.
 *
 * Functionality:
 * - Enables 10-bit ADC resolution.
 * - Reads the analog value from the specified ADC pin.
 * - Logs the following attributes to the serial interface:
 *   - Elapsed time in milliseconds.
 *   - 10-bit ADC value.
 *   - Reduced 3-bit ADC value (3 most significant bits of the 10-bit result).
 *   - A non-linear 3-bit ADC value (reads first 3 bits then saturates at 7)
 * - Provides an exit option by checking for user input ('X').
 *
 * Logging:
 * - Utilizes the `TelemetryLogger` class to structure and log the data.
 * - Clears previous log entries before adding new data points.
 * - All logs are written to the serial interface using the `DualSerial` instance.
 *
 * Behavior:
 * - Waits 1 second before entering the infinite logging loop.
 * - Captures data approximately every 250 milliseconds.
 * - Exits the loop and the function when the user types 'X'.
 *
 * Precondition:
 * - `ADC_PIN` should be defined and properly connected to the ADC source.
 * - The `logger` object and `Serials` instance must be initialized.
 * - `user_has_typed_x()` function must be implemented to detect the exit condition.
 *
 * @see TelemetryLogger
 * @see DualSerial
 * @see user_has_typed_x
 */
inline void metrology_example() {
	Serials.println("'X' to exit");
	delay(1000);

	while (!user_has_typed_x()) {
		// if (user_has_typed_x()){ return;}

		analogReadResolution(10);
		int adc = analogRead(ADC_PIN);

		logger.clear();

		logger.add("time", "ms", (int)millis());
		logger.add("adc_10_bit", "ct", adc);
		logger.add("adc_6_bit", "ct", (adc & 1111110000));
		logger.add("non_linear_6_bit", "ct", ( (adc & 0b1111000000) ?  0b111111: (adc & 0b111111)));

		logger.logToSerial(Serials);

		delay(250);

	} // end while(true)
} // end metrology_example()
