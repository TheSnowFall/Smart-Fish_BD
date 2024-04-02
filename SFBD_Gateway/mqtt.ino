

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






void mqttCallback(char *topic, byte *payload, unsigned int len) {
  SerialGeneric.print("Message arrived [");
  SerialGeneric.print(topic);
  SerialGeneric.print("]: ");
  SerialGeneric.write(payload, len);
  SerialGeneric.println();

  // Only proceed if incoming message's topic matches
  if (String(topic) == topicLed) {
    ledStatus = !ledStatus;
    digitalWrite(LED_GPIO, ledStatus);
    mqtt.publish(topicLedStatus, ledStatus ? "1" : "0");
  }
}



boolean mqttConnect() {
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

  // Only proceed if incoming message's topic matches
  if (String(topic) == topicLed) {
    ledStatus = !ledStatus;
    digitalWrite(LED_GPIO, ledStatus);
    mqtt_gprs.publish(topicLedStatus, ledStatus ? "1" : "0");
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


