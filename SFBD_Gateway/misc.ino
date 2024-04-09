


void generateAPName() {
  mac[6] = { 0 };
  char macStr[18] = { 0 };

  // Get MAC address
  WiFi.macAddress(mac);

  // Convert MAC address to string
  sprintf(macStr, "%02X%02X", mac[4], mac[5]);
  sprintf(gd_id, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

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
  SerialGeneric.println("[CALLBACK] saveParamCallback fired");
  apn_id = getParam("customfieldid");
  SerialGeneric.println("PARAM customfieldid = " + apn_id);
  bool return_erro_check;
  return_erro_check = NVS.setString("apn_id", apn_id);
  if (!return_erro_check) {

    SerialGeneric.println("APN ID saving fail");
  } else {
    SerialGeneric.println("APN ID saving success");
  }
}




void set_apn() {
  apn_id = NVS.getString("apn_id");
  SerialGeneric.print("APN read:");
  SerialGeneric.println(apn_id);
  String real_apn_name;
  if (apn_id == "1") {
    real_apn_name = "GP-INTERNET";
    real_apn_name.toCharArray(apn, 15);
    apnIsEmpty = false;
    Serial.print("[SET APN] apnIsEmpty = ");
    Serial.println(apnIsEmpty);

  } else if (apn_id == "2") {
    real_apn_name = "Robi-INTERNET";
    real_apn_name.toCharArray(apn, 15);
    apnIsEmpty = false;
    Serial.print("[SET APN] apnIsEmpty = ");
    Serial.println(apnIsEmpty);

  } else if (apn_id == "3") {
    real_apn_name = "Banglalink-WEB";
    real_apn_name.toCharArray(apn, 15);
    apnIsEmpty = false;
    Serial.print("[SET APN] apnIsEmpty = ");
    Serial.println(apnIsEmpty);

  } else if (apn_id == "4") {
    real_apn_name = "TT-INTERNET";
    real_apn_name.toCharArray(apn, 15);
    apnIsEmpty = false;
    Serial.print("[SET APN] apnIsEmpty = ");
    Serial.println(apnIsEmpty);

  } else if (apn_id == "5") {
    real_apn_name = "internet";
    real_apn_name.toCharArray(apn, 15);
    apnIsEmpty = false;
    Serial.print("[SET APN] apnIsEmpty = ");
    Serial.println(apnIsEmpty);

  } else {
    apnIsEmpty = true;
    Serial.print("[SET APN] apnIsEmpty = ");
    Serial.println(apnIsEmpty);
  }

  if (!apnIsEmpty) {
    int i = 0;
    for (i = 0; i < 11; i++) {
      SerialGeneric.print(apn[i]);
    }
  }
  SerialGeneric.println("");
}


bool delayPassed(unsigned long delayTime) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= delayTime) {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}


void erase_apn() {

  bool return_erro_check;
  return_erro_check = NVS.setString("apn_id", "");
  if (!return_erro_check) {

    SerialGeneric.println("APN could not be cleared");
  } else {
    SerialGeneric.println("APN cleared");
  }
}


void printBinary(uint8_t byte) {
  for (int i = 7; i >= 0; i--) {
    Serial.print((byte >> i) & 0x01);
  }
}

