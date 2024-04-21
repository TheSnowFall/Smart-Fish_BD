void temperature() {
  // put your main code here, to run repeatedly:
  // temp = TempRead();
  temp = TempRead() * 0.0625;  // conversion accuracy is 0.0625 / LSB
  Serial.print("[TEMP]");
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print(" °C");


  // delay(500);
  uint32_t temp_measures_by_prob = temp * 100;


  uint8_t temperature_difference = (temp_measures_by_prob > prev_sensorData.temperature) ? (temp_measures_by_prob - prev_sensorData.temperature) : (prev_sensorData.temperature - temp_measures_by_prob);
  Serial.print(", temperature_difference:");
  Serial.println(temperature_difference);
  delay(100);

  (temperature_difference > 30) ? (sensorData.temperature = temp_measures_by_prob) : Serial.println("No change in temperature values");
}

boolean DS18B20_Init() {
  pinMode(TEMP_PIN, OUTPUT);
  digitalWrite(TEMP_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(TEMP_PIN, LOW);
  delayMicroseconds(750);  //480-960
  digitalWrite(TEMP_PIN, HIGH);
  pinMode(TEMP_PIN, INPUT);
  int t = 0;
  while (digitalRead(TEMP_PIN)) {
    t++;
    if (t > 60) return false;
    delayMicroseconds(1);
  }
  t = 480 - t;
  pinMode(TEMP_PIN, OUTPUT);
  delayMicroseconds(t);
  digitalWrite(TEMP_PIN, HIGH);
  return true;
}

void DS18B20_Write(byte data) {
  pinMode(TEMP_PIN, OUTPUT);
  for (int i = 0; i < 8; i++) {
    digitalWrite(TEMP_PIN, LOW);
    delayMicroseconds(10);
    if (data & 1) digitalWrite(TEMP_PIN, HIGH);
    else digitalWrite(TEMP_PIN, LOW);
    data >>= 1;
    delayMicroseconds(50);
    digitalWrite(TEMP_PIN, HIGH);
  }
}

byte DS18B20_Read() {
  pinMode(TEMP_PIN, OUTPUT);
  digitalWrite(TEMP_PIN, HIGH);
  delayMicroseconds(2);
  byte data = 0;
  for (int i = 0; i < 8; i++) {
    digitalWrite(TEMP_PIN, LOW);
    delayMicroseconds(1);
    digitalWrite(TEMP_PIN, HIGH);
    pinMode(TEMP_PIN, INPUT);
    delayMicroseconds(5);
    data >>= 1;
    if (digitalRead(TEMP_PIN)) data |= 0x80;
    delayMicroseconds(55);
    pinMode(TEMP_PIN, OUTPUT);
    digitalWrite(TEMP_PIN, HIGH);
  }
  return data;
}

int TempRead() {
  if (!DS18B20_Init()) return 0;
  DS18B20_Write(0xCC);  // Send skip ROM command
  DS18B20_Write(0x44);  // Send reading start conversion command
  if (!DS18B20_Init()) return 0;
  DS18B20_Write(0xCC);          // Send skip ROM command
  DS18B20_Write(0xBE);          // Read the register, a total of nine bytes, the first two bytes are the conversion value
  int temp = DS18B20_Read();    // Low byte
  temp |= DS18B20_Read() << 8;  // High byte
  return temp;
}