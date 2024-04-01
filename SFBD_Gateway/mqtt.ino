
void mqttCallback(char *topic, byte *payload, unsigned int len) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.write(payload, len);
  Serial.println();

  // Only proceed if incoming message's topic matches
  if (String(topic) == topicLed) {
    ledStatus = !ledStatus;
    digitalWrite(LED_GPIO, ledStatus);
    mqtt.publish(topicLedStatus, ledStatus ? "1" : "0");
  }
}



boolean mqttConnect()
{
    Serial.print("Connecting to ");
    Serial.print(broker);

    // Connect to MQTT Broker
    // boolean status = mqtt.connect("GsmClientTest");

    // // Or, if you want to authenticate MQTT:
    boolean status = mqtt.connect("GsmClientName", "rayan", "rayan");

    if (status == false) {
        Serial.println(" fail");
        return false;
    }
    Serial.println(" success");

    mqtt.subscribe(topicLed);
    return mqtt.connected();
}

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