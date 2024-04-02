void getting_sim_ready(){
  
  setupModem();

  SerialGeneric.println("Wait...");

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialGeneric.println("Initializing modem...");
  modem.restart();
  // modem.init();

  String modemInfo = modem.getModemInfo();
  SerialGeneric.print("Modem Info: ");
  SerialGeneric.println(modemInfo);


  if ( GSM_PIN && modem.getSimStatus() != 3 ) {
        modem.simUnlock(GSM_PIN);
    }
  SerialGeneric.print("Waiting for the sim network...");
    if (!modem.waitForNetwork()) {
        SerialGeneric.println(" fail");
        delay(10000);
        return;
    }
    SerialGeneric.println(" success");

    if (modem.isNetworkConnected()) {
        SerialGeneric.println("Network connected");
    }

    // GPRS connection parameters are usually set after network registration
    SerialGeneric.print(F("Connecting to "));
    SerialGeneric.print(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        SerialGeneric.println(" fail");
        delay(10000);
        return;
    }
    SerialGeneric.println(" success");

    if (modem.isGprsConnected()) {
        SerialGeneric.println("GPRS connected");
    }
}