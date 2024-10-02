#include <Arduino.h>
#include <EEPROM.h>
#include <STM32RTC.h>
#include "DFRobot_PH.h"
#include "GravityTDS.h"

// ################# pin_def #################

#define DO_PIN PA1
#define PH_PIN PA0


#define TDS_PIN PA4
#define TEMP_PIN PA5

#define RAIN_PIN PB0
#define PUMP_PIN PB11


// #define FOOD_EMPTY    PB11
// #define FOOD_MEDIUM   PB10
// #define FOOD_FULL     PB1

// ################# pin_def #################

// ################# temp #################
double temp;
// ################# temp #################

// ################# ph #################
uint8_t phValue;
float voltage;
DFRobot_PH ph;
// ################# ph #################


// ################# TDS #################
float tdsValue = 0;
float sensorValue = 0;
GravityTDS gravityTds;
// ################# TDS #################


// ################# D.O #################


//Single-point calibration Mode=0
//Two-point calibration Mode=1
#define TWO_POINT_CALIBRATION 1


#define CAL1_V  (975)//mv
#define CAL1_T   (34) //℃


#define CAL2_V  (698)//mv
#define CAL2_T   (25) //℃

const uint16_t DO_Table[41] = {
    14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
    11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
    9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
    7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410};

uint8_t Temperaturet;
uint16_t ADC_Raw;
uint16_t ADC_Voltage;
uint16_t DO;

// ################# D.O ################# 


unsigned long idleTime = 10 * 60 * 1000;  // 10 minutes in milliseconds
unsigned long pumpTime = 3 * 60 * 1000;   // 3 minuites in milliseconds
unsigned long sensorRunTime = 8* 60 * 1000; // 8 minuite in milliseconds

unsigned long lastIdleTime = 0;
unsigned long lastPumpTime = 0;
unsigned long sensorStartTime = 0;
bool isIdle = true;
bool pumpOn = false;
bool runningSensors = false;
bool firstTimePull = true;



// ################# Timer #################

// ################# filter #################
const int numReadings = 10; // Adjust this value as needed

int readings[numReadings];   // Array to store the readings
int filter_index = 0;               // Index of the current reading
int filter_total = 0;               // Running total of the readings
int filter_average = 0;             // Smoothed average value

// ################# filter #################


#define VREF 3300     //VREF (mv)
#define ADC_RES 1024  //ADC Resolution

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
  analogReadResolution(10);
  LoRa.begin(9600);

  sensorData.o2 = 0;
  sensorData.ph = 0;
  sensorData.rain = 1;
  sensorData.food = 0;
  sensorData.tds = 0;
  sensorData.temperature=0;


  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  ph.begin();
  gravityTds.setPin(TDS_PIN);

  gravityTds.setAref(3.3);       //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();            //initialization


  pinMode(RAIN_PIN, INPUT);
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);
  // pinMode(FOOD_EMPTY, INPUT);
  // pinMode(FOOD_MEDIUM, INPUT);
  // pinMode(FOOD_FULL, INPUT);

  Serial.println("########  Starting Sensor Unit  ##########");
}

// Global variables for pump timing


void loop() {
  processEbytLoRaSerial();
  unsigned long currentMillis = millis();


  if (firstTimePull) {
      isIdle = false;
      firstTimePull= false;
      pumpOn = true;
      lastPumpTime = currentMillis;
      digitalWrite(PUMP_PIN, HIGH);  // Turn the pump on
    
  }



  // Handle idle mode (idle time)
if (isIdle) {
    if (currentMillis - lastIdleTime >= idleTime) {
      isIdle = false;
      lastPumpTime = currentMillis;
      pumpOn = true;
      digitalWrite(PUMP_PIN, HIGH);  // Turn the pump on
    } else {
      // Remain idle during idle time
      return;
    }
  }





  // Handle pump on time
  if (pumpOn ) {
    if (currentMillis - lastPumpTime >= pumpTime) {
      pumpOn = false;
      digitalWrite(PUMP_PIN, LOW);  // Turn the pump off

      // Start sensor functions for 30 seconds
      runningSensors = true;
      sensorStartTime = currentMillis;
    }
    return;
  }
   if (runningSensors) {
    if (currentMillis - sensorStartTime < sensorRunTime) {
      temperature();
      dissolve_o2_sen();
      ph_count();
      tds_count();
      rain();
    } else {
      runningSensors = false;

      // After sensor functions run, execute the data packing and transmission
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

      // Reset idle time
      isIdle = true;
      lastIdleTime = currentMillis;
    }
    return;
  }
}

