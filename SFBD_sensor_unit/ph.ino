void ph_count() {
  static unsigned long timepoint = millis();
  if (millis() - timepoint > 1000U) {  //time interval: 1s
    timepoint = millis();
    voltage = analogRead(PH_PIN) / 1024.0 * 3300;  // read the voltage
    phValue = ph.readPH(voltage, temp);            // convert voltage to pH with temp compensation
    Serial.print("[pH]");
    Serial.print("pH: ");
    Serial.print(phValue);


    uint8_t pH_difference = (phValue > prev_sensorData.ph) ? (phValue - prev_sensorData.ph) : (prev_sensorData.ph - phValue);
    Serial.print(", pH_difference:");
    Serial.println(pH_difference);
    delay(100);

    (pH_difference > 0.5) ? (sensorData.ph = phValue) : Serial.println("No change in pH values");
  }
  ph.calibration(voltage, temp);  // calibration process by Serail CMD
}