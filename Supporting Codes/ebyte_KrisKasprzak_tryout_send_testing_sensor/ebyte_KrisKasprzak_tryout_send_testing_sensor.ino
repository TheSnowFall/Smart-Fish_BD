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

byte payload[3] = {0x26, 0xAB, 0x22}; // Change payload values as per requirement
byte prevData[3] = {0,};
byte sen_payload[10] = {0,};

SoftwareSerial ESerial(PIN_RX, PIN_TX);
EBYTE Transceiver(&ESerial, PIN_M0, PIN_M1, PIN_AX);

struct SensorData {
  uint8_t food;
  uint32_t tds;
  uint8_t rain;
  uint16_t temperature;
  uint16_t o2;
  uint8_t ph;
};

void setup() {
  Serial.begin(9600);
  ESerial.begin(9600);
  Transceiver.init();
  Transceiver.PrintParameters();
}

void loop() {

  delay(5000);
  
  if (memcmp(prevData, payload, sizeof(payload)) != 0) {
    Serial.println("Starting to call data from sensor unit: ");
    for (int i = 0; i < 4; i++) {
      ESerial.write(payload[i]);
      Serial.print(payload[i], HEX);
    }
    Serial.println(" ");
    memcpy(prevData, payload, sizeof(payload));
  }

  if (ESerial.available() >= 10) {
    for (int i = 0; i < 10; i++) {
      sen_payload[i] = ESerial.read();
    }
    SensorData data;
    unpackSensorData(data);
    printSensorData(data);
  }

  delay(100);
}



// Function to unpack the sen_payload array into the sensor data
void unpackSensorData(SensorData& data) {
  // Unpack food (2 bits)
  data.food = (sen_payload[1] >> 6) & 0x03;

  // Unpack tds (17 bits)
  data.tds = ((sen_payload[1] & 0x3F) << 16) | (sen_payload[2] << 8) | sen_payload[3];

  // Unpack rain (8 bits)
  data.rain = sen_payload[4];

  // Unpack temperature (15 bits)
  data.temperature = (sen_payload[5] << 8) | sen_payload[6];

  // Unpack o2 (12 bits)
  data.o2 = ((sen_payload[7] << 4) & 0xFF0) | ((sen_payload[8] >> 4) & 0x0F);

  // Unpack ph (4 bits)
  data.ph = sen_payload[8] & 0x0F;
}

void printSensorData(const SensorData& data) {
  Serial.println("############## Print Sensor data start ##############");
  Serial.print("Food: ");
  Serial.println(data.food);
  Serial.print("TDS: ");
  Serial.println((float)data.tds / 100);
  Serial.print("Rain: ");
  Serial.println(data.rain);
  Serial.print("Temperature: ");
  Serial.println((float)data.temperature / 100);
  Serial.print("O2: ");
  Serial.println((float)data.o2 / 100);
  Serial.print("pH: ");
  Serial.println(data.ph);
  Serial.println("############## Print Sensor data end  ##############");
}


