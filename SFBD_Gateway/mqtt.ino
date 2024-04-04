

int8_t get_topic(char topic_name_buffer[], uint8_t topic_type) {



  if (topic_type == 0) {
    strcpy(topic_name_buffer, apNameChar);
    strcat(topic_name_buffer, "/sub");
    return ESP_OK;

  } else if (topic_type == 1) {
    strcpy(topic_name_buffer, apNameChar);
    strcat(topic_name_buffer, "/pub");
    return ESP_OK;

  } else {
  }




  return ESP_FAIL;
}


void topic_ready(){
  if (get_topic(subscribe_topic, 0) == ESP_OK) {
      SerialGeneric.print("Subscribe topic created:");
      SerialGeneric.println(subscribe_topic);
    }
    else {
      SerialGeneric.println("Subscribe topic could not be created!");
    }

    if (get_topic(publish_topic, 1) == ESP_OK) {
      SerialGeneric.print("Publish topic created:");
      SerialGeneric.println(publish_topic);
    }
    else {
      SerialGeneric.println("Publish topic could not be created!");
    }
}



void mqttCallback(char *topic, byte *payload, unsigned int len) {
  SerialGeneric.print("Message arrived by W [");
  SerialGeneric.print(topic);
  SerialGeneric.print("]: ");
  SerialGeneric.write(payload, len);
  SerialGeneric.println();


  char jsonBuffer[len + 1];
  for (int i = 0; i < len; i++) {
    jsonBuffer[i] = payload[i];
  }
  jsonBuffer[len] = '\0'; // Null-terminate the buffer

  // Serial.println("Payload:");
  // Serial.println(jsonBuffer);

  // Parse JSON
  DynamicJsonDocument doc(512); // Adjust the size as per your JSON message
  DeserializationError error = deserializeJson(doc, jsonBuffer);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Access the values from JSON
  const char* addr = doc["addr"];
  const char* type = doc["type"];
  const char* relay = doc["relay"];

  byte byteValue = strtol(addr, NULL, 0);
  unsigned long longValue = strtoul(relay, NULL, 2);
  byte byteValuebin = (byte)longValue;



Serial.print("Byte value of ADDR: ");
Serial.println(byteValue, HEX);
Serial.print("Byte value of message: ");
Serial.println(byteValuebin, BIN);
Serial.print("Byte value of suffix: ");
if (type == "sw") { 
  Serial.print(51, HEX);
 }
 

 


  // Only proceed if incoming message's topic matches
  if (String(topic) == topicLed) {
    ledStatus = !ledStatus;
    digitalWrite(LED_GPIO, ledStatus);
    // mqtt.publish(topicLedStatus, ledStatus ? "1" : "0");
  }
}



boolean mqttConnect() {
  mqtt_gprs.disconnect();
  SerialGeneric.print("Connecting to ");
  SerialGeneric.print(broker);

  // Connect to MQTT Broker
  // boolean status = mqtt.connect("GsmClientTest");

  // // Or, if you want to authenticate MQTT:
  boolean status = mqtt.connect("GsmClientName", "rayan", "rayan");

  if (status == false) {
    SerialGeneric.println(" fail");
    return false;
  }
  SerialGeneric.println(" success");

  mqtt.subscribe(topicLed);
  return mqtt.connected();
}

// #################################   GPRS MQTT ###########################

void mqttCallback_gprs(char *topic, byte *payload, unsigned int len) {

  SerialGeneric.print("Message arrived [");
  SerialGeneric.print(topic);
  SerialGeneric.print("]: ");
  SerialGeneric.write(payload, len);
  SerialGeneric.println();

   char jsonBuffer[len + 1];
  for (int i = 0; i < len; i++) {
    jsonBuffer[i] = payload[i];
  }
  jsonBuffer[len] = '\0'; // Null-terminate the buffer

  // Serial.println("Payload:");
  // Serial.println(jsonBuffer);

  // Parse JSON
  DynamicJsonDocument doc(512); // Adjust the size as per your JSON message
  DeserializationError error = deserializeJson(doc, jsonBuffer);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Access the values from JSON
  const char* addr = doc["addr"];
  const char* type = doc["type"];
  const char* relay = doc["relay"];

  byte byteValue = strtol(addr, NULL, 0);
  Serial.print("Byte value: ");
  Serial.println(byteValue, HEX);

  Serial.print("Address: ");
  Serial.println(addr);
  Serial.print("Type: ");
  Serial.println(type);
  Serial.print("Relay: ");
  Serial.println(relay);


  // Only proceed if incoming message's topic matches
  if (String(topic) == topicLed) {
    ledStatus = !ledStatus;
    digitalWrite(LED_GPIO, ledStatus);
    // mqtt.publish(topicLedStatus, ledStatus ? "1" : "0");
  }
}



boolean mqttConnect_gprs() {
 
  SerialGeneric.print("Connecting to ");
  SerialGeneric.print(broker);

  // Connect to MQTT Broker
  // boolean status = mqtt_gprs.connect("GsmClientTest");

  // // Or, if you want to authenticate MQTT:
  boolean status = mqtt_gprs.connect("GsmClientName", "rayan", "rayan");

  if (status == false) {
    SerialGeneric.println(" fail");
    return false;
  }
  SerialGeneric.println(" success");

  mqtt_gprs.subscribe(topicLed);
  return mqtt_gprs.connected();
}


