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

#include "io.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "gpio.h"
#include "system.h"
#include "tty.h"
#include "uart.h"

pwjs_io_loop_t loop;

/* forward declarations */

static void pwjs_io_timer_run();
static void pwjs_io_tty_run();
static void pwjs_io_watch_run();
static void pwjs_io_uart_run();
static void pwjs_io_idle_run();
static void pwjs_io_idle_run();

/* general handle functions */

uint32_t handle_id_count = 0;

void pwjs_io_handle_init(pwjs_io_handle_t *handle, pwjs_io_type_t type) {
  handle->id = handle_id_count++;
  handle->type = type;
  handle->flags = 0;
  handle->close_cb = NULL;
}

void pwjs_io_handle_close(pwjs_io_handle_t *handle, pwjs_io_close_cb close_cb) {
  PWJS_IO_SET_FLAG_ON(handle->flags, PWJS_IO_FLAG_CLOSING);
  handle->close_cb = close_cb;
  pwjs_list_append(&loop.closing_handles, (pwjs_list_node_t *)handle);
}

pwjs_io_handle_t *pwjs_io_handle_get_by_id(uint32_t id, pwjs_list_t *handle_list) {
  pwjs_io_handle_t *handle = (pwjs_io_handle_t *)handle_list->head;
  while (handle != NULL) {
    if (handle->id == id) {
      return handle;
    }
    handle = (pwjs_io_handle_t *)((pwjs_list_node_t *)handle)->next;
  }
  return NULL;
}

static void pwjs_io_update_time() { loop.time = pwjs_gettime(); }

static void pwjs_io_handle_closing() {
  while (loop.closing_handles.head != NULL) {
    pwjs_io_handle_t *handle = (pwjs_io_handle_t *)loop.closing_handles.head;
    pwjs_list_remove(&loop.closing_handles, (pwjs_list_node_t *)handle);
    if (handle->close_cb) {
      handle->close_cb(handle);
    }
  }
}

/* loop functions */

void pwjs_io_init() {
  loop.stop_flag = false;
  pwjs_io_update_time();
  pwjs_list_init(&loop.tty_handles);
  pwjs_list_init(&loop.timer_handles);
  pwjs_list_init(&loop.watch_handles);
  pwjs_list_init(&loop.uart_handles);
  pwjs_list_init(&loop.idle_handles);
  pwjs_list_init(&loop.stream_handles);
  pwjs_list_init(&loop.closing_handles);
}

void pwjs_io_cleanup() {
  pwjs_io_timer_cleanup();
  pwjs_io_watch_cleanup();
  pwjs_io_uart_cleanup();
  // pwjs_io_idle_cleanup();
  // Do not cleanup tty I/O to keep terminal communication
  pwjs_io_stream_cleanup();
}

void pwjs_io_run(bool infinite) {
  while (loop.stop_flag == false) {
    pwjs_io_update_time();
    pwjs_io_timer_run();
    pwjs_io_tty_run();
    pwjs_io_watch_run();
    pwjs_io_uart_run();
    pwjs_io_idle_run();
    pwjs_io_handle_closing();
    pwjs_custom_infinite_loop();

    // quite if there no IO handles
    if (!infinite) {
      if (loop.timer_handles.head == NULL && loop.watch_handles.head == NULL &&
          loop.uart_handles.head == NULL && loop.closing_handles.head == NULL) {
        loop.stop_flag = true;
      }
    }
  }
}

/* timer functions */

uint32_t timer_count = 0;

void pwjs_io_timer_init(pwjs_io_timer_handle_t *timer) {
  pwjs_io_handle_init((pwjs_io_handle_t *)timer, PWJS_IO_TIMER);
  timer->timer_cb = NULL;
}

void pwjs_io_timer_start(pwjs_io_timer_handle_t *timer, pwjs_io_timer_cb timer_cb,
                       uint64_t interval, bool repeat) {
  PWJS_IO_SET_FLAG_ON(timer->base.flags, PWJS_IO_FLAG_ACTIVE);
  timer->timer_cb = timer_cb;
  timer->clamped_timeout = loop.time + interval;
  timer->interval = interval;
  timer->repeat = repeat;
  pwjs_list_append(&loop.timer_handles, (pwjs_list_node_t *)timer);
}

void pwjs_io_timer_stop(pwjs_io_timer_handle_t *timer) {
  PWJS_IO_SET_FLAG_OFF(timer->base.flags, PWJS_IO_FLAG_ACTIVE);
  pwjs_list_remove(&loop.timer_handles, (pwjs_list_node_t *)timer);
}

pwjs_io_timer_handle_t *pwjs_io_timer_get_by_id(uint32_t id) {
  return (pwjs_io_timer_handle_t *)pwjs_io_handle_get_by_id(id,
                                                        &loop.timer_handles);
}

