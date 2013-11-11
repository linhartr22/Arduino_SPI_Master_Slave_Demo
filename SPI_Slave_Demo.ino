/* SPI Slave Demo
 *
 * Slave toggles slave LED upon receipt of cmdBtn master command code.
 *
 * Communication Protocol:
 * Slave acknowledges SS (Slave Select) by sending -1 slave response code and receiving 
 * master command code. On subsequent SS slave sends master command code or -1 slave 
 * response code indicating an unrecognized command has been received. Additional SS's 
 * may be needed to transfer additional data to/from Slave depending on the master command
 * code.
 *
 * Master command codes:
 * cmdBtn.
 * Inform slave of a master button press.
 *
 * SPI pin numbers:
 * SCK   13  // Serial Clock.
 * MISO  12  // Master In Slave Out.
 * MOSI  11  // Master Out Slave In.
 * SS    10  // Slave Select
 *
 * If you found this fun or interesting please make a small donation to my PayPal account 
 * at https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=GTBD7T7BXPGQY. 
 * I have many more Arduino projects in mind and appreciate your support.
 *
 * No commercial use without prior consent.
 *
 * Based on code by: Nick Cammon 2/2011.
 */
// Include SPI (Serial Peripheral Interface) library. Does not support SPI Slave.
#include <SPI.h>
boolean SSlast = LOW;         // SS last flag.
const byte led = 9;           // Slave LED digital I/O pin.
boolean ledState = HIGH;      // LED state flag.
const byte cmdBtn = 1;        // SPI cmdBtn master command code.
const byte cmdLEDState = 2;   // 
//Initialize SPI slave.
void SlaveInit(void) {
  // Initialize SPI pins.
  pinMode(SCK, INPUT);
  pinMode(MOSI, INPUT);
  pinMode(MISO, INPUT);
  pinMode(SS, INPUT);
  // Enable SPI as slave.
  SPCR = (1 << SPE);
}
// SPI Transfer.
byte SPItransfer(byte value) {
  SPDR = value;
  while(!(SPSR & (1<<SPIF)));
  delay(10);
  return SPDR;
}
// The setup() function runs after reset.
void setup() {
  // Initialize serial for troubleshooting.
  Serial.begin(9600);
  // Initialize slave LED pin.
  pinMode(led, OUTPUT);
  digitalWrite(led, ledState);
  // Initialize SPI Slave.
  SlaveInit();
  Serial.println("Slave Initialized");
}
// The loop function runs continuously after setup().
void loop() {
  // Slave Enabled?
  if (!digitalRead(SS)) {
    // Yes, first time?
    if (SSlast != LOW) {
      // Yes, take MISO pin.
      pinMode(MISO, OUTPUT);
      Serial.println("***Slave Enabled.");
      // Write -1 slave response code and receive master command code
      byte rx = SPItransfer(255);
      Serial.println("Initial -1 slave response code sent");
      Serial.println("rx:" + String(rx) + ".");
      // cmdBtn?
      if (rx == cmdBtn) {
        // Acknowledge cmdBtn.
        byte rx = SPItransfer(cmdBtn);
        Serial.println("cmdBtn Acknowledged.");
        Serial.println("rx:" + String(rx) + ".");
        // Toggle LED State
        ledState = !ledState;
        digitalWrite(led, ledState);
      }
      // cmdLEDState?
      else if (rx == cmdLEDState) {
        // Acknowledge cmdLEDState.
        byte rx = SPItransfer(cmdLEDState);
        Serial.println("cmdLEDState Acknowledged.");
        Serial.println("rx:" + String(rx) + ".");
        rx = SPItransfer(ledState);
        Serial.println("ledState:" + String(ledState) + " Sent.");
        Serial.println("rx:" + String(rx) + ".");        
      }
      else {
        // Unrecognized command.
        byte rx = SPItransfer(255);
        Serial.println("Unrecognized Command -1 slave response code sent.");
        Serial.println("rx:" + String(rx) + ".");
      }
      // Update SSlast.
      SSlast = LOW;
    }
  }
  else {
    // No, first time?
    if (SSlast != HIGH) {
      // Yes, release MISO pin.
      pinMode(MISO, INPUT);
      Serial.println("Slave Disabled.");
      // Update SSlast.
      SSlast = HIGH;
    }
  }
}

