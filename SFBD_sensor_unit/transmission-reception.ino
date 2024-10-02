void processEbytLoRaSerial() {
  byte receivedBytes[3] = { 0,};
  int byteIndex = 0;

  while (LoRa.available()) {
    byte incomingByte = LoRa.read();
    // Serial.println(incomingByte);

    if (byteIndex == 0 && incomingByte == 0x26) {

      receivedBytes[byteIndex] = incomingByte;

      // Serial.print("Received first byte & index: "); Serial.print(receivedBytes[byteIndex]); Serial.print(",");Serial.println(byteIndex);
      byteIndex++;


    } else if (byteIndex == 1 && incomingByte == 0xAB) {
      receivedBytes[byteIndex] = incomingByte;

      // Serial.print("Received first byte & index: "); Serial.print(receivedBytes[byteIndex]); Serial.print(",");Serial.println(byteIndex);
      byteIndex++;
    } else if (byteIndex == 2) {

      receivedBytes[byteIndex] = incomingByte;
      // Serial.print("Received first byte & index: "); Serial.print(receivedBytes[byteIndex]); Serial.print(",");Serial.println(byteIndex);
      byteIndex = 0;

    } else {
      Serial.println("Invalid byte, resetting...");
      byteIndex = 0;
    }
  }

  if (receivedBytes[0] == 0x26 && receivedBytes[2] == 0x22) {
    Serial.print("Request received: ");
    Serial.print("0x");
    Serial.print(receivedBytes[0], HEX);
    Serial.print(" ");
    Serial.print("0x");
    Serial.print(receivedBytes[1], HEX);
    Serial.print(" ");
    Serial.print("0x");
    Serial.print(receivedBytes[2], HEX);
    Serial.println(" ");
  }

  if (receivedBytes[0] == 0x26 && receivedBytes[1] == 0xAB && receivedBytes[2] == 0x22) {

    packSensorData(sensorData);

    // Print the packed payload
    Serial.println("Packed Payload:");
    for (int i = 0; i < 10; i++) {
      LoRa.write(sen_payload[i]);
      Serial.print(sen_payload[i], HEX);  // Print in hexadecimal format
      Serial.print(" ");
    }
    Serial.println();

    // Update previous sensor data for comparison
    prev_sensorData = sensorData;
    receivedBytes[3] = {0,};
  }
}
