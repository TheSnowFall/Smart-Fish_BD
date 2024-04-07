void topic_ready() {
  if (get_topic(subscribe_topic, 0) == ESP_OK) {
    Serial.print("Subscribe topic created: ");
    Serial.println(subscribe_topic);
  } else {
    Serial.println("Subscribe topic could not be created!");
  }

  if (get_topic(publish_topic, 1) == ESP_OK) {
    Serial.print("Publish topic created: ");
    Serial.println(publish_topic);
  } else {
    Serial.println("Publish topic could not be created!");
  }
}

int8_t get_topic(char *topic_name_buffer, uint8_t topic_type) {
  if (topic_type == 0) {
    strcpy(topic_name_buffer, apNameChar);
    strcat(topic_name_buffer, "/sub");
    return ESP_OK;
  } else if (topic_type == 1) {
    strcpy(topic_name_buffer, apNameChar);
    strcat(topic_name_buffer, "/pub");
    return ESP_OK;
  } else {
    return ESP_FAIL;
  }
}




// ######################################################################


void mqttCallback(char *topic, byte *payload, unsigned int len) {
  SerialGeneric.print("Message arrived by W [");
  SerialGeneric.print(topic);
  SerialGeneric.print("]: ");
  SerialGeneric.write(payload, len);
  SerialGeneric.println();
  //LED blink
  blinkLED(5, 200);
  //LED blink
  if (processJsonPayload((const char *)payload) == ESP_OK) {

    //  enter code for radio frequency change
    Serial.print("Sending data :");
    for (int i = 0; i < 4; i++) {
      ESerial.write(payload_from_mqtt[i]);
      if (i == 1 || i == 2) {
        Serial.print(" | 0b");
        printBinary(payload_from_mqtt[i]);
      } else {
        Serial.print(" | 0x");
        Serial.print(payload_from_mqtt[i], HEX);
      }
    }
    Serial.println(" ");
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
  //LED blink
  blinkLED(2, 500);
  //LED blink
  char *publishing_update_on_boot = "{\"update\":1}";
  // Serial.println("Publishing topic");
  mqtt.publish(publish_topic, publishing_update_on_boot);
  // mqtt.subscribe(topicLed);

  mqtt.subscribe(subscribe_topic);
  return mqtt.connected();
}

// #################################   GPRS MQTT ###########################

void mqttCallback_gprs(char *topic, byte *payload, unsigned int len) {

  SerialGeneric.print("Message arrived [");
  SerialGeneric.print(topic);
  SerialGeneric.print("]: ");
  SerialGeneric.write(payload, len);
  SerialGeneric.println();

  //LED blink
  blinkLED(5, 200);
  //LED blink

  if (processJsonPayload((const char *)payload) == ESP_OK) {

    //  enter code for radio frequency change
    Serial.print("Sending data :");
    for (int i = 0; i < 4; i++) {
      ESerial.write(payload_from_mqtt[i]);
      if (i == 1 || i == 2) {
        Serial.print(" | 0b");
        printBinary(payload_from_mqtt[i]);
      } else {
        Serial.print(" | 0x");
        Serial.print(payload_from_mqtt[i], HEX);
      }
    }
    Serial.println(" ");
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
  //LED blink
  blinkLED(2, 500);
  //LED blink
  char *publishing_update_on_boot = "{\"update\":1}";
  mqtt_gprs.publish(publish_topic, publishing_update_on_boot);
  mqtt_gprs.subscribe(subscribe_topic);
  return mqtt_gprs.connected();
}
