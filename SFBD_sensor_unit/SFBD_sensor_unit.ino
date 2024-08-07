#include <Arduino.h>
#include <EEPROM.h>
#include <STM32RTC.h>
#include "DFRobot_PH.h"
#include "GravityTDS.h"

// ################# pin_def #################

#define DO_PIN        PA0
#define TEMP_PIN      PA5
#define PH_PIN        PA1
#define TDS_PIN       PA4
#define RAIN_PIN      PB0

#define FOOD_EMPTY    PB11
#define FOOD_MEDIUM   PB10
#define FOOD_FULL     PB1

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
#define TWO_POINT_CALIBRATION 0


//Single point calibration needs to be filled CAL1_V and CAL1_T
#define CAL1_V (1762)  //mv
#define CAL1_T (31)    //℃
//Two-point calibration needs to be filled CAL2_V and CAL2_T
//CAL1 High temperature point, CAL2 Low temperature point
#define CAL2_V (1300)  //mv
#define CAL2_T (15)    //℃




const uint16_t DO_Table[41] = {
  14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
  11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
  9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
  7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410
};

uint8_t Temperaturet;
uint16_t ADC_Raw;
uint16_t ADC_Voltage;
uint16_t DO;

// ################# D.O #################


// ################# Timer  #################


/* Get the rtc object */
STM32RTC& rtc = STM32RTC::getInstance();

static STM32RTC::Hour_Format hourFormat = STM32RTC::HOUR_24;


unsigned long startTime = 0;
const unsigned long sleepPeriod = 300; // 300 seconds
const unsigned long sensorPeriod = 120;  // 120 seconds
const unsigned long valvePeriod = 60;    // 60 seconds



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
 //##############################  Timer ##################################

timer_setup();


//##############################  Timer ##################################

  ph.begin();
  gravityTds.setPin(TDS_PIN);

  gravityTds.setAref(3.3);       //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();            //initialization


  pinMode(RAIN_PIN, INPUT);
  pinMode(FOOD_EMPTY, INPUT);
  pinMode(FOOD_MEDIUM, INPUT);
  pinMode(FOOD_FULL, INPUT);

  Serial.println("########  Starting Sensor Unit  ##########");
}

void loop() {
  processEbytLoRaSerial();
  temperature();
  dissolve_o2_sen();
  ph_count();
  tds_count();
  rain();
  food_lvl_detection();
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
    Serial.println("Same sensor data found, no packing/unpacking necessary.");
  }
}
