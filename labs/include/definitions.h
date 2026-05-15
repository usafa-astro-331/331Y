/*---------------------------------------------------------------------------------------------*/
// Configuration:
/*---------------------------------------------------------------------------------------------*/
#pragma once

// sparkfun thing plus C pinout diagram:
// ../thing_pinout.pdf
// ../thing_pinout.png
// https://learn.sparkfun.com/tutorials/esp32-thing-plus-usb-c-hookup-guide/hardware-overview

///////////////////////////////////////////////////////////
//		RP2040 Adalogger Left Side						(SEE BELOW FOR RIGHT SIDE PINS)
//							 ____________________________
//							|	░░░░░░░SD SLOT░░░░░░░░	|
//							|	░░░░░░░░░░░░░░░░░░░░░░	|
//							2// (SDA)░░░░░░░░░░░░░░░░░	| 4
//							3// (SCL)░░░░░░░░░░░░░░░░░	| 0		//	(TX)
#define ENCODER_PIN_A		5//	░░░░░░░░░░░░░░░░░░░░░░	| 1		//	(RX)
#define ENCODER_PIN_B		6//	░░░░░░░░░░░░░░░░░░░░░░	| 8		//	(MISO)
#define MOTOR_PWM_2_PIN		9//	░░░░░░░░░░░░░░░░░░░░░░	| 15	//	(MOSI)
#define MOTOR_PWM_1_PIN		10//_______					| 14	//	(SCK)
//						 	11//|QWIIC|			 ___	| 25	//
#define ARDUCAM_CS		 	12//¯¯¯¯¯¯			|RGB|	| 24	//
//						 	13//(LED_BUILTIN)	¯¯¯¯	| A3	//	29 (A3)
//						 	|VUSB (5V)					| A2	//	28 (A2)
//						 	|EN (ground to disable)		| A1	//	27 (A1)
//						 	|VBAT (3.7 V)				| A0	//	26 (A0)
//						 	┌----┐				BOOT█	| GND
//						 	│ JST						| 3.3 V
//						 	│ (3.7 V battery)	RST █	| 3.3 V
//						 	└----┘___	┌----┐	 ___	| RESET
//							|____|LED|__│USBC│__|CHG|___|
//										│USBC│
//
//
///////////////////////////////////////////////////////////
//		RP2040 Adalogger RIGHT SIDE:
//							 ____________________________
//							|	░░░░░░░SD SLOT░░░░░░░░	|
//							|	░░░░░░░░░░░░░░░░░░░░░░	|
//															4
									#define XBEE_TX			0		//	(TX)
									#define XBEE_RX			1		//	(RX)T
									#define SPI_MISO		8		//	(MISO)
									#define SPI_MOSI		15	//	(MOSI)
									#define SPI_SCK			14	//	(SCK)
									//						25	//
									#define ADC_PIN			24	//
									#define SUN_NY_PIN		A3	//	29 (A3)
									#define SUN_NX_PIN		A2	//	28 (A2)
									#define SUN_PY_PIN		A1	//	27 (A1)
									#define SUN_PX_PIN		A0	//	26 (A0)
//						 	┌----┐				BOOT█	| GND
//						 	│ JST						| 3.3 V
//						 	│ (3.7 V battery)	RST █	| 3.3 V
//						 	└----┘___	┌----┐	 ___	| RESET
//						 	|____|LED|__│USBC│__|CHG|___|
//						 				│USBC│



///////////////////////////////////////////////////////////
///		Sparkfun XBEE Explorer pinout
///		https://www.sparkfun.com/sparkfun-xbee-explorer-usb.html
///
///							   ANT
///		NC			▓▓▓▓▓▓▓▓⎽⎼⎻▔▔▔▔▔▔⎺⎻⎼▓▓▓▓▓▓▓▓		NC
///		NC			▓▓▓▓▓▓⎽⎼⎻          ⎺⎻⎼▓▓▓▓▓▓		NC
///			DOUT	▓▓▓▓⎽⎼⎻              ⎺⎻⎼▓▓▓▓		NC
///			DIN		▓▓▓|                |▓▓▓		NC
///		NC			▓▓▓|                |▓▓▓		NC
///		NC			▓▓▓|                |▓▓▓		NC
///		NC			▓▓▓|                |▓▓▓		NC
///		NC			▓▓▓|    DIGI XBEE   |▓▓▓		NC
///		NC			▓▓▓|________________|▓▓▓		NC
///		NC			▓▓▓▓▓▓▓▓▓█████▓▓▓▓▓▓▓▓▓▓		NC
///		NC			▓▓▓▓▓▓▓▓▓█████▓▓▓▓▓▓▓▓▓▓		NC
///			GND		▓▓▓▓▓▓▓▓▓█████▓▓▓▓▓▓▓▓▓▓	5V
///							micro USB



