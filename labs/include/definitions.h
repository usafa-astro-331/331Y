/*---------------------------------------------------------------------------------------------*/
// Configuration:
/*---------------------------------------------------------------------------------------------*/
#pragma once

// sparkfun thing plus C pinout diagram:
// ../thing_pinout.png
// https://learn.sparkfun.com/tutorials/esp32-thing-plus-usb-c-hookup-guide/hardware-overview




//		THING PLUS LEFT SIDE:
//
//								███████	PCB antenna ██████
//								███████	PCB antenna ██████
//							21
//							22
#define ENCODER_PIN_A		14
#define ENCODER_PIN_B		32
#define MOTOR_PWM_2_PIN		15
#define MOTOR_PWM_1_PIN		33
//						 	27
#define ARDUCAM_CS		 	12
//						 	13 (LED_BUILTIN)		░ status LED (LED_BUILTIN)
//						 	VUSB (5V)				░ charging LED
//						 	EN (ground to disable)	░ power LED
//						 	VBAT						█ RST (reset button)
//						 	┌---┐
//						 	│ JST						█ BOOT (boot mode button)
//						 	│ (3.7 V battery)
//						 	└---┘
//						 	┌---┐
//						 	│ QWIIC
//						 	└---┘		┌----┐
//										│USBC│
//										│USBC│
//
//
//
//
//		THING PLUS RIGHT SIDE:
//
//								███████	PCB antenna ██████
//								███████	PCB antenna ██████
//															4
#define XBEE_TX												17  // ESP32 TX -> XBee DIN
#define XBEE_RX												16  // ESP32 RX <- XBee DOUT
#define SPI_MISO											19	//	19 (MISO)
#define SPI_MOSI											23	//	23 (MOSI)
#define SPI_SCK												18	//	18 (SCK)
//															35			(A5)
//															36			(A4)
#define SUN_NY_PIN											A3	//	39 (A3)
#define SUN_NX_PIN											A2	//	34 (A2)
#define SUN_PY_PIN											A1	//	25 (A1)
#define SUN_PX_PIN											A0	//	26 (A0)
//															GND
//															NC (not connected)
//															3.3 V
//															RESET
//
//
//										┌----┐
//										│USBC│
//										│USBC│




//		ARDUCAM MEGA pinout
//		https://docs.arducam.com/Arduino-SPI-camera/MEGA-SPI/MEGA-Quick-Start-Guide/
//
// 								████████████████████
// 5V			(red)			████████████████████
// GND			(black)			████████▓▓▓▓████████
// SCK			(white)			██████▓▓░░░░▓▓██████
// MISO			(gray)			██████▓▓░░░░▓▓██████
// MOSI			(yellow)		████████▓▓▓▓████████
// ARDUCAM_CS	(orange)		████████████████████
// 								████████████████████



// OTHER:

#define SD_CS_PIN 5  // Chip select pin for the microSD card on Thing Plus
// #define SD_CS_PIN 33  // Chip select pin for the microSD card on Feather Adalogger

#define XBEE_SPEED 57600

#define MOTOR_VOLTAGE           5.05f
#define CT_PER_REV              64 // encoder counts per revolution