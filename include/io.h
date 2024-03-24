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

#ifndef ___PWJS_IO_H
#define ___PWJS_IO_H

#include <stdbool.h>
#include <stdint.h>

#include "jerryscript.h"
#include "utils.h"

typedef struct pwjs_io_loop_s pwjs_io_loop_t;
typedef struct pwjs_io_handle_s pwjs_io_handle_t;
typedef struct pwjs_io_handle_list_s pwjs_io_handle_list_t;
typedef struct pwjs_io_timer_handle_s pwjs_io_timer_handle_t;
typedef struct pwjs_io_tty_handle_s pwjs_io_tty_handle_t;
typedef struct pwjs_io_watch_handle_s pwjs_io_watch_handle_t;
typedef struct pwjs_io_uart_handle_s pwjs_io_uart_handle_t;
typedef struct pwjs_io_idle_handle_s pwjs_io_idle_handle_t;
typedef struct pwjs_io_stream_handle_s pwjs_io_stream_handle_t;

/* handle flags */

#define PWJS_IO_FLAG_ACTIVE 0x01
#define PWJS_IO_FLAG_CLOSING 0x02

#define PWJS_IO_SET_FLAG_ON(field, flag) ((field) |= (flag))
#define PWJS_IO_SET_FLAG_OFF(field, flag) ((field) &= ~(flag))
#define PWJS_IO_HAS_FLAG(field, flag) ((field) & (flag))

/* general handle types */

typedef enum pwjs_io_type {
  PWJS_IO_TIMER,
  PWJS_IO_TTY,
  PWJS_IO_WATCH,
  PWJS_IO_UART,
  PWJS_IO_IDLE,
  PWJS_IO_STREAM
} pwjs_io_type_t;

typedef void (*pwjs_io_close_cb)(pwjs_io_handle_t *);

struct pwjs_io_handle_s {
  pwjs_list_node_t base;
  uint32_t id;
  pwjs_io_type_t type;
  uint8_t flags;
  pwjs_io_close_cb close_cb;
};

/* timer handle types */

typedef void (*pwjs_io_timer_cb)(pwjs_io_timer_handle_t *);

struct pwjs_io_timer_handle_s {
  pwjs_io_handle_t base;
  pwjs_io_timer_cb timer_cb;
  jerry_value_t timer_js_cb;
  uint64_t clamped_timeout;
  uint64_t interval;
  bool repeat;
  uint32_t tag;  // for application use
};

/* TTY handle types */

typedef void (*pwjs_io_tty_read_cb)(uint8_t *, size_t);

struct pwjs_io_tty_handle_s {
  pwjs_io_handle_t base;
  pwjs_io_tty_read_cb read_cb;
};

/* GPIO watch handle types */

typedef enum {
  PWJS_IO_WATCH_MODE_LOW_LEVEL = 1,   // BIT0
  PWJS_IO_WATCH_MODE_HIGH_LEVEL = 2,  // BIT1
  PWJS_IO_WATCH_MODE_FALLING = 4,     // BIT2
  PWJS_IO_WATCH_MODE_RISING = 8,      // BIT3
  PWJS_IO_WATCH_MODE_CHANGE = 12,     // BIT2 | BIT3
} pwjs_io_watch_mode_t;

typedef void (*pwjs_io_watch_cb)(pwjs_io_watch_handle_t *);

struct pwjs_io_watch_handle_s {
  pwjs_io_handle_t base;
  pwjs_io_watch_mode_t mode;
  uint8_t pin;
  uint64_t debounce_time;
  uint32_t debounce_delay;
  uint8_t last_val;
  uint8_t val;
  pwjs_io_watch_cb watch_cb;
  jerry_value_t watch_js_cb;
};

/* UART handle type */

typedef int (*pwjs_io_uart_available_cb)(pwjs_io_uart_handle_t *);
typedef void (*pwjs_io_uart_read_cb)(pwjs_io_uart_handle_t *, uint8_t *, size_t);

struct pwjs_io_uart_handle_s {
  pwjs_io_handle_t base;
  uint8_t port;
  pwjs_io_uart_available_cb available_cb;
  pwjs_io_uart_read_cb read_cb;
  jerry_value_t read_js_cb;
};

/* idle handle types */

typedef void (*pwjs_io_idle_cb)(pwjs_io_idle_handle_t *);

struct pwjs_io_idle_handle_s {
  pwjs_io_handle_t base;
  pwjs_io_idle_cb idle_cb;
};

/* stream handle type */

