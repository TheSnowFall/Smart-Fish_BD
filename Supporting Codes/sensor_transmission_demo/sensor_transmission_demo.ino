#include <Arduino.h>


HardwareSerial LoRa(USART2);
// Define the structure to hold the sensor data
struct SensorData {
  uint8_t food;
  uint32_t tds;  // Changed to uint32_t to accommodate the full range
  uint8_t rain;
  uint32_t temperature;  // Changed to uint32_t to accommodate the full range
  uint16_t o2;           // Changed to uint16_t to accommodate the full range
  uint8_t ph;
};

// Global variable for the payload
byte sen_payload[10];  // Total size of sen_payload according to specification

SensorData prev_sensorData;  // Previous sensor data for comparison
SensorData sensorData;
// Function to pack the sensor data into the sen_payload array


void setup() {
  Serial.begin(115200);
 
  LoRa.begin(9600);

  Serial.println("########  Starting Sensor Unit Demo  ##########");
}

void loop() {

  // temperature();
  // dissolve_o2_sen();
  // ph_count();
  // tds_count();
  // rain();
  // food_lvl_detection();
  processEbytLoRaSerial();
  sensorData.food = 3;
  sensorData.tds= 27233;
  sensorData.rain= 1;
  sensorData.ph=7;
  sensorData.temperature=3252;
  sensorData.o2= 912;


  delay(2000);


  // Compare each value with the previous sensor data
  if (sensorData.food != prev_sensorData.food || sensorData.tds != prev_sensorData.tds || sensorData.rain != prev_sensorData.rain || sensorData.temperature != prev_sensorData.temperature || sensorData.o2 != prev_sensorData.o2 || sensorData.ph != prev_sensorData.ph) {

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

    // Unpack sen_payload into sensor data
    SensorData unpackedData;
    unpackSensorData(unpackedData);

    // Print the unpacked sensor data
    Serial.println("Unpacked Sensor Data:");
    printSensorData(unpackedData);
    //   }
    // }

  } else {
    // If the data is the same, print the message
    // Serial.println("Same sensor data found, no packing/unpacking necessary.");
  }

  
}


