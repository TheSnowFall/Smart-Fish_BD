#include <Arduino.h>

#define VREF    3300//VREF(mv)
#define ADC_RES 1024//ADC Resolution

uint32_t raw;

void setup()
{
    Serial.begin(115200);
    analogReadResolution(10);
}

void loop()
{
    raw=analogRead(PA1);
    Serial.println("raw:\t"+String(raw)+"\tVoltage(mv)"+String(raw*VREF/ADC_RES));
    delay(1000);
}