void pwjs_io_timer_cleanup() {
  pwjs_io_timer_handle_t *handle =
      (pwjs_io_timer_handle_t *)loop.timer_handles.head;
  while (handle != NULL) {
    pwjs_io_timer_handle_t *next =
        (pwjs_io_timer_handle_t *)((pwjs_list_node_t *)handle)->next;
    free(handle);
    handle = next;
  }
  pwjs_list_init(&loop.timer_handles);
}

static void pwjs_io_timer_run() {
  pwjs_io_timer_handle_t *handle =
      (pwjs_io_timer_handle_t *)loop.timer_handles.head;
  while (handle != NULL) {
    if (PWJS_IO_HAS_FLAG(handle->base.flags, PWJS_IO_FLAG_ACTIVE)) {
      if (handle->clamped_timeout < loop.time) {
        if (handle->repeat) {
          handle->clamped_timeout = handle->clamped_timeout + handle->interval;
        } else {
          PWJS_IO_SET_FLAG_OFF(handle->base.flags, PWJS_IO_FLAG_ACTIVE);
        }
        if (handle->timer_cb) {
          handle->timer_cb(handle);
        }
      }
    }
    handle = (pwjs_io_timer_handle_t *)((pwjs_list_node_t *)handle)->next;
  }
}

/* TTY functions */

void pwjs_io_tty_init(pwjs_io_tty_handle_t *tty) {
  pwjs_io_handle_init((pwjs_io_handle_t *)tty, PWJS_IO_TTY);
  tty->read_cb = NULL;
}

void pwjs_io_tty_read_start(pwjs_io_tty_handle_t *tty, pwjs_io_tty_read_cb read_cb) {
  PWJS_IO_SET_FLAG_ON(tty->base.flags, PWJS_IO_FLAG_ACTIVE);
  tty->read_cb = read_cb;
  pwjs_list_append(&loop.tty_handles, (pwjs_list_node_t *)tty);
}

void pwjs_io_tty_read_stop(pwjs_io_tty_handle_t *tty) {
  PWJS_IO_SET_FLAG_OFF(tty->base.flags, PWJS_IO_FLAG_ACTIVE);
  pwjs_list_remove(&loop.tty_handles, (pwjs_list_node_t *)tty);
}

void pwjs_io_tty_cleanup() {
  pwjs_io_tty_handle_t *handle = (pwjs_io_tty_handle_t *)loop.tty_handles.head;
  while (handle != NULL) {
    pwjs_io_tty_handle_t *next =
        (pwjs_io_tty_handle_t *)((pwjs_list_node_t *)handle)->next;
    free(handle);
    handle = next;
  }
  pwjs_list_init(&loop.tty_handles);
}

static void pwjs_io_tty_run() {
  pwjs_io_tty_handle_t *handle = (pwjs_io_tty_handle_t *)loop.tty_handles.head;
  while (handle != NULL) {
    if (PWJS_IO_HAS_FLAG(handle->base.flags, PWJS_IO_FLAG_ACTIVE)) {
      uint32_t len = pwjs_tty_available();
      if (handle->read_cb != NULL && len > 0) {
        // for (int i = 0; i < size; i++) {
        //   handle->read_cb(pwjs_tty_getc());
        //}
        uint8_t buf[len];
        pwjs_tty_read(buf, len);
        handle->read_cb(buf, len);
      }
    }
    handle = (pwjs_io_tty_handle_t *)((pwjs_list_node_t *)handle)->next;
  }
}

/* GPIO watch functions */

void pwjs_io_watch_init(pwjs_io_watch_handle_t *watch) {
  pwjs_io_handle_init((pwjs_io_handle_t *)watch, PWJS_IO_WATCH);
  watch->watch_cb = NULL;
}

void pwjs_io_watch_start(pwjs_io_watch_handle_t *watch, pwjs_io_watch_cb watch_cb,
                       uint8_t pin, pwjs_io_watch_mode_t mode,
                       uint32_t debounce) {
  PWJS_IO_SET_FLAG_ON(watch->base.flags, PWJS_IO_FLAG_ACTIVE);
  watch->watch_cb = watch_cb;
  watch->pin = pin;
  watch->mode = mode;
  watch->debounce_time = 0;
  watch->debounce_delay = debounce;
  watch->last_val = (uint8_t)pwjs_gpio_read(watch->pin);
  watch->val = (uint8_t)pwjs_gpio_read(watch->pin);
  pwjs_list_append(&loop.watch_handles, (pwjs_list_node_t *)watch);
}

void pwjs_io_watch_stop(pwjs_io_watch_handle_t *watch) {
  PWJS_IO_SET_FLAG_OFF(watch->base.flags, PWJS_IO_FLAG_ACTIVE);
  pwjs_list_remove(&loop.watch_handles, (pwjs_list_node_t *)watch);
}

