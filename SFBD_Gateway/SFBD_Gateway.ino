#define TINY_GSM_MODEM_SIM800
#define SIM800L_IP5306_VERSION_20190610
#define WDT_TIMEOUT 15000
#define MAX_BUFFER_SIZE 20

#include <Arduino.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <TinyGsmClient.h>
#include "ArduinoNvs.h"
#include "OneButton.h"
#include <ESP32Ping.h>
#include <ArduinoJson.h>
#include "EBYTE.h"
#include "esp_task_wdt.h"






#define GSM_PIN ""

#define SerialGeneric Serial
#define SerialAT Serial1
                                            // #define DUMP_AT_COMMANDS
#define TINY_GSM_DEBUG SerialGeneric

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialGeneric);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

#include "utilities.h"

bool gsm_task_flag = false;
bool wifi_fail_flag = false;
bool printed_gprs_tag = false;
bool printed_wifi_tag = false;
char apNameChar[32];
char apNameCharMqtt[32];
int timeout = 120;
String apn_id;
uint8_t mac[6];
char gd_id[18];

// ############################  Inserting variable regarding MQTT  ############################

char *broker = "68.183.231.35";
// char *topicLed = "tushar/mqtt_test";
// char *topicLedStatus = "tushar/go";
// char subscribe_topic[13] = { 0,};
// char publish_topic[13] = { 0, };
char* subscribe_topic;
char* publish_topic;
const char* relay_response_to_server;
const char* sensor_data_to_server;

// ############################  Inserting variable regarding MQTT  ############################
const char *remote_host = "www.google.com";

// ############################  Inserting variable regarding GPRS  ###########################
char apn[15] = {0,};
bool apnIsEmpty = true;
const char gprsUser[] = "";
const char gprsPass[] = "";

// ############################  Inserting variable regarding GPRS  ###########################


int ledStatus = LOW;
uint32_t lastReconnectAttempt = 0;

int customFieldLength = 40;

// ############################  WiFi Connect/disconnect timing variables  ###########################
unsigned long previousMillis = 0;
int retryCount = 0;
// ############################  WiFi Connect/disconnect timing variables  ###########################

// ############################  Button variables  ###########################
unsigned long pressStartTime = 0;
bool reset_setting = false;
bool single_press = false;
// ############################  Button variables  ###########################
// ######## Radio Transmission data ######## 

byte payload_from_mqtt[4]   = {0,};
byte received_response[10]  = {0,};
byte received_sen_data[10]  = {0,};
byte received_relay_msg[4]  = {0,};

int relay_msg_index = 0; // Index to keep track of received bytes for payload[4]
int sen_data_index = 0; // Index to keep track of received bytes for sen_payload[10]

uint8_t payload_index = 0;
int channel_ebyte;
 
// ######## Radio Transmission data ########
byte mqtt_status_publish_done = false;
byte mqtt_status_gprs_publish_done = false ;
bool status_update = false;
//  ######### JSON Variable ############

uint8_t endbyte_sen = 0x22;
uint8_t endbyte_sw = 0x33;    
uint8_t sw_addr= 0x44 ;
uint8_t sen_addr= 0x26 ;
bool payload_received = false;

//  ######### JSON Variable ############

OneButton button(TRIGGER_PIN, true);

/* 
Change  the harsware serial 2 pins in BSP level of ESP32.
TX2 >>19  RX2 >>18 is used here.
*/

HardwareSerial ESerial(2);   
EBYTE Transceiver(&ESerial, PIN_M0, PIN_M1, PIN_AX);

WiFiClient client;
PubSubClient mqtt(client);

TinyGsmClient client_gprs(modem);
PubSubClient mqtt_gprs(client_gprs);

WiFiManager *wmPtr;

unsigned long startTime;
uint8_t Provision_status = 0;

enum wifi_conn_stat {
  NO_PROVITION_STAT = 0,
  NOT_PROVISIONED,
  PROVISIONED,
  GPRS_PROVISIONED,
  WIFI_PROVISIONED
};

