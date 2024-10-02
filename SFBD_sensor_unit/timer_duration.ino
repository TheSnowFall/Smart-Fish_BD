// void rtc_SecondsCB(void *data)
// {
//   UNUSED(data);
// }

// void timer_setup(){

// #if defined(LED_BUILTIN)
//   // configure pin in output mode
//   pinMode(pin, OUTPUT);
//   digitalWrite(pin, HIGH);
// #endif /* LED_BUILTIN */

//   Serial.print("RTC Init ");

//   // Select RTC clock source: LSI_CLOCK, LSE_CLOCK or HSE_CLOCK.
//   // By default the LSI is selected as source. Use LSE for better accuracy if available
//   // rtc.setClockSource(STM32RTC::LSE_CLOCK);

//   // initialize RTC 24H format
//   rtc.begin(hourFormat);

//   // Set the time
//   rtc.setMinutes(0);
//   rtc.setSeconds(0);

//   Serial.println("with seconds Alarm");
//   rtc.attachSecondsInterrupt(rtc_SecondsCB);

// }

// void timer_duration(){

  
// }