#pragma once

#include "dual_serial.h"
#include "project_common.h"

/*---------------------------------------------------------------------------------------------*/
// Get XBee RSSI:
/*---------------------------------------------------------------------------------------------*/
/**
 * @brief Query the connected XBee radio for received signal strength (RSSI).
 *
 * This function places the XBee module into command mode, issues the ATDB
 * command to request the RSSI of the last received packet, parses the hex
 * response into a decimal value, and prints the result over both Serial and
 * Xbee in dBm.
 *
 * @note The function uses blocking delays and loops; it will not return until
 *       the XBee responds correctly with "OK\r".
 *
 * @warning Assumes that Xbee is connected to an XBee radio and already
 *          initialized at the correct baud rate.
 *
 * @post After the function runs, the XBee is returned to data mode using the
 *       ATCN command.
 *
 * @see Digi XBee AT Command Reference for details on ATDB and ATCN commands.
 */

inline void get_sat_rssi() {
  Xbee.println(" standby for RSSI");

  // put the radio in command mode:
  bool not_done = true;
  String ok_response = "OK\r";   //The response we expect.
  String response = String("");  //Create an empty string
  Xbee.println("Starting get_sat_rssi()");

  // Read the text of the response into the response variable
  int xbee_timeout = millis() +2500; // timer to detect unresponsive xbee
  while (not_done) {  // As long as we did not get a response from the Xbee
    response = String("");
    delay(1100);
    Xbee.print("+++");  // Put the Xbee 3 into 'Command Mode'
    // Xbee.print("+++");   // Put the Xbee 3 into 'Command Mode'

    // delay(1100);  // Wait for the Xbee to finish
    while (response.length() < ok_response.length()) {
      // if (millis() > xbee_timeout)  {Xbee.println("Xbee unresponsive");  return;  }
      if (Xbee.available() > 0) {response += (char)Xbee.read(); } // Read a single character at a time
      }
    not_done = !response.equals(ok_response);  // Set the not_done flag to the opposite of the result of equality check
    if (millis() > xbee_timeout)  {Xbee.println("Xbee unresponsive");  return;   }
  }

  // If we got the right response, configure the radio and return true.
  Xbee.print("ATDB\r");  // destination high and destination low addresses set to 0 means all messages will only go
  delay(100);               // Wait for the Xbee
  response = String("");
  while (Xbee.available() > 0) {
    response += (char)Xbee.read();  //Read a single character at a time
  }
  Xbee.println(response);
  Xbee.print("ATCN\r");  // Switch back to data mode
  delay(5); Xbee.readStringUntil('\r');

  String response2 = response;
  uint32_t dec_response = strtoul(response2.c_str(), NULL, 16);

  Xbee.print("RSSI: -");
  Xbee.print(dec_response);
  Xbee.println(" dBm");
  Xbee.println("");

} // end function get_rssi()

inline void toggle_LED()
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  serial_print_twice(Xbee, Serial, digitalRead(LED_BUILTIN) ? "LED ON\n" : "LED OFF\n");
}
