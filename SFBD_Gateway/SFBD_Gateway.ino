#define TINY_GSM_MODEM_SIM800
#define SIM800L_IP5306_VERSION_20190610


#include <Arduino.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <TinyGsmClient.h>
#include "ArduinoNvs.h"
#include "OneButton.h"
#include <ESP32Ping.h>

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
int timeout = 120;
String apn_id;
int i;

// ############################  Inserting variable regarding MQTT  ############################

char *broker = "68.183.231.35";
char *topicLed = "tushar/mqtt_test";
char *topicLedStatus = "tushar/go";
char subscribe_topic[13] = {
  0,
};
char publish_topic[13] = {
  0,
};

// ############################  Inserting variable regarding MQTT  ############################
const char *remote_host = "www.google.com";
// ############################  Inserting variable regarding GPRS  ###########################
char apn[15] = {
  0,
};
const char gprsUser[] = "";
const char gprsPass[] = "";

// ############################  Inserting variable regarding GPRS  ###########################


int ledStatus = LOW;
uint32_t lastReconnectAttempt = 0;

int customFieldLength = 40;


// ############################  WiFi Connect/disconnect timing variables  ###########################
unsigned long previousMillis = 0;
unsigned long interval = 10000;
int retryCount = 0;

// ############################  WiFi Connect/disconnect timing variables  ###########################


// ############################  Button variables  ###########################

unsigned long pressStartTime = 0;


bool long_press = false;
bool single_press = false;

// ############################  Button variables  ###########################




OneButton button(TRIGGER_PIN, false);

WiFiClient client;
TinyGsmClient client_gprs(modem);
PubSubClient mqtt(client);
PubSubClient mqtt_gprs(client_gprs);

WiFiManager *wmPtr;


unsigned long startTime;
bool Provision_status = false;

enum wifi_conn_stat {
  NOT_PROVISIONED = 0,
  PROVISIONED
};

void IRAM_ATTR checkTicks() {

  button.tick();
}




void setup() {
  delay(3000);
  WiFi.mode(WIFI_STA);
  WiFi.begin();
  delay(3000);

  NVS.begin();
  SerialGeneric.begin(115200);
  SerialGeneric.println("\n Starting");
  SerialGeneric.setDebugOutput(true);
  // pinMode(TRIGGER_PIN, INPUT_PULLUP);
  pinMode(LED_GPIO, OUTPUT);
  set_apn();
  generateAPName();

  bool apnIsEmpty = true;
  for (int i = 0; i < sizeof(apn); i++) {
    if ((i == 0 || i == 1) && apn[i] != 0) {
      apnIsEmpty = false;
      break;
    }
  }

  if (WiFi.SSID() == "" && apnIsEmpty == true){

    Provision_status = NOT_PROVISIONED;
    Serial.println("Not provisioned, Hit the button once please");
  }
  else{
    Provision_status = PROVISIONED;
    Serial.println("Provisioned, Sit tight");
  }


   if (Provision_status == PROVISIONED ){

    
  if (get_topic(subscribe_topic, 0) == ESP_OK) {
    SerialGeneric.print("Subscribe topic created:");
    SerialGeneric.println(subscribe_topic);
  } else {
    SerialGeneric.println("Subscribe topic could not be created!");
  }

  if (get_topic(publish_topic, 1) == ESP_OK) {
    SerialGeneric.print("Publish topic created:");
    SerialGeneric.println(publish_topic);
  } else {
    SerialGeneric.println("Publish topic could not be created!");
  }



  

  //#########  GSM start  #########

  getting_sim_ready();

  mqtt.setServer(broker, 1883);
  mqtt.setCallback(mqttCallback);

  mqtt_gprs.setServer(broker, 1883);
  mqtt_gprs.setCallback(mqttCallback_gprs);
  // mqtt_gprs.setCallback(mqttCallback);
  }

  attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), checkTicks, CHANGE);
  button.attachClick(singleClick);
  button.attachDoubleClick(doubleClick);




  // button.setPressMs(1000);
  // button.attachLongPressStart(pressStart);
  // button.attachDuringLongPress(duringPress);
}
void loop() {

  button.tick();




  // wifi_manager
  if (single_press == true || Provision_status == NOT_PROVISIONED ) {
    startTime = millis();

    WiFiManager wm;

    wmPtr = &wm;

    if (long_press == true) {
      SerialGeneric.println("Erasing Config, restarting");
      wm.resetSettings();  // Reset WiFiManager settings
      ESP.restart();       // Restart ESP32
    }


    const char *mobile_operator = "<br/><label for='customfieldid'><b>Select your SIM operator:</b></label><br><input type='radio' name='customfieldid' value='1' checked> Grameenphone<br><input type='radio' name='customfieldid' value='2'> Robi<br><input type='radio' name='customfieldid' value='3'> Banglalink<br><input type='radio' name='customfieldid' value='4'> Teletalk<br><input type='radio' name='customfieldid' value='5'> Airtel";
    WiFiManagerParameter custom_field(mobile_operator);  // Declaring WiFiManagerParameter object
    wm.addParameter(&custom_field);

    // if (wm.server->hasArg("customfieldid")) {
    // value = wm.server->arg("customfieldid");
    // }
    wm.setSaveParamsCallback(saveParamCallback);
    const char *menu[] = { "param", "wifi", "info", "sep", "restart", "exit" };
    wm.setMenu(menu, 6);
    wm.setClass("invert");
    wm.setConfigPortalTimeout(timeout);

    bool res;
    generateAPName();
    // SerialGeneric.println("apNameChar: ");SerialGeneric.println(apNameChar);
    res = wm.autoConnect(apNameChar);  // anonymous ap

    if (!res) {
      SerialGeneric.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }



   
    SerialGeneric.println("connected...yeey :)");
    
    single_press = false;
    esp_restart();
  }



if (Provision_status == PROVISIONED){

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
else{

}




gprs_mqtt:


  if (gsm_task_flag == true && wifi_fail_flag == true) {

    if (!printed_gprs_tag) {
      SerialGeneric.println("########### MQTT IN GPRS #############");
      printed_gprs_tag = true;
    }

    if (!mqtt_gprs.connected()) {
      SerialGeneric.println("=== MQTT NOT CONNECTED G===");
      // Reconnect every 10 seconds
      uint32_t t = millis();
      if (t - lastReconnectAttempt > 10000L) {
        lastReconnectAttempt = t;
        if (mqttConnect_gprs()) {
          lastReconnectAttempt = 0;
        }
      }
      delay(100);
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
      // Reconnect every 10 seconds
      uint32_t t = millis();
      if (t - lastReconnectAttempt > 10000L) {
        lastReconnectAttempt = t;
        if (mqttConnect()) {
          lastReconnectAttempt = 0;
        }
      }
      delay(100);
      return;
    }

    mqtt.loop();
  }
}
