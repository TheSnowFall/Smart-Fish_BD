


void generateAPName() {
  uint8_t mac[6];
  char macStr[18] = { 0 };

  // Get MAC address
  WiFi.macAddress(mac);

  // Convert MAC address to string
  sprintf(macStr, "%02X%02X", mac[4], mac[5]);

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
  String apn_id = NVS.getString("apn_id");
  SerialGeneric.print("APN read:");
  SerialGeneric.println(apn_id);
  String real_apn_name;
  if (apn_id == "1") {
    real_apn_name = "GP-INTERNET";
    real_apn_name.toCharArray(apn, 15);
  } else if (apn_id == "2") {
    real_apn_name = "Robi-INTERNET";
    real_apn_name.toCharArray(apn, 15);
    ;
  } else if (apn_id == "3") {
    real_apn_name = "Banglalink-WEB";
    real_apn_name.toCharArray(apn, 15);
    ;
  } else if (apn_id == "4") {
    real_apn_name = "TT-INTERNET";
    real_apn_name.toCharArray(apn, 15);
    ;
  } else if (apn_id == "5") {
    real_apn_name = "Airtel Internet";
    ;
    real_apn_name.toCharArray(apn, 15);
    ;
  } else {
  }
 for (i = 0; i < 11; i++) {
        SerialGeneric.print(apn[i]);
    } 
    SerialGeneric.println("");
}





