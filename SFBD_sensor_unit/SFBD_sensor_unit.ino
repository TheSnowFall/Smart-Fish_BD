#include <Arduino.h>


HardwareSerial LoRa(USART2);
// Define the structure to hold the sensor data
struct SensorData {
  uint8_t food;
  uint32_t tds;  // Changed to uint32_t to accommodate the full range
  uint8_t rain;
  uint32_t temp;  // Changed to uint32_t to accommodate the full range
  uint16_t o2;    // Changed to uint16_t to accommodate the full range
  uint8_t ph;
};

// Global variable for the payload
byte sen_payload[10];  // Total size of sen_payload according to specification

SensorData prev_sensorData;  // Previous sensor data for comparison

// Function to pack the sensor data into the sen_payload array
void packSensorData(const SensorData& data) {
  sen_payload[0] = 0x26;  // Address
  sen_payload[9] = 0x22;  // Last element

  // Pack food (2 bits)
  sen_payload[1] = (data.food << 6) & 0xC0;

  // Pack tds (17 bits)
  sen_payload[1] |= (data.tds >> 16) & 0x3F;
  sen_payload[2] = (data.tds >> 8) & 0xFF;
  sen_payload[3] = data.tds & 0xFF;

  // Pack rain (8 bits)
  sen_payload[4] = data.rain;

  // Pack temp (15 bits)
  sen_payload[5] = (data.temp >> 8) & 0xFF;
  sen_payload[6] = data.temp & 0xFF;

  // Pack o2 (12 bits)
  sen_payload[7] = (data.o2 >> 4) & 0xFF;
  sen_payload[8] = ((data.o2 & 0x0F) << 4) & 0xF0;

  // Pack ph (4 bits)
  sen_payload[8] |= (data.ph & 0x0F);
}

// Function to unpack the sen_payload array into the sensor data
void unpackSensorData(SensorData& data) {
  // Unpack food (2 bits)
  data.food = (sen_payload[1] >> 6) & 0x03;

  // Unpack tds (17 bits)
  data.tds = ((sen_payload[1] & 0x3F) << 16) | (sen_payload[2] << 8) | sen_payload[3];

  // Unpack rain (8 bits)
  data.rain = sen_payload[4];

  // Unpack temp (15 bits)
  data.temp = (sen_payload[5] << 8) | sen_payload[6];

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
  Serial.print("Temp: ");
  Serial.println((float)data.temp / 100);
  Serial.print("O2: ");
  Serial.println((float)data.o2 / 100);
  Serial.print("pH: ");
  Serial.println(data.ph);
  Serial.println("############## Print Sensor data end  ##############");
 
}

void setup() {
  Serial.begin(9600);
  LoRa.begin(9600);
  Serial.println("Enter sensor data in the format: food tds rain temp o2 ph");
}

void loop() {

  if (Serial.available() > 0) {
    SensorData sensorData;
    sensorData.food = Serial.parseInt();
    sensorData.tds = Serial.parseFloat() * 100;
    sensorData.rain = Serial.parseInt();
    sensorData.temp = Serial.parseFloat() * 100;
    sensorData.o2 = Serial.parseFloat() * 100;
    sensorData.ph = Serial.parseInt();
    if (sensorData.food != 0 && sensorData.tds != 0 && sensorData.rain != 0 && sensorData.temp != 0 && sensorData.o2 != 0 && sensorData.ph != 0 ) {
      Serial.println("Your inputs are:");

      // Compare each value with the previous sensor data
      if (sensorData.food != prev_sensorData.food || sensorData.tds != prev_sensorData.tds || sensorData.rain != prev_sensorData.rain || sensorData.temp != prev_sensorData.temp || sensorData.o2 != prev_sensorData.o2 || sensorData.ph != prev_sensorData.ph) {

        // Print the new sensor data
        // printSensorData(sensorData);

        // Pack sensor data into sen_payload
        packSensorData(sensorData);

        // Print the packed payload
        Serial.println("Packed Payload:");
        for (int i = 0; i < 10; i++) {
          LoRa.write(sen_payload[i]);
          Serial.print(sen_payload[i], HEX);  // Print in hexadecimal format
          Serial.print(" ");
        }
        Serial.println();

        // Update previous sensor data for comparison
        prev_sensorData = sensorData;
      } else {
        // If the data is the same, print the message
        Serial.println("Same sensor data found, no packing/unpacking necessary.");
      }

      // Unpack sen_payload into sensor data
      SensorData unpackedData;
      unpackSensorData(unpackedData);

      // Print the unpacked sensor data
      Serial.println("Unpacked Sensor Data:");
      printSensorData(unpackedData);
    }
  }
}
