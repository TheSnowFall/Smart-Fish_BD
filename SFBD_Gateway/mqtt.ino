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
    strcpy(topic_name_buffer, apNameCharMqtt);
    strcat(topic_name_buffer, "/SUB");
    return ESP_OK;
  } else if (topic_type == 1) {
    strcpy(topic_name_buffer, apNameCharMqtt);
    strcat(topic_name_buffer, "/PUB");
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
  // blinkLED(5, 200);
  //LED blink
  if (processJsonPayload((const char *)payload) == ESP_OK) {

  Transceiver.SetChannel(28); //438 MHz
  Transceiver.SaveParameters(TEMPORARY);
  channel_change = true;
  chChangeTimerStart = millis();

    Serial.print("Sending data :");
    for (int i = 0; i < 4; i++) {
      ESerial.write(relay_payload_from_mqtt[i]);
      if (i == 1 || i == 2) {
        Serial.print(" | 0b");
        printBinary(relay_payload_from_mqtt[i]);
      } else {
        Serial.print(" | 0x");
        Serial.print(relay_payload_from_mqtt[i], HEX);
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
  digitalWrite(LED_GPIO, LED_ON );
  //LED blink
  // blinkLED(2, 500);
  //LED blink


  if(status_update == false){
    char *publishing_update_on_boot = "{\"update\":1}";
       mqtt.publish(publish_topic, publishing_update_on_boot);
       status_update = true;
  }
  


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
 
  //LED blink

  if (processJsonPayload((const char *)payload) == ESP_OK) {

    //  enter code for radio frequency change
  Transceiver.SetChannel(28); //438 MHz
  Transceiver.SaveParameters(TEMPORARY);
  channel_change = true;
  chChangeTimerStart = millis();
    Serial.print("Sending data :");
    for (int i = 0; i < 4; i++) {
      ESerial.write(relay_payload_from_mqtt[i]);
      if (i == 1 || i == 2) {
        Serial.print(" | 0b");
        printBinary(relay_payload_from_mqtt[i]);
      } else {
        Serial.print(" | 0x");
        Serial.print(relay_payload_from_mqtt[i], HEX);
      }
    }
    Serial.println(" ");
    relay_payload_from_mqtt[4]={0,};
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
    // modem.init();
    return false;
  }
  SerialGeneric.println(" success");
  digitalWrite(LED_GPIO, LED_ON);

  //LED blink
  // blinkLED(2, 500);
  //LED blink
  if(status_update == false){
    char *publishing_update_on_boot = "{\"update\":1}";
     mqtt_gprs.publish(publish_topic, publishing_update_on_boot);
     status_update = true;
  }
  
 
  mqtt_gprs.subscribe(subscribe_topic);
  return mqtt_gprs.connected();
}