pwjs_io_watch_handle_t *pwjs_io_watch_get_by_id(uint32_t id) {
  return (pwjs_io_watch_handle_t *)pwjs_io_handle_get_by_id(id,
                                                        &loop.watch_handles);
}

void pwjs_io_watch_cleanup() {
  pwjs_io_watch_handle_t *handle =
      (pwjs_io_watch_handle_t *)loop.watch_handles.head;
  while (handle != NULL) {
    pwjs_io_watch_handle_t *next =
        (pwjs_io_watch_handle_t *)((pwjs_list_node_t *)handle)->next;
    free(handle);
    handle = next;
  }
  pwjs_list_init(&loop.watch_handles);
}

static void pwjs_io_watch_run() {
  pwjs_io_watch_handle_t *handle =
      (pwjs_io_watch_handle_t *)loop.watch_handles.head;
  while (handle != NULL) {
    if (PWJS_IO_HAS_FLAG(handle->base.flags, PWJS_IO_FLAG_ACTIVE)) {
      uint8_t reading = (uint8_t)pwjs_gpio_read(handle->pin);
      if (handle->last_val != reading) { /* changed by noise or pressing */
        handle->debounce_time = pwjs_gettime();
      }
      /* debounce delay elapsed */
      uint32_t elapsed_time = pwjs_gettime() - handle->debounce_time;
      if ((handle->watch_cb) &&
          (((handle->mode == PWJS_IO_WATCH_MODE_LOW_LEVEL) && (reading == 0)) ||
           ((handle->mode == PWJS_IO_WATCH_MODE_HIGH_LEVEL) && (reading == 1)))) {
        handle->watch_cb(handle);
      } else if (handle->debounce_time > 0 &&
                 elapsed_time >= handle->debounce_delay) {
        if (reading != handle->val) {
          handle->val = reading;
          switch (handle->mode) {
            case PWJS_IO_WATCH_MODE_CHANGE:
              if (handle->watch_cb) {
                handle->watch_cb(handle);
              }
              break;
            case PWJS_IO_WATCH_MODE_RISING:
              if (handle->val == 1 && handle->watch_cb) {
                handle->watch_cb(handle);
              }
              break;
            case PWJS_IO_WATCH_MODE_FALLING:
              if (handle->val == 0 && handle->watch_cb) {
                handle->watch_cb(handle);
              }
              break;
            default:
              break;
          }
        }
        handle->debounce_time = 0;
      }
      handle->last_val = reading;
    }
    handle = (pwjs_io_watch_handle_t *)((pwjs_list_node_t *)handle)->next;
  }
}

/* UART functions */

void pwjs_io_uart_init(pwjs_io_uart_handle_t *uart) {
  pwjs_io_handle_init((pwjs_io_handle_t *)uart, PWJS_IO_UART);
}

void pwjs_io_uart_read_start(pwjs_io_uart_handle_t *uart, uint8_t port,
                           pwjs_io_uart_available_cb available_cb,
                           pwjs_io_uart_read_cb read_cb) {
  PWJS_IO_SET_FLAG_ON(uart->base.flags, PWJS_IO_FLAG_ACTIVE);
  uart->port = port;
  uart->available_cb = available_cb;
  uart->read_cb = read_cb;
  pwjs_list_append(&loop.uart_handles, (pwjs_list_node_t *)uart);
}

void pwjs_io_uart_read_stop(pwjs_io_uart_handle_t *uart) {
  PWJS_IO_SET_FLAG_OFF(uart->base.flags, PWJS_IO_FLAG_ACTIVE);
  pwjs_list_remove(&loop.uart_handles, (pwjs_list_node_t *)uart);
}

pwjs_io_uart_handle_t *pwjs_io_uart_get_by_id(uint32_t id) {
  return (pwjs_io_uart_handle_t *)pwjs_io_handle_get_by_id(id, &loop.uart_handles);
}

void pwjs_io_uart_cleanup() {
  pwjs_io_uart_handle_t *handle = (pwjs_io_uart_handle_t *)loop.uart_handles.head;
  while (handle != NULL) {
    pwjs_io_uart_handle_t *next =
        (pwjs_io_uart_handle_t *)((pwjs_list_node_t *)handle)->next;
    free(handle);
    handle = next;
  }
  pwjs_list_init(&loop.uart_handles);
}

static void pwjs_io_uart_run() {
  pwjs_io_uart_handle_t *handle = (pwjs_io_uart_handle_t *)loop.uart_handles.head;
  while (handle != NULL) {
    if (PWJS_IO_HAS_FLAG(handle->base.flags, PWJS_IO_FLAG_ACTIVE)) {
      if (handle->available_cb != NULL && handle->read_cb != NULL) {
        int len = handle->available_cb(handle);
        if (len > 0) {
          uint8_t buf[len];
          pwjs_uart_read(handle->port, buf, len);
          handle->read_cb(handle, buf, len);
        }
      }
    }
    handle = (pwjs_io_uart_handle_t *)((pwjs_list_node_t *)handle)->next;
  }
}

