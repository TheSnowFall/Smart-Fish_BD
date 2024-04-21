
void packSensorData(const SensorData& data) {
  sen_payload[0] = 0x26;  // Address
  sen_payload[9] = 0x22;  // Last element

  // Pack food (2 bits)
  sen_payload[1] = (data.food << 6) & 0xC0;

  // Pack tds (17 bits)
  sen_payload[1] |= (data.tds >> 16) & 0x3F;
  sen_payload[2] = (data.tds >> 8) & 0xFF;
  sen_payload[3] = data.tds & 0xFF;

  // Pack rain (8 bits)
  sen_payload[4] = data.rain;

  // Pack temperature (15 bits)
  sen_payload[5] = (data.temperature >> 8) & 0xFF;
  sen_payload[6] = data.temperature & 0xFF;

  // Pack o2 (12 bits)
  sen_payload[7] = (data.o2 >> 4) & 0xFF;
  sen_payload[8] = ((data.o2 & 0x0F) << 4) & 0xF0;

  // Pack ph (4 bits)
  sen_payload[8] |= (data.ph & 0x0F);
}

// Function to unpack the sen_payload array into the sensor data
void unpackSensorData(SensorData& data) {
  // Unpack food (2 bits)
  data.food = (sen_payload[1] >> 6) & 0x03;

  // Unpack tds (17 bits)
  data.tds = ((sen_payload[1] & 0x3F) << 16) | (sen_payload[2] << 8) | sen_payload[3];

  // Unpack rain (8 bits)
  data.rain = sen_payload[4];

  // Unpack temperature (15 bits)
  data.temperature = (sen_payload[5] << 8) | sen_payload[6];

  // Unpack o2 (12 bits)
  data.o2 = ((sen_payload[7] << 4) & 0xFF0) | ((sen_payload[8] >> 4) & 0x0F);

  // Unpack ph (4 bits)
  data.ph = sen_payload[8] & 0x0F;
}

void printSensorData(const SensorData& data) {
  Serial.println("############## Print Sensor data start ##############");
  Serial.print("Food: ");
  Serial.println(data.food);
  Serial.print("TDS: ");
  Serial.println((float)data.tds / 100);
  Serial.print("Rain: ");
  Serial.println(data.rain);
  Serial.print("Temperature: ");
  Serial.println((float)data.temperature / 100);
  Serial.print("O2: ");
  Serial.println((float)data.o2 / 100);
  Serial.print("pH: ");
  Serial.println(data.ph);
  Serial.println("############## Print Sensor data end  ##############");
}
