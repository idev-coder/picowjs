#include "rtc.h"

#include <time.h>

#include "hardware/rtc.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"

void picowjs_rtc_init() {
  rtc_init();
  // set as unix epoch time
  datetime_t t = {
      .year = 1970, .month = 1, .day = 1, .dotw = 4, .min = 0, .sec = 0};
  rtc_set_datetime(&t);
}

void picowjs_rtc_cleanup() {}

void picowjs_rtc_set_time(uint64_t time) {
  struct tm* ptm;
  uint64_t stime = time / 1000;
  // uint64_t ms = time % 1000;
  time_t t = (time_t)stime;
  ptm = gmtime(&t);
  datetime_t datetime;
  datetime.sec = ptm->tm_sec;
  datetime.min = ptm->tm_min;
  datetime.hour = ptm->tm_hour;
  datetime.day = ptm->tm_mday;
  datetime.dotw = ptm->tm_wday;
  datetime.month = ptm->tm_mon + 1;
  datetime.year = ptm->tm_year + 1900;
  rtc_set_datetime(&datetime);
}

uint64_t picowjs_rtc_get_time() {
  datetime_t datetime;
  rtc_get_datetime(&datetime);
  struct tm ts;
  ts.tm_sec = datetime.sec;
  ts.tm_min = datetime.min;
  ts.tm_hour = datetime.hour;
  ts.tm_mday = datetime.day;
  ts.tm_mon = datetime.month - 1;
  ts.tm_year = datetime.year - 1900;
  time_t tsec = mktime(&ts);
  return (uint64_t)(tsec * 1000);
}
