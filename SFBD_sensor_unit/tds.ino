void tds_count() {

  gravityTds.setTemperature(temp);      // set the temperature and execute temperature compensation
  gravityTds.update();                  //sample and calculate
  tdsValue= gravityTds.getTdsValue();  // then get the valu
  
  // sensorValue= gravityTds.getTdsValue();  // then get the value
  // tdsValue = addReadingToMovingAverage(sensorValue);
  Serial.print("[TDS] ");
   Serial.print("TDS: ");
  Serial.print(tdsValue, 2);
  Serial.print("ppm");
  delay(1000);

  uint32_t tds_measures_by_prob = tdsValue * 100;
  sensorData.tds = tds_measures_by_prob;
   Serial.println();

  // uint8_t tds_difference = (tds_measures_by_prob > prev_sensorData.tds) ? (tds_measures_by_prob - prev_sensorData.tds) : (prev_sensorData.tds - tds_measures_by_prob);
  // Serial.print(",  tds_difference:");
  // Serial.println(tds_difference);
  // delay(100);

  // (tds_difference > 30) ? (sensorData.tds = tds_measures_by_prob) : Serial.println("No change in TDS values");
}







// void initMovingAverage() {
//   // Initialize all readings to 0
//   for (int i = 0; i < numReadings; i++) {
//     readings[i] = 0;
//   }
// }

// // Function to add a new reading to the moving average filter and return the smoothed value
// int addReadingToMovingAverage(int newValue) {
//   // Subtract the oldest reading from the total
//   filter_total -= readings[filter_index];
//   // Store the new reading in the array
//   readings[filter_index] = newValue;
//   // Add the new reading to the total
//   filter_total += newValue;
//   // Increment the index for the next reading
//   filter_index = (filter_index + 1) % numReadings;
  
//   // Calculate the average
//   filter_average = filter_total / numReadings;

//   // Return the smoothed value
//   return filter_average;
// }