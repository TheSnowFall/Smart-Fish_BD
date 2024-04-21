
#define DO_PIN PB0     // ESP32's pin GPIO13 connected to DO pin of the rain sensor

void setup() {
  // initialize serial communication
  Serial.begin(115200);
  // initialize the Arduino's pin as an input

  pinMode(DO_PIN, INPUT);
}

void loop() {
                    // wait 10 milliseconds

  int rain_state = digitalRead(DO_PIN);



  if (rain_state == HIGH)
    Serial.println("The rain is NOT detected");
  else
    Serial.println("The rain is detected");

  delay(1000);  // pause for 1 sec to avoid reading sensors frequently to prolong the sensor lifetime
}

