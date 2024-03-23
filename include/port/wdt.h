#ifndef __PICOWJS_WDT_H
#define __PICOWJS_WDT_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Enable watchdog timer
 *
 * @param {bool} en
 * @param {uint32_t} timeout, milliseconds
 * @return error code
 */
int picowjs_wdt_enable(bool en, uint32_t timeout_ms);

/**
 * Feed (Kick) watch dog reset timer
 *
 */
void picowjs_wdt_feed(void);
#endif /* __PICOWJS_WDT_H */
