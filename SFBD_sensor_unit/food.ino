void food_lvl_detection() {

  if (digitalRead(FOOD_EMPTY) == LOW && digitalRead(FOOD_MEDIUM) == LOW && digitalRead(FOOD_FULL) == LOW) {
    sensorData.food = 3;
  } else if (digitalRead(FOOD_EMPTY) == LOW && digitalRead(FOOD_MEDIUM) == LOW && digitalRead(FOOD_FULL) == HIGH) {
    sensorData.food = 2;
  } else if (digitalRead(FOOD_EMPTY) == LOW && digitalRead(FOOD_MEDIUM) == HIGH && digitalRead(FOOD_FULL) == HIGH) {
    sensorData.food = 1;
  } else if (digitalRead(FOOD_EMPTY) == HIGH && digitalRead(FOOD_MEDIUM) == HIGH && digitalRead(FOOD_FULL) == HIGH) {
    sensorData.food = 0;
  }
}