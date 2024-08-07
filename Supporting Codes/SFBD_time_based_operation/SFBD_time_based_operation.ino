#define PUMP LED_BUILTIN
#define VALVE PB1

#include <STM32RTC.h>

#ifndef ONESECOND_IRQn
#error "RTC has no feature for One-Second interrupt"
#endif

/* Get the rtc object */
STM32RTC& rtc = STM32RTC::getInstance();

static STM32RTC::Hour_Format hourFormat = STM32RTC::HOUR_24;

unsigned long startTime = 0;
const int sleepPeriod = 30; // 300 seconds
const int pumpPeriod = 30;   // 120 seconds
const int sensingPeriod = 20;  // 120 seconds
const int valvePeriod = 20;    // 60 seconds


bool pump_stat_flag = false;
bool valve_stat_flag = false;






void rtc_SecondsCB(void *data) {
  UNUSED(data);
}







void setup() {
  Serial.begin(115200);
  rtc.begin(hourFormat); // Initialize RTC in 24H format
  rtc.setSeconds(0);

  startTime = rtc.getEpoch(); // Store the start time

  Serial.println("System initialized.");
  rtc.attachSecondsInterrupt(rtc_SecondsCB);

  pinMode(PUMP, OUTPUT);
  pinMode(VALVE, OUTPUT);
  digitalWrite(PUMP, HIGH);

}

void loop() {
  unsigned long currentTime = rtc.getEpoch();
  unsigned long elapsedTime = currentTime - startTime;


  if (elapsedTime < sleepPeriod) {
    // First phase: Waiting to reach initial period
    // Serial.println("Device is Sleeping");
    Serial.printf("Device is Sleeping: %lu seconds remaining\n", sleepPeriod -elapsedTime);

  } else if (elapsedTime >= sleepPeriod && elapsedTime < sleepPeriod + pumpPeriod){
    
     Serial.printf("Pump Switching: %lu seconds remaining\n", sleepPeriod + pumpPeriod - elapsedTime);
    if(pump_stat_flag ==false){ 

      Serial.println("Pump on!");
      digitalWrite(PUMP, LOW);
      pump_stat_flag = true;
      }
  
  }
  else if (elapsedTime >= sleepPeriod && elapsedTime < sleepPeriod + pumpPeriod + sensingPeriod) {
    // Second phase: Sensors are initializing
   Serial.printf("Sensor data: %lu seconds remaining\n", sleepPeriod + pumpPeriod + sensingPeriod - elapsedTime);
      if(pump_stat_flag == true){ 

      digitalWrite(PUMP, HIGH);
      Serial.println("Pump off!");
      pump_stat_flag = false;
      }
     
      Serial.println("PH sensor started");
      Serial.println("DO sensor started");
      Serial.println("Temp sensor started");
      Serial.println("Rain sensor started");
    
   
  } else if (elapsedTime >= sleepPeriod + sensingPeriod && elapsedTime < sleepPeriod + pumpPeriod + sensingPeriod + valvePeriod) {
    // Third phase: Valve operation
    Serial.printf("Sensor data: %lu seconds remaining\n", sleepPeriod + pumpPeriod + sensingPeriod + valvePeriod - elapsedTime);
      if(valve_stat_flag == false){ 

      Serial.println("Valve open");
      digitalWrite(VALVE, HIGH);
      valve_stat_flag = true;
      }


      
    }
    
   else {
    // Reset time after completing all phases
    Serial.println("Cycle complete. Resetting...");
    digitalWrite(PUMP, HIGH);
    digitalWrite(VALVE, LOW);
    valve_stat_flag = false;
    startTime = rtc.getEpoch(); // Reset the start time
  }

  delay(1000);
}

/* Callback function on each second interrupt */

