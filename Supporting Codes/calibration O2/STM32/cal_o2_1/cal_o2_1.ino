#include <Arduino.h>

int DSPIN = PA5;
int DOPIN = PA1;

#define VREF    3300//VREF(mv)
#define ADC_RES 1024//ADC Resolution

const int numReadings = 10;

int readings[numReadings];  // the readings from the analog input
int readIndex = 0;          // the index of the current reading
int total = 0;              // the running total
int average = 0; 

uint32_t raw;

void setup() {

  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  Serial.begin(115200);
  analogReadResolution(10);
}

void loop()
{


  double temp = TempRead();
  temp  = temp * 0.0625; // conversion accuracy is 0.0625 / LSB

   total = total - readings[readIndex];
   readings[readIndex]=analogRead(DOPIN);
    // raw=analogRead(PA1);

   total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }
  raw=total/numReadings;

    // Serial.print("raw:\t"+String(raw)+"\tVoltage(mv)"+String(raw*VREF/ADC_RES)+ " ");
      Serial.print("raw: "+String(raw)+" Voltage(mv)"+String(raw*VREF/ADC_RES));
    delay(1000);
    // Serial.print(raw);
      Serial.print(" Temperature: ");
  Serial.print(temp);
  Serial.print(" °C");
  Serial.println("");
   delay(20);
}

 
boolean DS18B20_Init()
{
  pinMode(DSPIN, OUTPUT);
  digitalWrite(DSPIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(DSPIN, LOW);
  delayMicroseconds(750);//480-960
  digitalWrite(DSPIN, HIGH);
  pinMode(DSPIN, INPUT);
  int t = 0;
  while (digitalRead(DSPIN))
  {
    t++;
    if (t > 60) return false;
    delayMicroseconds(1);
  }
  t = 480 - t;
  pinMode(DSPIN, OUTPUT);
  delayMicroseconds(t);
  digitalWrite(DSPIN, HIGH);
  return true;
}
 
void DS18B20_Write(byte data)
{
  pinMode(DSPIN, OUTPUT);
  for (int i = 0; i < 8; i++)
  {
    digitalWrite(DSPIN, LOW);
    delayMicroseconds(10);
    if (data & 1) digitalWrite(DSPIN, HIGH);
    else digitalWrite(DSPIN, LOW);
    data >>= 1;
    delayMicroseconds(50);
    digitalWrite(DSPIN, HIGH);
  }
}
 
byte DS18B20_Read()
{
  pinMode(DSPIN, OUTPUT);
  digitalWrite(DSPIN, HIGH);
  delayMicroseconds(2);
  byte data = 0;
  for (int i = 0; i < 8; i++)
  {
    digitalWrite(DSPIN, LOW);
    delayMicroseconds(1);
    digitalWrite(DSPIN, HIGH);
    pinMode(DSPIN, INPUT);
    delayMicroseconds(5);
    data >>= 1;
    if (digitalRead(DSPIN)) data |= 0x80;
    delayMicroseconds(55);
    pinMode(DSPIN, OUTPUT);
    digitalWrite(DSPIN, HIGH);
  }
  return data;
}
 
int TempRead()
{
  if (!DS18B20_Init()) return 0;
  DS18B20_Write (0xCC); // Send skip ROM command
  DS18B20_Write (0x44); // Send reading start conversion command
  if (!DS18B20_Init()) return 0;
  DS18B20_Write (0xCC); // Send skip ROM command
  DS18B20_Write (0xBE); // Read the register, a total of nine bytes, the first two bytes are the conversion value
  int temp = DS18B20_Read (); // Low byte
  temp |= DS18B20_Read () << 8; // High byte
  return temp;
}