typedef int (*pwjs_io_stream_available_cb)(pwjs_io_stream_handle_t *);
typedef void (*pwjs_io_stream_read_cb)(pwjs_io_stream_handle_t *, uint8_t *,
                                     size_t);

struct pwjs_io_stream_handle_s {
  pwjs_io_handle_t base;
  bool blocking;
  pwjs_io_stream_available_cb available_cb;
  pwjs_io_stream_read_cb read_cb;
};

/* loop type */

struct pwjs_io_loop_s {
  bool stop_flag;
  uint64_t time;
  pwjs_list_t timer_handles;
  pwjs_list_t tty_handles;
  pwjs_list_t watch_handles;
  pwjs_list_t uart_handles;
  pwjs_list_t idle_handles;
  pwjs_list_t stream_handles;
  pwjs_list_t closing_handles;
};

/* loop functions */

void pwjs_io_init();
void pwjs_io_cleanup();
void pwjs_io_run(bool infinite);

/* general handle functions */

void pwjs_io_handle_init(pwjs_io_handle_t *handle, pwjs_io_type_t type);
void pwjs_io_handle_close(pwjs_io_handle_t *handle, pwjs_io_close_cb close_cb);
pwjs_io_handle_t *pwjs_io_handle_get_by_id(uint32_t id, pwjs_list_t *handle_list);

/* timer functions */

void pwjs_io_timer_init(pwjs_io_timer_handle_t *timer);
void pwjs_io_timer_start(pwjs_io_timer_handle_t *timer, pwjs_io_timer_cb timer_cb,
                       uint64_t interval, bool repeat);
void pwjs_io_timer_stop(pwjs_io_timer_handle_t *timer);
pwjs_io_timer_handle_t *pwjs_io_timer_get_by_id(uint32_t id);
void pwjs_io_timer_cleanup();

/* TTY functions */

void pwjs_io_tty_init(pwjs_io_tty_handle_t *tty);
void pwjs_io_tty_read_start(pwjs_io_tty_handle_t *tty, pwjs_io_tty_read_cb read_cb);
void pwjs_io_tty_read_stop(pwjs_io_tty_handle_t *tty);
void pwjs_io_tty_cleanup();

/* GPIO watch functions */

void pwjs_io_watch_init(pwjs_io_watch_handle_t *watch);
void pwjs_io_watch_start(pwjs_io_watch_handle_t *watch, pwjs_io_watch_cb watch_cb,
                       uint8_t pin, pwjs_io_watch_mode_t mode, uint32_t debounce);
void pwjs_io_watch_stop(pwjs_io_watch_handle_t *watch);
pwjs_io_watch_handle_t *pwjs_io_watch_get_by_id(uint32_t id);
void pwjs_io_watch_cleanup();

/* UART function */

void pwjs_io_uart_init(pwjs_io_uart_handle_t *uart);
void pwjs_io_uart_read_start(pwjs_io_uart_handle_t *uart, uint8_t port,
                           pwjs_io_uart_available_cb available_cb,
                           pwjs_io_uart_read_cb read_cb);
void pwjs_io_uart_read_stop(pwjs_io_uart_handle_t *uart);
pwjs_io_uart_handle_t *pwjs_io_uart_get_by_id(uint32_t id);
void pwjs_io_uart_cleanup();

/* idle function */

void pwjs_io_idle_init(pwjs_io_idle_handle_t *idle);
void pwjs_io_idle_start(pwjs_io_idle_handle_t *idle, pwjs_io_idle_cb idle_cb);
void pwjs_io_idle_stop(pwjs_io_idle_handle_t *idle);
pwjs_io_idle_handle_t *pwjs_io_idle_get_by_id(uint32_t id);
void pwjs_io_idle_cleanup();

/* stream functions */

void pwjs_io_stream_init(pwjs_io_stream_handle_t *stream);
void pwjs_io_stream_set_blocking(pwjs_io_stream_handle_t *stream, bool blocking);
void pwjs_io_stream_read_start(pwjs_io_stream_handle_t *stream,
                             pwjs_io_stream_available_cb available_cb,
                             pwjs_io_stream_read_cb read_cb);
void pwjs_io_stream_read_stop(pwjs_io_stream_handle_t *stream);
void pwjs_io_stream_cleanup();
// int pwjs_io_stream_is_readable(pwjs_io_stream_handle_t *stream);
// int pwjs_io_stream_read(pwjs_io_stream_handle_t *stream);
// void pwjs_io_stream_push(pwjs_io_stream_handle_t *stream, uint8_t *buffer, size_t
// size); // push to read buffer

#endif /* ___PWJS_IO_H */
