int8_t processJsonPayload(const char* json_payload) {

  payload_from_mqtt[4] = {0,};

  DynamicJsonDocument doc(512); // Adjust the size as per your JSON message
  DeserializationError error = deserializeJson(doc, json_payload);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return ESP_FAIL;
  }

  // Access the values from JSON
  const char* addr = doc["addr"];
  const char* type = doc["type"];
  const char* relay = doc["relay"];

  byte address_hex = strtol(addr, NULL, 0); // Convert address to byte
  // Store data into payload_from_mqtt array
  payload_from_mqtt[0] = address_hex; // Address

  // unsigned long instructions_bin = strtoul(relay, NULL, 2); // Convert relay to unsigned long
 uint16_t instructions_bin = strtoul(relay, NULL, 2); // Convert relay to unsigned long
  SerialGeneric.print("[JSON_PARSER] instructions_bin:"); SerialGeneric.println(instructions_bin,BIN);
  instructions_bin <<= 4;
  uint8_t relayMSB = (uint8_t)((instructions_bin >> 8) & 0xFF); // Most significant byte
  uint8_t relayLSB = (uint8_t)(instructions_bin & 0xFF);        // Least significant byte
  payload_from_mqtt[1] = relayMSB; // First 8 bits of relay
  payload_from_mqtt[2] = relayLSB; // Last 8 bits of relay

  // Convert type to byte
  byte type_hex;
  if (strcmp(type, "sw") == 0) {
    type_hex = 0x33; // Hex value for "sw"
  } else {
    type_hex = 0x00; // Default value
  }
  payload_from_mqtt[3] = type_hex; // Type

  return ESP_OK;
}