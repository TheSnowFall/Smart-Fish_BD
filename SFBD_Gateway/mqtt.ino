

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


void topic_ready() {
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
}



void mqttCallback(char *topic, byte *payload, unsigned int len) {
  SerialGeneric.print("Message arrived by W [");
  SerialGeneric.print(topic);
  SerialGeneric.print("]: ");
  SerialGeneric.write(payload, len);
  SerialGeneric.println();

  if ( processJsonPayload((const char*)payload) == ESP_OK) {

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
  if ( processJsonPayload((const char*)payload) == ESP_OK) {

    //  enter code for radio frequency change

    for (int i = 0; i < 4; i++) {
      ESerial.write(payload_from_mqtt[i]);
    }
  }
  else{
    SerialGeneric.println("Json pars failed");
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