///////////////////////////////////////////////////////////
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


///////////////////////////////////////////////////////////
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
//				┌-----------------------┐		NC	(DIAG)
//				|						|		NC	(OCM)
//				|	  ██████			|	MOTOR_PWM_1_PIN
//	phase A		|	  ██████			|	MOTOR_PWM_2_PIN
//	phase B		|	  ██████			|	GND	(ENB)
//		GND		|						|	5V	(EN)
//		5V		|						|		NC	(OCC)
//				|				 ░░░	|	5V	(VCC/power in)
//				|				 ░░░	|	GND	(board ground)
//				└-----------------------┘		NC	(VM)




///////////////////////////////////////////////////////////
//		SOLAR PANEL POWER
//		https://learn.adafruit.com/adafruit-ina237-dc-current-voltage-power-monitor
//
//						(QWIIC)
//		(VCC)  NC	┌---████---┐							  ↗
//		(GND)  NC	|		   |						     ╱
//		(SCL)  NC	|		 ██|	Vin- --> to pot --> ^˅^˅╱^˅^˅ --> to GND/tie to solar ground
//		(SDA)  NC	|		 ██|	NC (Vbus)			   ╱
//		(Vbus) NC	|		 ██|	Vin+ --> to	solar +
//		(Vin-) NC	|		   |
//		(Vin+) NC	|		   |
//		(ALRT) NC	└---████---┘
//						(QWIIC)


///////////////////////////////////////////////////////////
//		POTENTIOMETER
//
//				║  ║  □  □  □  □  □  ▒▒▒▒  □  □  □  □  □  ║  ║
//				║  ║  □  □  □  □  □  ▒▒▒▒  □  □  □  □  □  ║  ║
//				╟──╫──□  □  □  □  A  ▒▒▒▒  B  □  □  □  □  ║  ║
//				║  ║  □  □  □  □  A  ▒▒▒▒  B  □  □  □  □──╢  ║
//		3.3V <──╫──╫──□  □  □  □  A  ▒▒▒▒  B  □  □  □  □──╫──╫──> Vin-
//				║  ║  □  □  □  □  □  ▒▒▒▒  □  □  □  □  □  ║  ║
//				║  ║  □  □  □  □  □  ▒▒▒▒  □  □  □  □  □  ║  ║
//				⏚ 5V  □  □  □  □  □  ▒▒▒▒  □  □  □  □  □  ⏚ 5V
//
//	potentiometer pointing left		|	potentiometer pointing right
//	for quantization demo:			|	for solar panel characterization:
//									|
//					▒▒▒▒			|		▒▒▒▒
//			□ □	□ ▄▄▄▄▄▄▄▄▄			|	 ▄▄▄▄▄▄▄▄▄ □  □  □
//	(GND)	□ □ A-███▀▀▀███			|	 ███▀▀▀███-B  □		(NC)
//	(ADC_PIN) □	A-██  🟐	 ██			|	 ██  🟐 	██-B  □	(GND)
//	(3.3V)	□ □	A-███▄▄▄███			|	 ███▄▄▄███-B  □	(Vin- from current sensor)
//			□ □	□ ▀▀▀▀▀▀▀▀▀			|	 ▀▀▀▀▀▀▀▀▀ □  □  □
//					▒▒▒▒					▒▒▒▒


///////////////////////////////////////////////////////////
// sparkfun 9DOF IMU via QWIIC/I2C
//    black: GND
//    red: 3.3V
//    blue: SDA
//    yellow: SCL
//


///////////////////////////////////////////////////////////
//		PHOTOTRANSISTORS
//		https://www.digikey.com/en/products/detail/american-bright-optoelectronics-corporation/BPT-NPG3C1/9678552
//
// 4x phototransistor sun sensors via voltage divider
//
//   3V3         (phototransistor)
//    |         ↙
//    └-----(PT)-----┬-----<1kΩ>-----┐
//             sensor pin           |
//           (SUN_PX_PIN, etc.)		⏚
//                                 GND
//

// OTHER:

#define SD_CS_PIN 33  // Chip select pin for the microSD card on Feather Adalogger

#define Xbee Serial2

#define XBEE_SPEED 57600

#define MOTOR_VOLTAGE           5.05f
#define CT_PER_REV              64 // encoder counts per revolution

#define COMMAND_TIMEOUT 10000 // time in ms to wait for user response

#define neopixelWrite(...) ((void)0)


