/*---------------------------------------------------------------------------------------------*/
// Configuration:
/*---------------------------------------------------------------------------------------------*/
#pragma once

// sparkfun thing plus C pinout diagram:
// ../thing_pinout.pdf
// ../thing_pinout.png
// https://learn.sparkfun.com/tutorials/esp32-thing-plus-usb-c-hookup-guide/hardware-overview

///////////////////////////////////////////////////////////
//		RP Pico (with PiCowbell Adalogger https://learn.adafruit.com/adafruit-picowbell-adalogger-for-pico/pinouts)
//
//                      LEFT SIDE
//
//                                                   USB
//                                          _________USB__________
#define XBEE_TX				0		//  1 	|	                |  40       Vbus
#define XBEE_RX				1		//  2 	|	  LED (GP25)    |  39       Vsys
//                             		//  3 	|	 ░░░░ GND ░░░░  |  38
//                          2		//  4 	|	                |  37       EN
//                          3		//  5 	|	                |  36       3V3
//                          4		//  6 	|	                |  35
#define ARDUCAM_CS          5		//  7 	|	                |  34   GP28
//                              	//  8 	|	 ░░░░ GND ░░░░  |  33
//                          6		//  9 	|	                |  32   GP27
//                          7		//  10	|	                |  31   GP26
//                          8		//  11	|	                |  30       RUN
//                          9		//  12	|	                |  29   GP22
//                             	    //  13	|	 ░░░░ GND ░░░░  |  28
//                          10	    //  14	|	                |  27   GP21
//                          11	    //  15	|	                |  26   GP20
#define ENCODER_PIN_A 		12	    //  16	|	                |  25   GP19
#define ENCODER_PIN_B 		13	    //  17	|	                |  24   GP18
//                             		//  18	|	 ░░░░ GND ░░░░  |  23
#define MOTOR_PWM_2_PIN		14	    //  19	|	                |  22   GP17
#define MOTOR_PWM_1_PIN		15	    //  20	|	                |  21   GP16
//                                          ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔

///////////////////////////////////////////////////////////
///
///                     RIGHT SIDE
///
/////
//                                             USB
//                                      _______USB_______
                                   //   |          Vbus  //  40   (5V/USB)
                                   //   |          Vsys  //  39   (3.3V) (power in)
                                   //   | ░░░░ GND ░░░░  //  38
                                   //   |          EN    //  37
                                   //   |          3V3   //  36   (power out)
                                   //   |                //  35
                                   //   |            28  //  34
                                   //   | ░░░░ GND ░░░░  //  33
                                   //   |            27  //  32
                 #define ADC_PIN                     26  //  31
                                   //                    //  30   RUN
                                   //   |            22  //  29
                                   //   | ░░░░ GND ░░░░  //  28
                                   //   |            21  //  27
                                   //   |            20  //  26
                 #define SPI_MOSI                    19  //  25
                 #define SPI_SCK                     18  //  24
                                    //  | ░░░░ GND ░░░░  //  23
                 #define SD_CS_PIN                   17  //  22
                 #define SPI_MISO                    16  //  21
//                                      ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔



									#define SUN_NY_PIN		A3	//	29 (A3)
									#define SUN_NX_PIN		A2	//	28 (A2)
									#define SUN_PY_PIN		A1	//	27 (A1)
									#define SUN_PX_PIN		A0	//	26 (A0)



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


#define Xbee Serial1

#define XBEE_SPEED 57600

#define MOTOR_VOLTAGE           5.05f
#define CT_PER_REV              64 // encoder counts per revolution

#define COMMAND_TIMEOUT 10000 // time in ms to wait for user response

#define neopixel_pin 17 // for RP2040 adalogger
#define neopixel_count 1 // there is only 1 on this board
// strip.setPixelColor(0, 0, 0, 0);
// strip.show();

#define neopixelWrite(a, b, c, d) {;}
#define RGB_BUILTIN

// #define neopixelWrite(a,b,c,d) { \
// 			strip.setPixelColor((b), (c), (d), 0); \
// 			strip.show(); \
// }

