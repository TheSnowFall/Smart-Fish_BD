#include <STM32RTC.h>

#ifndef ONESECOND_IRQn
#error "RTC has no feature for One-Second interrupt"
#endif

/* use led to display seconds */
#if defined(LED_BUILTIN)
#define pin  LED_BUILTIN
#endif

/* Get the rtc object */
STM32RTC& rtc = STM32RTC::getInstance();

static STM32RTC::Hour_Format hourFormat = STM32RTC::HOUR_24;

void setup()
{
  Serial.begin(115200);
  while (!Serial) {}

#if defined(LED_BUILTIN)
  // configure pin in output mode
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
#endif /* LED_BUILTIN */

  Serial.print("RTC Init ");

  // Select RTC clock source: LSI_CLOCK, LSE_CLOCK or HSE_CLOCK.
  // By default the LSI is selected as source. Use LSE for better accuracy if available
  // rtc.setClockSource(STM32RTC::LSE_CLOCK);

  // initialize RTC 24H format
  rtc.begin(hourFormat);

  // Set the time
  rtc.setMinutes(0);
  rtc.setSeconds(0);

  Serial.println("with seconds Alarm");
  rtc.attachSecondsInterrupt(rtc_SecondsCB);
}

void loop()
{
  if (time_duration(5)) {
    Serial.println("5 minutes have passed. Resetting the minute counter.");
  }

  uint8_t minutes = rtc.getMinutes();
  uint8_t seconds = rtc.getSeconds();

  // Print the time every second
  Serial.printf("%02d:%02d\n", minutes, seconds);

#if defined(LED_BUILTIN)
  digitalWrite(pin, !digitalRead(pin)); // Toggle LED
#endif /* LED_BUILTIN */

  delay(1000);
}

/* callback function on each second interrupt */
void rtc_SecondsCB(void *data)
{
  UNUSED(data);
}

bool time_duration(uint8_t duration_minutes)
{
  static uint8_t start_minutes = rtc.getMinutes();
  uint8_t current_minutes = rtc.getMinutes();

  if (current_minutes < start_minutes) {
    current_minutes += 60; // handle wrap-around from 59 to 0 minutes
  }

  if ((current_minutes - start_minutes) >= duration_minutes) {
    // Reset the start time
    start_minutes = rtc.getMinutes();
    rtc.setMinutes(0);
    rtc.setSeconds(0);
    return true;
  }

  return false;
}
