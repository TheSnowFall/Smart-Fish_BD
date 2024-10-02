void rain() {

  int rain_state = digitalRead(RAIN_PIN);


  Serial.print("[RAIN] ");
  if (rain_state == HIGH){
    Serial.print("The rain is NOT detected");
    }
    
  else{

    Serial.print("The rain is detected");
  }
    Serial.println(); 
  sensorData.rain = rain_state;

  // uint8_t rain_difference = (rain_state > prev_sensorData.rain) ? (rain_state - prev_sensorData.rain) : (prev_sensorData.rain - rain_state);
  // Serial.print(" , rain_difference:");
  // Serial.println(rain_difference);
  // delay(100);

  // (rain_difference != 0) ? (sensorData.rain = rain_state) : Serial.println("No change in rain values");
}