/* idle functions */

void pwjs_io_idle_init(pwjs_io_idle_handle_t *idle) {
  pwjs_io_handle_init((pwjs_io_handle_t *)idle, PWJS_IO_IDLE);
  idle->idle_cb = NULL;
}

void pwjs_io_idle_start(pwjs_io_idle_handle_t *idle, pwjs_io_idle_cb idle_cb) {
  PWJS_IO_SET_FLAG_ON(idle->base.flags, PWJS_IO_FLAG_ACTIVE);
  idle->idle_cb = idle_cb;
  pwjs_list_append(&loop.idle_handles, (pwjs_list_node_t *)idle);
}

void pwjs_io_idle_stop(pwjs_io_idle_handle_t *idle) {
  PWJS_IO_SET_FLAG_OFF(idle->base.flags, PWJS_IO_FLAG_ACTIVE);
  pwjs_list_remove(&loop.idle_handles, (pwjs_list_node_t *)idle);
}

pwjs_io_idle_handle_t *pwjs_io_idle_get_by_id(uint32_t id) {
  return (pwjs_io_idle_handle_t *)pwjs_io_handle_get_by_id(id, &loop.idle_handles);
}

void pwjs_io_idle_cleanup() {
  pwjs_io_idle_handle_t *handle = (pwjs_io_idle_handle_t *)loop.idle_handles.head;
  while (handle != NULL) {
    pwjs_io_idle_handle_t *next =
        (pwjs_io_idle_handle_t *)((pwjs_list_node_t *)handle)->next;
    free(handle);
    handle = next;
  }
  pwjs_list_init(&loop.idle_handles);
}

static void pwjs_io_idle_run() {
  pwjs_io_idle_handle_t *handle = (pwjs_io_idle_handle_t *)loop.idle_handles.head;
  while (handle != NULL) {
    if (PWJS_IO_HAS_FLAG(handle->base.flags, PWJS_IO_FLAG_ACTIVE)) {
      if (handle->idle_cb) {
        handle->idle_cb(handle);
      }
    }
    handle = (pwjs_io_idle_handle_t *)((pwjs_list_node_t *)handle)->next;
  }
}

/* stream function */

void pwjs_io_stream_init(pwjs_io_stream_handle_t *stream) {
  pwjs_io_handle_init((pwjs_io_handle_t *)stream, PWJS_IO_STREAM);
}

void pwjs_io_stream_set_blocking(pwjs_io_stream_handle_t *stream, bool blocking) {
  stream->blocking = blocking;
}

void pwjs_io_stream_read_start(pwjs_io_stream_handle_t *stream,
                             pwjs_io_stream_available_cb available_cb,
                             pwjs_io_stream_read_cb read_cb) {
  PWJS_IO_SET_FLAG_ON(stream->base.flags, PWJS_IO_FLAG_ACTIVE);
  stream->blocking = false;  // non-blocking
  stream->available_cb = available_cb;
  stream->read_cb = read_cb;
  pwjs_list_append(&loop.stream_handles, (pwjs_list_node_t *)stream);
}

void pwjs_io_stream_read_stop(pwjs_io_stream_handle_t *stream) {
  PWJS_IO_SET_FLAG_OFF(stream->base.flags, PWJS_IO_FLAG_ACTIVE);
  pwjs_list_remove(&loop.stream_handles, (pwjs_list_node_t *)stream);
}

void pwjs_io_stream_cleanup() {
  pwjs_io_stream_handle_t *handle =
      (pwjs_io_stream_handle_t *)loop.stream_handles.head;
  while (handle != NULL) {
    pwjs_io_stream_handle_t *next =
        (pwjs_io_stream_handle_t *)((pwjs_list_node_t *)handle)->next;
    free(handle);
    handle = next;
  }
  pwjs_list_init(&loop.stream_handles);
}

/*
static void pwjs_io_stream_run() {
  pwjs_io_stream_handle_t *handle =
      (pwjs_io_stream_handle_t *)loop.stream_handles.head;
  while (handle != NULL) {
    if (PWJS_IO_HAS_FLAG(handle->base.flags, PWJS_IO_FLAG_ACTIVE)) {
      if (!handle->blocking && handle->available_cb != NULL &&
          handle->read_cb != NULL) {
        int len = handle->available_cb(handle);
        if (len > 0) {
          uint8_t buf[len];
          // pwjs_stream_read(handle->port, buf, len);
          handle->read_cb(handle, buf, len);
        }
      }
    }
    handle = (pwjs_io_stream_handle_t *)((pwjs_list_node_t *)handle)->next;
  }
}
*/