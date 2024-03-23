#include "wdt.h"

#include <stdint.h>

#include "board.h"
#include "err.h"
#include "hardware/watchdog.h"
#define MAX_TIMEOUT 0x7fffff

/**
 * Enable watchdog timer
 *
 * @param {uint32_t} timeout, milliseconds
 * @return error code
 */
int picowjs_wdt_enable(bool en, uint32_t timeout_ms) {
    (void) en;
    if (timeout_ms > MAX_TIMEOUT) {
        timeout_ms = MAX_TIMEOUT;
    }
    watchdog_enable(timeout_ms, true);
    return 0;
}

/**
 * Feed (Kick) watch dog reset timer
 *
 */
void picowjs_wdt_feed(void) {
    watchdog_update();
}
