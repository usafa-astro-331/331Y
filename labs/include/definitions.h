/*---------------------------------------------------------------------------------------------*/
// Configuration:
/*---------------------------------------------------------------------------------------------*/
#pragma once

// sparkfun thing plus C pinout diagram:
// ../thing_pinout.pdf
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
//						 	VBAT (3.7 V)				█ RST (reset button)
//						 	┌----┐
//						 	│ JST						█ BOOT (boot mode button)
//						 	│ (3.7 V battery)
//						 	└----┘
//						 	┌----┐
//						 	│ QWIIC
//						 	└----┘		┌----┐
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
//						 	┌----┐							GND
//						 	│ JST							NC (not connected)
//						 	│ (3.7 V battery)				3.3 V
//						 	└----┘							RESET
//						 	┌----┐
//						 	│ QWIIC
//						 	└----┘		┌----┐
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



//		MOTOR/MOTOR CONTROLLER
//		https://www.pololu.com/product/2997
//		../motor_driver.jpg
//
//
//		MOTOR CONNECTOR
//		https://www.pololu.com/product/4758
//
//					┌	<--	white		ENCODER_PIN_A	┐ -->	to arduino	-->
//					|	<--	blue		ENCODER_PIN_B	┘ -->
//	<--	to motor	|	<--	green		GND
//					|	<--	yellow		5V
//					|	<--	black		Control phase A	┐ -->	to motor controller -->
//					└	<--	red			Control phase B	┘ -->
//
//
//		MOTOR CONTROLLER
//		https://www.pololu.com/product/2997
//		../motor_driver.jpg
//
//				┌-----------------------┐	NC	(DIAG)
//				|						|	NC	(OCM)
//				|	  ██████			|	MOTOR_PWM_1_PIN
//	phase A		|	  ██████			|	MOTOR_PWM_2_PIN
//	phase B		|	  ██████			|	GND	(ENB)
//		GND		|						|	5V	(EN)
//		5V		|						|	NC	(OCC)
//				|				 ░░░	|	5V	(VCC/power in)
//				|				 ░░░	|	GND	(board ground)
//				└-----------------------┘	NC	(VM)





//		SOLAR PANEL POWER
//		https://learn.adafruit.com/adafruit-ina237-dc-current-voltage-power-monitor
//
//						(QWIIC)
//		(VCC)  NC	┌---████---┐							  ↗
//		(GBD)  NC	|		   |						     ╱
//		(SCL)  NC	|		 ██|	Vin- --> to pot --> ^˅^˅╱^˅^˅ --> to GND/tie to solar ground
//		(SDA)  NC	|		 ██|	NC (Vbus)			   ╱
//		(Vbus) NC	|		 ██|	Vin+ --> to	solar power +
//		(Vin-) NC	|		   |
//		(Vin+) NC	|		   |
//		(ALRT) NC	└---████---┘
//						(QWIIC)




// OTHER:

#define SD_CS_PIN 5  // Chip select pin for the microSD card on Thing Plus
// #define SD_CS_PIN 33  // Chip select pin for the microSD card on Feather Adalogger

#define XBEE_SPEED 57600

#define MOTOR_VOLTAGE           5.05f
#define CT_PER_REV              64 // encoder counts per revolution