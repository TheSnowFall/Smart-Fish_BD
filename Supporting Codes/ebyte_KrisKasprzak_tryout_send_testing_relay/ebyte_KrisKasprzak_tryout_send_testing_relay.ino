/*

  This example shows how to connect to an EBYTE transceiver
  using an Arduino Nano

  This code for for the sender


  connections
  Module      Arduino
  M0          4
  M1          5
  Rx          2 (This is the MCU Tx lined)
  Tx          3 (This is the MCU Rx line)
  Aux         6
  Vcc         3V3
  Gnd         Gnd

*/

#include <SoftwareSerial.h>
#include "EBYTE.h"

#define PIN_RX 2
#define PIN_TX 3
#define PIN_M0 4
#define PIN_M1 5
#define PIN_AX 6

// i recommend putting this code in a .h file and including it
// from both the receiver and sender modules

// these are just dummy variables, replace with your own
// struct DATA {
//   unsigned long Count;
//   int Bits;
//   float Volts;
//   float Amps;

// };
byte payload[4] = {0,};
byte prevData[4] = {0,};
byte received_payload[4] = {0,};


uint8_t address = 0x17;              // 1 byte
uint16_t data = 0b0001101010110000;  // 16 bits
uint8_t ending = 0x33;               // 1 byte

int Chan;
// DATA MyData;

// you will need to define the pins to create the serial port
SoftwareSerial ESerial(PIN_RX, PIN_TX);


// create the transceiver object, passing in the serial and pins
EBYTE Transceiver(&ESerial, PIN_M0, PIN_M1, PIN_AX);

void setup() {

  Serial.begin(9600);

  // start the transceiver serial port--i have yet to get a different
  // baud rate to work--data sheet says to keep on 9600
  ESerial.begin(9600);

  Serial.println("Starting Sender");

  // this init will set the pinModes for you
  Transceiver.init();

  // all these calls are optional but shown to give examples of what you can do

  // Serial.println(Transceiver.GetAirDataRate());
  // Serial.println(Transceiver.GetChannel());

  // Transceiver.SetAddressH(1);
  // Transceiver.SetAddressL(0);
  // Chan = 5;
  // Transceiver.SetChannel(Chan);
  // save the parameters to the unit,
  // Transceiver.SaveParameters(PERMANENT);

  // you can print all parameters and is good for debugging
  // if your units will not communicate, print the parameters
  // for both sender and receiver and make sure air rates, channel
  // and address is the same
  Transceiver.PrintParameters();
}

void loop() {

    payload[0] = address;
    payload[1] = (data >> 8) & 0xFF;  // High byte of data
    payload[2] = data & 0xFF;         // Low byte of data
    payload[3] = ending;


  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    if (input.length() == 12) {  // Check if input length is 12
      data = 0;
      for (int i = 0; i < 12; i++) {
        if (input.charAt(i) == '1') {
          data |= (1 << (11 - i));  // Set corresponding bit to 1
        }
    //     else {
    //   Serial.println("Invalid input. Nothing excepted without 0 & 1");
    // }
      }
      data <<= 4;  // Add four zeros
      Serial.print("Data stored: ");
      Serial.println(data, BIN);
    } else {
      Serial.println("Invalid input. Input should be 12 characters long.");
    }
  }

  if (memcmp(prevData, payload, sizeof(payload)) != 0) {
        Serial.print("Sending: ");
        for (int i = 0; i < 4; i++) {
          ESerial.write(payload[i]);
          if (i == 1 || i == 2) {
            Serial.print(" | 0b");
            printBinary(payload[i]);
          } else {
            Serial.print(" | 0x");
            Serial.print(payload[i], HEX);
          }
        }
        // Update prevData to match the current payload
        memcpy(prevData, payload, sizeof(payload));
        Serial.println(" ");
      }
if (ESerial.available() >= 3) {
  Serial.print(" Response Received:");
  for (int i = 0; i < 4; i++) {
    payload[i] = ESerial.read();
    if (i == 1 || i == 2) {
      Serial.print(" | 0b");
      printBinary(payload[i]);
    } else {
      Serial.print(" | 0x");
      Serial.print(payload[i], HEX);
    }
  }
  Serial.println(" ");
} 


  delay(100);  // Delay for readability
}
void printBinary(uint8_t byte) {
  for (int i = 7; i >= 0; i--) {
    Serial.print((byte >> i) & 0x01);
  }
}