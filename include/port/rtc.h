#ifndef __PICOWJS_RTC_H
#define __PICOWJS_RTC_H

#include <stdbool.h>
#include <stdint.h>

void picowjs_rtc_init();
void picowjs_rtc_cleanup();

/**
 * Set RTC to the number of milliseconds since the Unix Epoch
 */
void picowjs_rtc_set_time(uint64_t time);

/**
 * Returns the number of milliseconds since the Unix Epoch
 */
uint64_t picowjs_rtc_get_time();

#endif /* __PICOWJS_RTC_H */
