#define RELAY_1 PB12
#define RELAY_2 PB13
#define RELAY_3 PB14
#define RELAY_4 PB15
#define RELAY_5 PA15
#define RELAY_6 PB3
#define RELAY_7 PB4
#define RELAY_8 PB5


#define RELAY_9 PB6
#define RELAY_10 PB7
#define RELAY_11 PB8
#define RELAY_12 PB9


#define M0_PIN PA0
#define M1_PIN PA1


HardwareSerial LoRA(USART2);

byte payload[4] = { 0, };

void setup() {
  pinMode(M0_PIN, OUTPUT_OPEN_DRAIN);
  pinMode(M1_PIN, OUTPUT_OPEN_DRAIN);

  digitalWrite(M0_PIN, LOW);
  digitalWrite(M1_PIN, LOW);



  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);
  pinMode(RELAY_4, OUTPUT);
  pinMode(RELAY_5, OUTPUT);
  pinMode(RELAY_6, OUTPUT);
  pinMode(RELAY_7, OUTPUT);
  pinMode(RELAY_8, OUTPUT);
  pinMode(RELAY_9, OUTPUT);
  pinMode(RELAY_10, OUTPUT);
  pinMode(RELAY_11, OUTPUT);
  pinMode(RELAY_12, OUTPUT);

  Serial.begin(9600);
  LoRA.begin(9600);
  Serial.println("###### Relay operation starting ######");
}

void loop() {




  if (LoRA.available() >= 3) {
   
    for (int i = 0; i < 4; i++) {
      payload[i] = LoRA.read();
       if (i == 1 || i == 2) {
            Serial.print(" | 0b");
            printBinary(payload[i]);
          } else {
            Serial.print(" | 0x");
            Serial.print(payload[i], HEX);
          }
    }
      Serial.println(" ");
      for (int i = 0; i < 4; i++) {
          LoRA.write(payload[i]);
          }
  }



  digitalWrite(RELAY_1,  isBitHigh(payload[1], 7) ? HIGH : LOW);
  digitalWrite(RELAY_2,  isBitHigh(payload[1], 6) ? HIGH : LOW);
  digitalWrite(RELAY_3,  isBitHigh(payload[1], 5) ? HIGH : LOW);
  digitalWrite(RELAY_4,  isBitHigh(payload[1], 4) ? HIGH : LOW);
  digitalWrite(RELAY_5,  isBitHigh(payload[1], 3) ? HIGH : LOW);
  digitalWrite(RELAY_6,  isBitHigh(payload[1], 2) ? HIGH : LOW);
  digitalWrite(RELAY_7,  isBitHigh(payload[1], 1) ? HIGH : LOW);
  digitalWrite(RELAY_8,  isBitHigh(payload[1], 0) ? HIGH : LOW);

  digitalWrite(RELAY_9,  isBitHigh(payload[2], 7) ? HIGH : LOW);
  digitalWrite(RELAY_10, isBitHigh(payload[2], 6) ? HIGH : LOW);
  digitalWrite(RELAY_11, isBitHigh(payload[2], 5) ? HIGH : LOW);
  digitalWrite(RELAY_12, isBitHigh(payload[2], 4) ? HIGH : LOW);

  delay(100);  // Delay for readability

}

void printBinary(uint8_t byte) {
  for (int i = 7; i >= 0; i--) {
    Serial.print((byte >> i) & 0x01);
  }
}


uint8_t isBitHigh(unsigned char byte, int bit_position) {
  return (byte & (1 << bit_position)) == 0;
}