// ############## Received sensor data variable ################

struct SensorData {
  uint8_t food;
  uint32_t tds;  // Changed to uint32_t to accommodate the full range
  uint8_t rain;
  uint32_t temp;  // Changed to uint32_t to accommodate the full range
  uint16_t o2;    // Changed to uint16_t to accommodate the full range
  uint8_t ph;
};

// ############## Received sensor data variable ################
void IRAM_ATTR checkTicks() {
  button.tick();
}

void setup() {
  subscribe_topic = (char*)malloc(20 * sizeof(char));
  publish_topic = (char*)malloc(20 * sizeof(char));

  delay(3000);
  WiFi.mode(WIFI_STA);
  WiFi.begin();
  delay(3000);

  NVS.begin();
  SerialGeneric.begin(115200);
  ESerial.begin(9600);
  SerialGeneric.println(F("\n#############      Starting Gateway, Smart Fish BD      #############"));
  SerialGeneric.println(F("#############      Developed By:Erfan Mahmud Tushar     #############"));
  SerialGeneric.println(F("#############      Firmware Version : 1.1.0             #############\n"));
  SerialGeneric.setDebugOutput(true);
  pinMode(LED_GPIO, OUTPUT);

  pinMode(PIN_M0, OUTPUT);
  pinMode(PIN_M1, OUTPUT);

  digitalWrite(PIN_M0, LOW);
  digitalWrite(PIN_M1, LOW);
  
  Transceiver.init();
  delay(2000);
  // Transceiver.PrintParameters();


  esp_task_wdt_init(WDT_TIMEOUT, true); // Enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);

  attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), checkTicks, CHANGE);
  button.attachClick(singleClick);
  // button.attachDoubleClick(doubleClick);
  
  button.attachDuringLongPress(duringPress);
  



  set_apn();
  generateAPName();



  if (WiFi.SSID() == "" && apnIsEmpty == true) {

    Provision_status = NOT_PROVISIONED;
    Serial.println("Not provisioned, Hit the button once please");
  } else if (WiFi.SSID() == "" && apnIsEmpty == false) {

    Provision_status = GPRS_PROVISIONED;
    Serial.println("Provisioned but no WiFi is provisioned");

  } else if (WiFi.SSID() != "" && apnIsEmpty == true) {
   
    Provision_status = WIFI_PROVISIONED;
    Serial.println("Provisioned but no GPRS is provisioned");
  } else {
    Provision_status = PROVISIONED;
    Serial.println("Everything Provisioned,sit tight");
  }


  if (Provision_status == PROVISIONED) {
    Serial.println("I am inside fully provisioned");
    topic_ready();
    

    getting_sim_ready();

    mqtt.setServer(broker, 1883);
    mqtt.setCallback(mqttCallback);

    mqtt_gprs.setServer(broker, 1883);
    mqtt_gprs.setCallback(mqttCallback_gprs);
  }

  else if (Provision_status == WIFI_PROVISIONED) {
    Serial.println("I am inside only WiFi provisioned");
    topic_ready();
    mqtt.setServer(broker, 1883);
    mqtt.setCallback(mqttCallback);
  }

  else if (Provision_status == GPRS_PROVISIONED) {
    Serial.println("I am inside only GPRS provisioned");
    topic_ready();
    getting_sim_ready();
    mqtt_gprs.setServer(broker, 1883);
    mqtt_gprs.setCallback(mqttCallback_gprs);
  }

  else{
    // ??
  }

 blinkLED(2,100);
}

