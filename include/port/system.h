#ifndef __PICOWJS_SYSTEM_H
#define __PICOWJS_SYSTEM_H

#include <stdint.h>

#include "io.h"

/**
 * Initialize the system
 */
void picowjs_system_init();

/**
 * Cleanup all resources in the system
 */
void picowjs_system_cleanup();

/**
 * Delay in milliseconds
 *
 * @param {uint32_t} msec
 */
void picowjs_delay(uint32_t msec);

/**
 * Return current time (UNIX timestamp in milliseconds)
 */
uint64_t picowjs_gettime();

/**
 * Return uid of device
 */
char *picowjs_getuid();

/**
 * Return MAX of the microsecond counter
 * Use this value to detect counter overflow
 */
uint64_t picowjs_micro_maxtime(void);

/**
 * Return microsecond counter
 */
uint64_t picowjs_micro_gettime(void);

/**
 * micro microsecond
 */
void picowjs_micro_delay(uint32_t usec);

/**
 * check script running mode - skipping or running user script
 */
uint8_t picowjs_running_script_check();

/**
 * custom process which need to be run in the infinite loop
 */
void picowjs_custom_infinite_loop();
#endif /* __PICOWJS_SYSTEM_H */