void processEbyteSerial() {
  while (ESerial.available()) {
    byte incomingByte = ESerial.read();

    // Check if received payload[4]
    if (incomingByte == sw_addr && relay_msg_index == 0) {
      received_relay_msg[relay_msg_index++] = incomingByte;
    } else if (relay_msg_index > 0 && relay_msg_index < 4) {
      received_relay_msg[relay_msg_index++] = incomingByte;
      if (relay_msg_index == 4 && incomingByte == endbyte_sw) {
        // Entire payload[4] received
        // Do something with received_relay_msg[] here
        // For example, print it to Serial
        publish_relay_response();
        if (wifi_fail_flag == false && gsm_task_flag == false) {
          mqtt.publish(publish_topic, relay_response_to_server);
        } else {
          mqtt_gprs.publish(publish_topic, relay_response_to_server);
        }
        Serial.print("Received relay Response:  ");
        for (int i = 0; i < 4; i++) {

          if (i == 1 || i == 2) {
            Serial.print(" | 0b");
            printBinary(received_relay_msg[i]);
          } else {
            Serial.print(" | 0x");
            Serial.print(received_relay_msg[i], HEX);
            Serial.print(" ");
          }
        }
        Serial.println();
        // Reset relay_msg_index for next payload[4]
        relay_msg_index = 0;
        Transceiver.SetChannel(23);
        Transceiver.SaveParameters(TEMPORARY);
      }
    }

    // Check if received received_sen_data[10]
    if (incomingByte == sen_addr && sen_data_index == 0) {
      received_sen_data[sen_data_index++] = incomingByte;
    } else if (sen_data_index > 0 && sen_data_index < 10) {
      received_sen_data[sen_data_index++] = incomingByte;
      if (sen_data_index == 10 && incomingByte == endbyte_sen) {
        // Entire received_sen_data[10] received
        // Do something with received_sen_data[] here
        // For example, print it to Serial
        Serial.println("Received sensor data:");
        for (int i = 0; i < 10; i++) {
          if (i == 0 || i == 9) {
            Serial.print(" | 0x");
            Serial.print(received_sen_data[i], HEX);
            Serial.print(" ");
          } else {
            Serial.print(" | 0b");
            printBinary(received_sen_data[i]);
            Serial.print(" ");
          }
        }

        Serial.println();
        SensorData unpackedData;
        unpackSensorData(unpackedData);
        publish_sensor_data(unpackedData);
        printSensorData(unpackedData);

        // Publish the JSON string to the specified topic
        if (wifi_fail_flag == false && gsm_task_flag == false) {
          mqtt.publish(publish_topic, sensor_data_to_server);
        } else {
          mqtt_gprs.publish(publish_topic, sensor_data_to_server);
        }
        // Reset sen_data_index for next received_sen_data[10]
        sen_data_index = 0;
      }
    }
  }
}

void blinkLED(int numBlinks, int blinkDelay) {


  for (int i = 0; i < numBlinks; i++) {
    digitalWrite(LED_GPIO, LOW);   // Turn LED on
    delayPassed(blinkDelay);       // Wait
    digitalWrite(LED_GPIO, HIGH);  // Turn LED off
    delayPassed(blinkDelay);       // Wait
  }
}


void unpackSensorData(SensorData& data) {
  // Unpack food (2 bits)
  data.food = (received_sen_data[1] >> 6) & 0x03;

  // Unpack tds (17 bits)
  data.tds = ((received_sen_data[1] & 0x3F) << 16) | (received_sen_data[2] << 8) | received_sen_data[3];

  // Unpack rain (8 bits)
  data.rain = received_sen_data[4];

  // Unpack temp (15 bits)
  data.temp = (received_sen_data[5] << 8) | received_sen_data[6];

  // Unpack o2 (12 bits)
  data.o2 = ((received_sen_data[7] << 4) & 0xFF0) | ((received_sen_data[8] >> 4) & 0x0F);

  // Unpack ph (4 bits)
  data.ph = received_sen_data[8] & 0x0F;
}

void printSensorData(const SensorData& data) {
  Serial.println("############## Print Sensor data start ##############");
  Serial.print("Food: ");
  Serial.println(data.food);
  Serial.print("TDS: ");
  Serial.println((float)data.tds / 100);
  Serial.print("Rain: ");
  Serial.println(data.rain);
  Serial.print("Temp: ");
  Serial.println((float)data.temp / 100);
  Serial.print("O2: ");
  Serial.println((float)data.o2 / 100);
  Serial.print("pH: ");
  Serial.println(data.ph);
  Serial.println("############## Print Sensor data end  ##############");
}