#include "io.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "gpio.h"
#include "system.h"
#include "tty.h"
#include "uart.h"

picowjs_io_loop_t loop;

/* forward declarations */

static void picowjs_io_timer_run();
static void picowjs_io_tty_run();
static void picowjs_io_watch_run();
static void picowjs_io_uart_run();
static void picowjs_io_idle_run();
static void picowjs_io_idle_run();

/* general handle functions */

uint32_t handle_id_count = 0;

void picowjs_io_handle_init(picowjs_io_handle_t *handle, picowjs_io_type_t type) {
  handle->id = handle_id_count++;
  handle->type = type;
  handle->flags = 0;
  handle->close_cb = NULL;
}

void picowjs_io_handle_close(picowjs_io_handle_t *handle, picowjs_io_close_cb close_cb) {
  PICOWJS_IO_SET_FLAG_ON(handle->flags, PICOWJS_IO_FLAG_CLOSING);
  handle->close_cb = close_cb;
  picowjs_list_append(&loop.closing_handles, (picowjs_list_node_t *)handle);
}

picowjs_io_handle_t *picowjs_io_handle_get_by_id(uint32_t id, picowjs_list_t *handle_list) {
  picowjs_io_handle_t *handle = (picowjs_io_handle_t *)handle_list->head;
  while (handle != NULL) {
    if (handle->id == id) {
      return handle;
    }
    handle = (picowjs_io_handle_t *)((picowjs_list_node_t *)handle)->next;
  }
  return NULL;
}

static void picowjs_io_update_time() { loop.time = picowjs_gettime(); }

static void picowjs_io_handle_closing() {
  while (loop.closing_handles.head != NULL) {
    picowjs_io_handle_t *handle = (picowjs_io_handle_t *)loop.closing_handles.head;
    picowjs_list_remove(&loop.closing_handles, (picowjs_list_node_t *)handle);
    if (handle->close_cb) {
      handle->close_cb(handle);
    }
  }
}

/* loop functions */

void picowjs_io_init() {
  loop.stop_flag = false;
  picowjs_io_update_time();
  picowjs_list_init(&loop.tty_handles);
  picowjs_list_init(&loop.timer_handles);
  picowjs_list_init(&loop.watch_handles);
  picowjs_list_init(&loop.uart_handles);
  picowjs_list_init(&loop.idle_handles);
  picowjs_list_init(&loop.stream_handles);
  picowjs_list_init(&loop.closing_handles);
}

void picowjs_io_cleanup() {
  picowjs_io_timer_cleanup();
  picowjs_io_watch_cleanup();
  picowjs_io_uart_cleanup();
  // picowjs_io_idle_cleanup();
  // Do not cleanup tty I/O to keep terminal communication
  picowjs_io_stream_cleanup();
}

