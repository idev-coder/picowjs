/* Copyright (c) 2024 Pico-W-JS
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __PWJS_SYSTEM_H
#define __PWJS_SYSTEM_H

#include <stdint.h>

#include "io.h"

/**
 * Initialize the system
 */
void pwjs_system_init();

/**
 * Cleanup all resources in the system
 */
void pwjs_system_cleanup();

/**
 * Delay in milliseconds
 *
 * @param {uint32_t} msec
 */
void pwjs_delay(uint32_t msec);

/**
 * Return current time (UNIX timestamp in milliseconds)
 */
uint64_t pwjs_gettime();

/**
 * Return uid of device
 */
char *pwjs_getuid();

/**
 * Return MAX of the microsecond counter
 * Use this value to detect counter overflow
 */
uint64_t pwjs_micro_maxtime(void);

/**
 * Return microsecond counter
 */
uint64_t pwjs_micro_gettime(void);

/**
 * micro microsecond
 */
void pwjs_micro_delay(uint32_t usec);

/**
 * check script running mode - skipping or running user script
 */
uint8_t pwjs_running_script_check();

/**
 * custom process which need to be run in the infinite loop
 */
void pwjs_custom_infinite_loop();
#endif /* __PWJS_SYSTEM_H */
