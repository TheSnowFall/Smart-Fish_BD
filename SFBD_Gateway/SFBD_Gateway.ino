#include <WiFiManager.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include "ArduinoNvs.h"

#define SIM800L_IP5306_VERSION_20190610

#include "utilities.h"



char apNameChar[32];
int timeout = 120;
String apn_id;

// ############################  Inserting variable regarding MQTT  ############################

char *broker = "68.183.231.35";
char *topicLed = "tushar/mqtt_test";
char *topicLedStatus = "tushar/go";
char subscribe_topic[13] = {0,};
char publish_topic[13] = { 0,};

// ############################  Inserting variable regarding MQTT  ############################

// ############################  Inserting variable regarding GPRS  ###########################
char apn[15]={0,};


// ############################  Inserting variable regarding GPRS  ###########################


int ledStatus = LOW;
uint32_t lastReconnectAttempt = 0;

int customFieldLength = 40;





WiFiClient client;
PubSubClient mqtt(client);

WiFiManager *wmPtr;








void setup() {
  WiFi.mode(WIFI_STA);
  NVS.begin();
  Serial.begin(115200);
  Serial.println("\n Starting");
  Serial.setDebugOutput(true);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  pinMode(LED_GPIO, OUTPUT);
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(mqttCallback);

  generateAPName();
  if (get_topic(subscribe_topic, 0) == ESP_OK) {
    Serial.print("Subscribe topic created:");
    Serial.println(subscribe_topic);
  } else {
    Serial.println("Subscribe topic could not be created!");
  }

  if (get_topic(publish_topic, 1) == ESP_OK) {
    Serial.print("Publish topic created:");
    Serial.println(publish_topic);
  } else {
    Serial.println("Publish topic could not be created!");
  }

  set_apn();



}





void loop() {
  // is configuration portal requested?
  if (digitalRead(TRIGGER_PIN) == HIGH) {
    WiFiManager wm;

    wmPtr = &wm;

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
    // Serial.println("apNameChar: ");Serial.println(apNameChar);
    res = wm.autoConnect(apNameChar);  // anonymous ap

    if (!res) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }

    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");

    if (WiFi.localIP() != IPAddress(0, 0, 0, 0)) {

      if (!mqtt.connected()) {
        Serial.println("=== MQTT NOT CONNECTED ===");
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

  // put your main code here, to run repeatedly:
}




void generateAPName() {
  uint8_t mac[6];
  char macStr[18] = { 0 };

  // Get MAC address
  WiFi.macAddress(mac);

  // Convert MAC address to string
  sprintf(macStr, "%02X%02X", mac[4], mac[5]);

  // Generate AP name
  sprintf(apNameChar, "SFBD_%s", macStr);
}


String getParam(String name) {
  // Read parameter from server, for custom HTML input
  String value;
  if (wmPtr->server->hasArg(name)) {
    value = wmPtr->server->arg(name);
  }
  return value;
}

// Function to save parameter callback
void saveParamCallback() {
  Serial.println("[CALLBACK] saveParamCallback fired");
  apn_id = getParam("customfieldid");
  Serial.println("PARAM customfieldid = " + apn_id);
  bool return_erro_check;
  return_erro_check = NVS.setString("apn_id", apn_id);
  if (!return_erro_check) {

    Serial.println("APN ID saving fail");
  } else {
    Serial.println("APN ID saving success");
  }
}




void set_apn(){
  String apn_id = NVS.getString("apn_id");
  String real_apn_name;
  if (apn_id == "1") {
    real_apn_name="GP-INTERNET";
    real_apn_name.toCharArray(apn, 15); 
  } else if (apn_id == "2") {
    real_apn_name="Robi-INTERNET";
    real_apn_name.toCharArray(apn, 15); ;
  }else if (apn_id == "3") {
    real_apn_name="Banglalink-WEB";
    real_apn_name.toCharArray(apn, 15); ;
  }else if (apn_id == "4") {
    real_apn_name="TT-INTERNET";
    real_apn_name.toCharArray(apn, 15); ;
  }else if (apn_id == "5") {
    real_apn_name="Airtel Internet";;
    real_apn_name.toCharArray(apn, 15); ;
  }else{

  }

}
