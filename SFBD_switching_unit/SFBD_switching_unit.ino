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


HardwareSerial LoRa(USART2);

byte payload[4] = { 0, };
byte res_payload[4] = { 0, };
uint8_t sw_addr= 0x44 ;

bool send_response = false;
bool response_sent = false;

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

digitalWrite(RELAY_1, HIGH);
digitalWrite(RELAY_2, HIGH);
digitalWrite(RELAY_3, HIGH);
digitalWrite(RELAY_4, HIGH);
digitalWrite(RELAY_5, HIGH);
digitalWrite(RELAY_6, HIGH);
digitalWrite(RELAY_7, HIGH);
digitalWrite(RELAY_8, HIGH);
digitalWrite(RELAY_9, HIGH);
digitalWrite(RELAY_10, HIGH);
digitalWrite(RELAY_11, HIGH);
digitalWrite(RELAY_12, HIGH);



  Serial.begin(9600);
  LoRa.begin(9600);
  Serial.println("###### Relay operation starting ######");
}

void loop() {




  if (LoRa.available() >= 3) {
   
    for (int i = 0; i < 4; i++) {
      payload[i] = LoRa.read();
       if (i == 1 || i == 2) {
            Serial.print(" | 0b");
            printBinary(payload[i]);
          } else {
            Serial.print(" | 0x");
            Serial.print(payload[i], HEX);
          }
    }
      Serial.println(" ");
      // for (int i = 0; i < 4; i++) {
      //     LoRa.write(payload[i]);
      //     }

      send_response = true;
  }

  
if(payload[0] == sw_addr && payload[3] == 0x33){

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


}


  delay(100);  // Delay for readability

  if(send_response){
    // read_pin_state();
    for (int k = 0; k < 4; k++) {
          LoRa.write(payload[k]);
          if (k == 1 || k == 2) {
            Serial.print(" | 0b");
            printBinary(payload[k]);
          } else {
            Serial.print(" | 0x");
            Serial.print(payload[k], HEX);
          }

          }
          Serial.println(" ");
          response_sent =true;

  }

if(response_sent){
send_response =false;
response_sent = false;

}


}

void printBinary(uint8_t byte) {
  for (int i = 7; i >= 0; i--) {
    Serial.print((byte >> i) & 0x01);
  }
}


uint8_t isBitHigh(unsigned char byte, int bit_position) {
  return (byte & (1 << bit_position)) == 0;
}


// void read_pin_state(){

 
//   pinMode(RELAY_1, INPUT);
//   pinMode(RELAY_2, INPUT);
//   pinMode(RELAY_3, INPUT);
//   pinMode(RELAY_4, INPUT);
//   pinMode(RELAY_5, INPUT);
//   pinMode(RELAY_6, INPUT);
//   pinMode(RELAY_7, INPUT);
//   pinMode(RELAY_8, INPUT);
//   pinMode(RELAY_9, INPUT);
//   pinMode(RELAY_10, INPUT);
//   pinMode(RELAY_11, INPUT);
//   pinMode(RELAY_12, INPUT);

//     delay(200);
//     Serial.println(digitalRead(RELAY_1));
//     bitWrite(res_payload[1], 7, (digitalRead(RELAY_1)== HIGH)? 0 : 1 );
//      Serial.println(digitalRead(RELAY_2));
//     bitWrite(res_payload[1], 6, (digitalRead(RELAY_2)== HIGH)? 0 : 1 );
//      Serial.println(digitalRead(RELAY_3));
//     bitWrite(res_payload[1], 5, (digitalRead(RELAY_3)== HIGH)? 0 : 1 );
//      Serial.println(digitalRead(RELAY_4));
//     bitWrite(res_payload[1], 4, (digitalRead(RELAY_4)== HIGH)? 0 : 1 );
//      Serial.println(digitalRead(RELAY_5));
//     bitWrite(res_payload[1], 3, (digitalRead(RELAY_5)== HIGH)? 0 : 1 );
//      Serial.println(digitalRead(RELAY_6));
//     bitWrite(res_payload[1], 2, (digitalRead(RELAY_6)== HIGH)? 0 : 1 );
//      Serial.println(digitalRead(RELAY_7));
//     bitWrite(res_payload[1], 1, (digitalRead(RELAY_7)== HIGH)? 0 : 1 );
//      Serial.println(digitalRead(RELAY_8));
//     bitWrite(res_payload[1], 0, (digitalRead(RELAY_8)== HIGH)? 0 : 1 );
     

//     Serial.println(digitalRead(RELAY_9));
//     bitWrite(res_payload[2], 7, (digitalRead(RELAY_9)== HIGH)? 0 : 1 );
//     Serial.println(digitalRead(RELAY_10));
//     bitWrite(res_payload[2], 6, (digitalRead(RELAY_10)== HIGH)? 0 : 1 );
//     Serial.println(digitalRead(RELAY_11));
//     bitWrite(res_payload[2], 5, (digitalRead(RELAY_11)== HIGH)? 0 : 1 );
//     Serial.println(digitalRead(RELAY_12));
//     bitWrite(res_payload[2], 4, (digitalRead(RELAY_12)== HIGH)? 0 : 1 );

//     bitWrite(res_payload[2], 3, 0);
//     bitWrite(res_payload[2], 2, 0);
//     bitWrite(res_payload[2], 1, 0);
//     bitWrite(res_payload[2], 0, 0);


//     res_payload[0]=payload[0];
//     res_payload[3]=payload[3];

//   pinMode(RELAY_1, OUTPUT);
//   pinMode(RELAY_2, OUTPUT);
//   pinMode(RELAY_3, OUTPUT);
//   pinMode(RELAY_4, OUTPUT);
//   pinMode(RELAY_5, OUTPUT);
//   pinMode(RELAY_6, OUTPUT);
//   pinMode(RELAY_7, OUTPUT);
//   pinMode(RELAY_8, OUTPUT);
//   pinMode(RELAY_9, OUTPUT);
//   pinMode(RELAY_10, OUTPUT);
//   pinMode(RELAY_11, OUTPUT);
//   pinMode(RELAY_12, OUTPUT);


// }