void loop() {
  button.tick();

  if (reset_setting == true) {
    single_press = true;
  }


//   if (ESerial.available() > 1) {
//   Serial.print("Received Data:");
//   for (int i = 0; i < 4; i++) {
//     received_response[i] = ESerial.read();
//     if (i == 1 || i == 2) {
//       Serial.print(" | 0b");
//       printBinary(received_response[i]);
//     } else {
//       Serial.print(" | 0x");
//       Serial.print(received_response[i], HEX);
//     }
//   }
//   Serial.println(" ");
// } 


 processEbyteSerial();


  // wifi_manager
  if (single_press == true || Provision_status == NOT_PROVISIONED) {
    startTime = millis();
    WiFiManager wm;
    wmPtr = &wm;

    if (reset_setting == true) {
      SerialGeneric.println("Erasing Config, restarting");
      erase_apn() ;         //Erase APN
      wm.resetSettings();  // Reset WiFiManager settings
      ESP.restart();       // Restart ESP32
    }

    const char *mobile_operator = "<br/><label for='customfieldid'><b>Select your SIM operator:</b></label><br><input type='radio' name='customfieldid' value='1' checked> Grameenphone<br><input type='radio' name='customfieldid' value='2'> Robi<br><input type='radio' name='customfieldid' value='3'> Banglalink<br><input type='radio' name='customfieldid' value='4'> Teletalk<br><input type='radio' name='customfieldid' value='5'> Airtel";
    WiFiManagerParameter custom_field(mobile_operator);
    wm.addParameter(&custom_field);
    wm.setSaveParamsCallback(saveParamCallback);
    const char *menu[] = { "param", "wifi", "info", "sep", "restart", "exit" };
    wm.setMenu(menu, 6);
    wm.setClass("invert");
    wm.setConfigPortalTimeout(timeout);
    bool res;
    generateAPName();
    res = wm.autoConnect(apNameChar);

    if (!res) {
      SerialGeneric.println("failed to connect and hit timeout");
      delayPassed(3000);
      ESP.restart();
      delayPassed(5000);
    }
    SerialGeneric.println("connected...yeey :)");
    single_press = false;
    esp_restart();
  }

  if (Provision_status == PROVISIONED) {
    if ((WiFi.status() != WL_CONNECTED) || (Ping.ping(remote_host) == false)) {
      wifi_fail_flag = true;
      gsm_task_flag = true;
      goto gprs_mqtt;
    } else {
      wifi_fail_flag = false;
      gsm_task_flag = false;
      goto wifi_mqtt;
    }
  }

  else if (Provision_status == WIFI_PROVISIONED) {
    wifi_fail_flag = false;
    gsm_task_flag = false;
    mqtt_gprs.disconnect();
    goto wifi_mqtt;
  } else if (Provision_status == GPRS_PROVISIONED) {
    wifi_fail_flag = true;
    gsm_task_flag = true;
    mqtt.disconnect();
    goto gprs_mqtt;
  }

  else{
    Serial.println("Not provisioned yet!");
  }

gprs_mqtt:
  if (gsm_task_flag == true && wifi_fail_flag == true) {\
   
    if (!printed_gprs_tag) {
      SerialGeneric.println("########### MQTT IN GPRS #############");
      printed_gprs_tag = true;
    }
    if (!mqtt_gprs.connected()) {
      SerialGeneric.println("=== MQTT NOT CONNECTED G===");
      uint32_t t = millis();
      if (t - lastReconnectAttempt > 10000L) {
        lastReconnectAttempt = t;
        if (mqttConnect_gprs()) {
          lastReconnectAttempt = 0;
        }
      }
      delayPassed(100);
      return;
    }
    mqtt_gprs.loop();
  }

wifi_mqtt:
  if (gsm_task_flag == false && wifi_fail_flag == false) {
      
    if (!printed_wifi_tag) {
      SerialGeneric.println("########### MQTT IN WiFi #############");
      printed_wifi_tag = true;
    }
    if (!mqtt.connected()) {
      SerialGeneric.println("=== MQTT NOT CONNECTED W===");
      uint32_t t = millis();
      if (t - lastReconnectAttempt > 10000L) {
        lastReconnectAttempt = t;
        if (mqttConnect()) {
          lastReconnectAttempt = 0;
        }
      }
      delayPassed(100);
      return;
    }
    mqtt.loop();
  }
}
