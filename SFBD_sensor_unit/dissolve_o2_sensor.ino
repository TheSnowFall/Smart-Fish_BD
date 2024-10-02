int16_t readDO(uint32_t voltage_mv, uint8_t temperature_c)
{
#if TWO_POINT_CALIBRATION == 0
  uint16_t V_saturation = (uint32_t)CAL1_V + (uint32_t)35 * temperature_c - (uint32_t)CAL1_T * 35;
  return (voltage_mv * DO_Table[temperature_c] / V_saturation);
#else
  uint16_t V_saturation = (int16_t)((int8_t)temperature_c - CAL2_T) * ((uint16_t)CAL1_V - CAL2_V) / ((uint8_t)CAL1_T - CAL2_T) + CAL2_V;
  return (voltage_mv * DO_Table[temperature_c] / V_saturation);
#endif
}


void dissolve_o2_sen() {

  Temperaturet = (uint8_t)temp;
  filter_total = filter_total - readings[filter_index];
  readings[filter_index] = analogRead(DO_PIN);

  filter_total = filter_total + readings[filter_index];
  // advance to the next position in the array:
  filter_index = filter_index + 1;

  // if we're at the end of the array...
  if (filter_index >= numReadings) {
    // ...wrap around to the beginning:
    filter_index = 0;
  }
  ADC_Raw=filter_total/numReadings;


  ADC_Voltage = uint32_t(VREF) * ADC_Raw / ADC_RES;
  Serial.print("[DO]");
  Serial.print("DO:   ");
  Serial.print(readDO(ADC_Voltage, Temperaturet) / 1000.0);
  Serial.print("mg/L");
  Serial.println();

  uint16_t o2_measures_by_prob = readDO(ADC_Voltage, Temperaturet) / 1000.0 * 100;

 sensorData.o2 = o2_measures_by_prob;


  // uint8_t o2_difference = (o2_measures_by_prob > prev_sensorData.o2) ? (o2_measures_by_prob - prev_sensorData.o2) : (prev_sensorData.o2 - o2_measures_by_prob);
  // Serial.print(",  o2_difference:");
  // Serial.println(o2_difference);
  // delay(100);

  // (o2_difference > 30) ? (sensorData.o2 = o2_measures_by_prob) : Serial.println("No change in D.O values");
}