void picowjs_io_run(bool infinite) {
  while (loop.stop_flag == false) {
    picowjs_io_update_time();
    picowjs_io_timer_run();
    picowjs_io_tty_run();
    picowjs_io_watch_run();
    picowjs_io_uart_run();
    picowjs_io_idle_run();
    picowjs_io_handle_closing();
    picowjs_custom_infinite_loop();

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

void picowjs_io_timer_init(picowjs_io_timer_handle_t *timer) {
  picowjs_io_handle_init((picowjs_io_handle_t *)timer, PICOWJS_IO_TIMER);
  timer->timer_cb = NULL;
}

void picowjs_io_timer_start(picowjs_io_timer_handle_t *timer, picowjs_io_timer_cb timer_cb,
                       uint64_t interval, bool repeat) {
  PICOWJS_IO_SET_FLAG_ON(timer->base.flags, PICOWJS_IO_FLAG_ACTIVE);
  timer->timer_cb = timer_cb;
  timer->clamped_timeout = loop.time + interval;
  timer->interval = interval;
  timer->repeat = repeat;
  picowjs_list_append(&loop.timer_handles, (picowjs_list_node_t *)timer);
}

void picowjs_io_timer_stop(picowjs_io_timer_handle_t *timer) {
  PICOWJS_IO_SET_FLAG_OFF(timer->base.flags, PICOWJS_IO_FLAG_ACTIVE);
  picowjs_list_remove(&loop.timer_handles, (picowjs_list_node_t *)timer);
}

picowjs_io_timer_handle_t *picowjs_io_timer_get_by_id(uint32_t id) {
  return (picowjs_io_timer_handle_t *)picowjs_io_handle_get_by_id(id,
                                                        &loop.timer_handles);
}

void picowjs_io_timer_cleanup() {
  picowjs_io_timer_handle_t *handle =
      (picowjs_io_timer_handle_t *)loop.timer_handles.head;
  while (handle != NULL) {
    picowjs_io_timer_handle_t *next =
        (picowjs_io_timer_handle_t *)((picowjs_list_node_t *)handle)->next;
    free(handle);
    handle = next;
  }
  picowjs_list_init(&loop.timer_handles);
}

static void picowjs_io_timer_run() {
  picowjs_io_timer_handle_t *handle =
      (picowjs_io_timer_handle_t *)loop.timer_handles.head;
  while (handle != NULL) {
    if (PICOWJS_IO_HAS_FLAG(handle->base.flags, PICOWJS_IO_FLAG_ACTIVE)) {
      if (handle->clamped_timeout < loop.time) {
        if (handle->repeat) {
          handle->clamped_timeout = handle->clamped_timeout + handle->interval;
        } else {
          PICOWJS_IO_SET_FLAG_OFF(handle->base.flags, PICOWJS_IO_FLAG_ACTIVE);
        }
        if (handle->timer_cb) {
          handle->timer_cb(handle);
        }
      }
    }
    handle = (picowjs_io_timer_handle_t *)((picowjs_list_node_t *)handle)->next;
  }
}

/* TTY functions */

void picowjs_io_tty_init(picowjs_io_tty_handle_t *tty) {
  picowjs_io_handle_init((picowjs_io_handle_t *)tty, PICOWJS_IO_TTY);
  tty->read_cb = NULL;
}

void picowjs_io_tty_read_start(picowjs_io_tty_handle_t *tty, picowjs_io_tty_read_cb read_cb) {
  PICOWJS_IO_SET_FLAG_ON(tty->base.flags, PICOWJS_IO_FLAG_ACTIVE);
  tty->read_cb = read_cb;
  picowjs_list_append(&loop.tty_handles, (picowjs_list_node_t *)tty);
}

void picowjs_io_tty_read_stop(picowjs_io_tty_handle_t *tty) {
  PICOWJS_IO_SET_FLAG_OFF(tty->base.flags, PICOWJS_IO_FLAG_ACTIVE);
  picowjs_list_remove(&loop.tty_handles, (picowjs_list_node_t *)tty);
}

void picowjs_io_tty_cleanup() {
  picowjs_io_tty_handle_t *handle = (picowjs_io_tty_handle_t *)loop.tty_handles.head;
  while (handle != NULL) {
    picowjs_io_tty_handle_t *next =
        (picowjs_io_tty_handle_t *)((picowjs_list_node_t *)handle)->next;
    free(handle);
    handle = next;
  }
  picowjs_list_init(&loop.tty_handles);
}

static void picowjs_io_tty_run() {
  picowjs_io_tty_handle_t *handle = (picowjs_io_tty_handle_t *)loop.tty_handles.head;
  while (handle != NULL) {
    if (PICOWJS_IO_HAS_FLAG(handle->base.flags, PICOWJS_IO_FLAG_ACTIVE)) {
      uint32_t len = picowjs_tty_available();
      if (handle->read_cb != NULL && len > 0) {
        // for (int i = 0; i < size; i++) {
        //   handle->read_cb(picowjs_tty_getc());
        //}
        uint8_t buf[len];
        picowjs_tty_read(buf, len);
        handle->read_cb(buf, len);
      }
    }
    handle = (picowjs_io_tty_handle_t *)((picowjs_list_node_t *)handle)->next;
  }
}

/* GPIO watch functions */

void picowjs_io_watch_init(picowjs_io_watch_handle_t *watch) {
  picowjs_io_handle_init((picowjs_io_handle_t *)watch, PICOWJS_IO_WATCH);
  watch->watch_cb = NULL;
}

void picowjs_io_watch_start(picowjs_io_watch_handle_t *watch, picowjs_io_watch_cb watch_cb,
                       uint8_t pin, picowjs_io_watch_mode_t mode,
                       uint32_t debounce) {
  PICOWJS_IO_SET_FLAG_ON(watch->base.flags, PICOWJS_IO_FLAG_ACTIVE);
  watch->watch_cb = watch_cb;
  watch->pin = pin;
  watch->mode = mode;
  watch->debounce_time = 0;
  watch->debounce_delay = debounce;
  watch->last_val = (uint8_t)picowjs_gpio_read(watch->pin);
  watch->val = (uint8_t)picowjs_gpio_read(watch->pin);
  picowjs_list_append(&loop.watch_handles, (picowjs_list_node_t *)watch);
}

void picowjs_io_watch_stop(picowjs_io_watch_handle_t *watch) {
  PICOWJS_IO_SET_FLAG_OFF(watch->base.flags, PICOWJS_IO_FLAG_ACTIVE);
  picowjs_list_remove(&loop.watch_handles, (picowjs_list_node_t *)watch);
}

picowjs_io_watch_handle_t *picowjs_io_watch_get_by_id(uint32_t id) {
  return (picowjs_io_watch_handle_t *)picowjs_io_handle_get_by_id(id,
                                                        &loop.watch_handles);
}

void picowjs_io_watch_cleanup() {
  picowjs_io_watch_handle_t *handle =
      (picowjs_io_watch_handle_t *)loop.watch_handles.head;
  while (handle != NULL) {
    picowjs_io_watch_handle_t *next =
        (picowjs_io_watch_handle_t *)((picowjs_list_node_t *)handle)->next;
    free(handle);
    handle = next;
  }
  picowjs_list_init(&loop.watch_handles);
}

static void picowjs_io_watch_run() {
  picowjs_io_watch_handle_t *handle =
      (picowjs_io_watch_handle_t *)loop.watch_handles.head;
  while (handle != NULL) {
    if (PICOWJS_IO_HAS_FLAG(handle->base.flags, PICOWJS_IO_FLAG_ACTIVE)) {
      uint8_t reading = (uint8_t)picowjs_gpio_read(handle->pin);
      if (handle->last_val != reading) { /* changed by noise or pressing */
        handle->debounce_time = picowjs_gettime();
      }
      /* debounce delay elapsed */
      uint32_t elapsed_time = picowjs_gettime() - handle->debounce_time;
      if ((handle->watch_cb) &&
          (((handle->mode == PICOWJS_IO_WATCH_MODE_LOW_LEVEL) && (reading == 0)) ||
           ((handle->mode == PICOWJS_IO_WATCH_MODE_HIGH_LEVEL) && (reading == 1)))) {
        handle->watch_cb(handle);
      } else if (handle->debounce_time > 0 &&
                 elapsed_time >= handle->debounce_delay) {
        if (reading != handle->val) {
          handle->val = reading;
          switch (handle->mode) {
            case PICOWJS_IO_WATCH_MODE_CHANGE:
              if (handle->watch_cb) {
                handle->watch_cb(handle);
              }
              break;
            case PICOWJS_IO_WATCH_MODE_RISING:
              if (handle->val == 1 && handle->watch_cb) {
                handle->watch_cb(handle);
              }
              break;
            case PICOWJS_IO_WATCH_MODE_FALLING:
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
    handle = (picowjs_io_watch_handle_t *)((picowjs_list_node_t *)handle)->next;
  }
}

/* UART functions */

void picowjs_io_uart_init(picowjs_io_uart_handle_t *uart) {
  picowjs_io_handle_init((picowjs_io_handle_t *)uart, PICOWJS_IO_UART);
}

void picowjs_io_uart_read_start(picowjs_io_uart_handle_t *uart, uint8_t port,
                           picowjs_io_uart_available_cb available_cb,
                           picowjs_io_uart_read_cb read_cb) {
  PICOWJS_IO_SET_FLAG_ON(uart->base.flags, PICOWJS_IO_FLAG_ACTIVE);
  uart->port = port;
  uart->available_cb = available_cb;
  uart->read_cb = read_cb;
  picowjs_list_append(&loop.uart_handles, (picowjs_list_node_t *)uart);
}

void picowjs_io_uart_read_stop(picowjs_io_uart_handle_t *uart) {
  PICOWJS_IO_SET_FLAG_OFF(uart->base.flags, PICOWJS_IO_FLAG_ACTIVE);
  picowjs_list_remove(&loop.uart_handles, (picowjs_list_node_t *)uart);
}

picowjs_io_uart_handle_t *picowjs_io_uart_get_by_id(uint32_t id) {
  return (picowjs_io_uart_handle_t *)picowjs_io_handle_get_by_id(id, &loop.uart_handles);
}

void picowjs_io_uart_cleanup() {
  picowjs_io_uart_handle_t *handle = (picowjs_io_uart_handle_t *)loop.uart_handles.head;
  while (handle != NULL) {
    picowjs_io_uart_handle_t *next =
        (picowjs_io_uart_handle_t *)((picowjs_list_node_t *)handle)->next;
    free(handle);
    handle = next;
  }
  picowjs_list_init(&loop.uart_handles);
}

static void picowjs_io_uart_run() {
  picowjs_io_uart_handle_t *handle = (picowjs_io_uart_handle_t *)loop.uart_handles.head;
  while (handle != NULL) {
    if (PICOWJS_IO_HAS_FLAG(handle->base.flags, PICOWJS_IO_FLAG_ACTIVE)) {
      if (handle->available_cb != NULL && handle->read_cb != NULL) {
        int len = handle->available_cb(handle);
        if (len > 0) {
          uint8_t buf[len];
          picowjs_uart_read(handle->port, buf, len);
          handle->read_cb(handle, buf, len);
        }
      }
    }
    handle = (picowjs_io_uart_handle_t *)((picowjs_list_node_t *)handle)->next;
  }
}

/* idle functions */

void picowjs_io_idle_init(picowjs_io_idle_handle_t *idle) {
  picowjs_io_handle_init((picowjs_io_handle_t *)idle, PICOWJS_IO_IDLE);
  idle->idle_cb = NULL;
}

void picowjs_io_idle_start(picowjs_io_idle_handle_t *idle, picowjs_io_idle_cb idle_cb) {
  PICOWJS_IO_SET_FLAG_ON(idle->base.flags, PICOWJS_IO_FLAG_ACTIVE);
  idle->idle_cb = idle_cb;
  picowjs_list_append(&loop.idle_handles, (picowjs_list_node_t *)idle);
}

void picowjs_io_idle_stop(picowjs_io_idle_handle_t *idle) {
  PICOWJS_IO_SET_FLAG_OFF(idle->base.flags, PICOWJS_IO_FLAG_ACTIVE);
  picowjs_list_remove(&loop.idle_handles, (picowjs_list_node_t *)idle);
}

picowjs_io_idle_handle_t *picowjs_io_idle_get_by_id(uint32_t id) {
  return (picowjs_io_idle_handle_t *)picowjs_io_handle_get_by_id(id, &loop.idle_handles);
}

void picowjs_io_idle_cleanup() {
  picowjs_io_idle_handle_t *handle = (picowjs_io_idle_handle_t *)loop.idle_handles.head;
  while (handle != NULL) {
    picowjs_io_idle_handle_t *next =
        (picowjs_io_idle_handle_t *)((picowjs_list_node_t *)handle)->next;
    free(handle);
    handle = next;
  }
  picowjs_list_init(&loop.idle_handles);
}

static void picowjs_io_idle_run() {
  picowjs_io_idle_handle_t *handle = (picowjs_io_idle_handle_t *)loop.idle_handles.head;
  while (handle != NULL) {
    if (PICOWJS_IO_HAS_FLAG(handle->base.flags, PICOWJS_IO_FLAG_ACTIVE)) {
      if (handle->idle_cb) {
        handle->idle_cb(handle);
      }
    }
    handle = (picowjs_io_idle_handle_t *)((picowjs_list_node_t *)handle)->next;
  }
}

/* stream function */

void picowjs_io_stream_init(picowjs_io_stream_handle_t *stream) {
  picowjs_io_handle_init((picowjs_io_handle_t *)stream, PICOWJS_IO_STREAM);
}

void picowjs_io_stream_set_blocking(picowjs_io_stream_handle_t *stream, bool blocking) {
  stream->blocking = blocking;
}

void picowjs_io_stream_read_start(picowjs_io_stream_handle_t *stream,
                             picowjs_io_stream_available_cb available_cb,
                             picowjs_io_stream_read_cb read_cb) {
  PICOWJS_IO_SET_FLAG_ON(stream->base.flags, PICOWJS_IO_FLAG_ACTIVE);
  stream->blocking = false;  // non-blocking
  stream->available_cb = available_cb;
  stream->read_cb = read_cb;
  picowjs_list_append(&loop.stream_handles, (picowjs_list_node_t *)stream);
}

void picowjs_io_stream_read_stop(picowjs_io_stream_handle_t *stream) {
  PICOWJS_IO_SET_FLAG_OFF(stream->base.flags, PICOWJS_IO_FLAG_ACTIVE);
  picowjs_list_remove(&loop.stream_handles, (picowjs_list_node_t *)stream);
}

void picowjs_io_stream_cleanup() {
  picowjs_io_stream_handle_t *handle =
      (picowjs_io_stream_handle_t *)loop.stream_handles.head;
  while (handle != NULL) {
    picowjs_io_stream_handle_t *next =
        (picowjs_io_stream_handle_t *)((picowjs_list_node_t *)handle)->next;
    free(handle);
    handle = next;
  }
  picowjs_list_init(&loop.stream_handles);
}

/*
static void picowjs_io_stream_run() {
  picowjs_io_stream_handle_t *handle =
      (picowjs_io_stream_handle_t *)loop.stream_handles.head;
  while (handle != NULL) {
    if (PICOWJS_IO_HAS_FLAG(handle->base.flags, PICOWJS_IO_FLAG_ACTIVE)) {
      if (!handle->blocking && handle->available_cb != NULL &&
          handle->read_cb != NULL) {
        int len = handle->available_cb(handle);
        if (len > 0) {
          uint8_t buf[len];
          // picowjs_stream_read(handle->port, buf, len);
          handle->read_cb(handle, buf, len);
        }
      }
    }
    handle = (picowjs_io_stream_handle_t *)((picowjs_list_node_t *)handle)->next;
  }
}
*/