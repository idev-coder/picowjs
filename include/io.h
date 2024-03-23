#ifndef ___PICOWJS_IO_H
#define ___PICOWJS_IO_H

#include <stdbool.h>
#include <stdint.h>

#include "jerryscript.h"
#include "utils.h"

typedef struct picowjs_io_loop_s picowjs_io_loop_t;
typedef struct picowjs_io_handle_s picowjs_io_handle_t;
typedef struct picowjs_io_handle_list_s picowjs_io_handle_list_t;
typedef struct picowjs_io_timer_handle_s picowjs_io_timer_handle_t;
typedef struct picowjs_io_tty_handle_s picowjs_io_tty_handle_t;
typedef struct picowjs_io_watch_handle_s picowjs_io_watch_handle_t;
typedef struct picowjs_io_uart_handle_s picowjs_io_uart_handle_t;
typedef struct picowjs_io_idle_handle_s picowjs_io_idle_handle_t;
typedef struct picowjs_io_stream_handle_s picowjs_io_stream_handle_t;

/* handle flags */

#define PICOWJS_IO_FLAG_ACTIVE 0x01
#define PICOWJS_IO_FLAG_CLOSING 0x02

#define PICOWJS_IO_SET_FLAG_ON(field, flag) ((field) |= (flag))
#define PICOWJS_IO_SET_FLAG_OFF(field, flag) ((field) &= ~(flag))
#define PICOWJS_IO_HAS_FLAG(field, flag) ((field) & (flag))

/* general handle types */

typedef enum picowjs_io_type {
  PICOWJS_IO_TIMER,
  PICOWJS_IO_TTY,
  PICOWJS_IO_WATCH,
  PICOWJS_IO_UART,
  PICOWJS_IO_IDLE,
  PICOWJS_IO_STREAM
} picowjs_io_type_t;

typedef void (*picowjs_io_close_cb)(picowjs_io_handle_t *);

struct picowjs_io_handle_s {
  picowjs_list_node_t base;
  uint32_t id;
  picowjs_io_type_t type;
  uint8_t flags;
  picowjs_io_close_cb close_cb;
};

/* timer handle types */

typedef void (*picowjs_io_timer_cb)(picowjs_io_timer_handle_t *);

struct picowjs_io_timer_handle_s {
  picowjs_io_handle_t base;
  picowjs_io_timer_cb timer_cb;
  jerry_value_t timer_js_cb;
  uint64_t clamped_timeout;
  uint64_t interval;
  bool repeat;
  uint32_t tag;  // for application use
};

/* TTY handle types */

typedef void (*picowjs_io_tty_read_cb)(uint8_t *, size_t);

struct picowjs_io_tty_handle_s {
  picowjs_io_handle_t base;
  picowjs_io_tty_read_cb read_cb;
};

/* GPIO watch handle types */

typedef enum {
  PICOWJS_IO_WATCH_MODE_LOW_LEVEL = 1,   // BIT0
  PICOWJS_IO_WATCH_MODE_HIGH_LEVEL = 2,  // BIT1
  PICOWJS_IO_WATCH_MODE_FALLING = 4,     // BIT2
  PICOWJS_IO_WATCH_MODE_RISING = 8,      // BIT3
  PICOWJS_IO_WATCH_MODE_CHANGE = 12,     // BIT2 | BIT3
} picowjs_io_watch_mode_t;

typedef void (*picowjs_io_watch_cb)(picowjs_io_watch_handle_t *);

struct picowjs_io_watch_handle_s {
  picowjs_io_handle_t base;
  picowjs_io_watch_mode_t mode;
  uint8_t pin;
  uint64_t debounce_time;
  uint32_t debounce_delay;
  uint8_t last_val;
  uint8_t val;
  picowjs_io_watch_cb watch_cb;
  jerry_value_t watch_js_cb;
};

/* UART handle type */

typedef int (*picowjs_io_uart_available_cb)(picowjs_io_uart_handle_t *);
typedef void (*picowjs_io_uart_read_cb)(picowjs_io_uart_handle_t *, uint8_t *, size_t);

struct picowjs_io_uart_handle_s {
  picowjs_io_handle_t base;
  uint8_t port;
  picowjs_io_uart_available_cb available_cb;
  picowjs_io_uart_read_cb read_cb;
  jerry_value_t read_js_cb;
};

/* idle handle types */

typedef void (*picowjs_io_idle_cb)(picowjs_io_idle_handle_t *);

struct picowjs_io_idle_handle_s {
  picowjs_io_handle_t base;
  picowjs_io_idle_cb idle_cb;
};

/* stream handle type */

typedef int (*picowjs_io_stream_available_cb)(picowjs_io_stream_handle_t *);
typedef void (*picowjs_io_stream_read_cb)(picowjs_io_stream_handle_t *, uint8_t *,
                                     size_t);

struct picowjs_io_stream_handle_s {
  picowjs_io_handle_t base;
  bool blocking;
  picowjs_io_stream_available_cb available_cb;
  picowjs_io_stream_read_cb read_cb;
};

/* loop type */

struct picowjs_io_loop_s {
  bool stop_flag;
  uint64_t time;
  picowjs_list_t timer_handles;
  picowjs_list_t tty_handles;
  picowjs_list_t watch_handles;
  picowjs_list_t uart_handles;
  picowjs_list_t idle_handles;
  picowjs_list_t stream_handles;
  picowjs_list_t closing_handles;
};

/* loop functions */

void picowjs_io_init();
void picowjs_io_cleanup();
void picowjs_io_run(bool infinite);

/* general handle functions */

void picowjs_io_handle_init(picowjs_io_handle_t *handle, picowjs_io_type_t type);
void picowjs_io_handle_close(picowjs_io_handle_t *handle, picowjs_io_close_cb close_cb);
picowjs_io_handle_t *picowjs_io_handle_get_by_id(uint32_t id, picowjs_list_t *handle_list);

/* timer functions */

void picowjs_io_timer_init(picowjs_io_timer_handle_t *timer);
void picowjs_io_timer_start(picowjs_io_timer_handle_t *timer, picowjs_io_timer_cb timer_cb,
                       uint64_t interval, bool repeat);
void picowjs_io_timer_stop(picowjs_io_timer_handle_t *timer);
picowjs_io_timer_handle_t *picowjs_io_timer_get_by_id(uint32_t id);
void picowjs_io_timer_cleanup();

/* TTY functions */

void picowjs_io_tty_init(picowjs_io_tty_handle_t *tty);
void picowjs_io_tty_read_start(picowjs_io_tty_handle_t *tty, picowjs_io_tty_read_cb read_cb);
void picowjs_io_tty_read_stop(picowjs_io_tty_handle_t *tty);
void picowjs_io_tty_cleanup();

/* GPIO watch functions */

void picowjs_io_watch_init(picowjs_io_watch_handle_t *watch);
void picowjs_io_watch_start(picowjs_io_watch_handle_t *watch, picowjs_io_watch_cb watch_cb,
                       uint8_t pin, picowjs_io_watch_mode_t mode, uint32_t debounce);
void picowjs_io_watch_stop(picowjs_io_watch_handle_t *watch);
picowjs_io_watch_handle_t *picowjs_io_watch_get_by_id(uint32_t id);
void picowjs_io_watch_cleanup();

/* UART function */

void picowjs_io_uart_init(picowjs_io_uart_handle_t *uart);
void picowjs_io_uart_read_start(picowjs_io_uart_handle_t *uart, uint8_t port,
                           picowjs_io_uart_available_cb available_cb,
                           picowjs_io_uart_read_cb read_cb);
void picowjs_io_uart_read_stop(picowjs_io_uart_handle_t *uart);
picowjs_io_uart_handle_t *picowjs_io_uart_get_by_id(uint32_t id);
void picowjs_io_uart_cleanup();

/* idle function */

void picowjs_io_idle_init(picowjs_io_idle_handle_t *idle);
void picowjs_io_idle_start(picowjs_io_idle_handle_t *idle, picowjs_io_idle_cb idle_cb);
void picowjs_io_idle_stop(picowjs_io_idle_handle_t *idle);
picowjs_io_idle_handle_t *picowjs_io_idle_get_by_id(uint32_t id);
void picowjs_io_idle_cleanup();

/* stream functions */

void picowjs_io_stream_init(picowjs_io_stream_handle_t *stream);
void picowjs_io_stream_set_blocking(picowjs_io_stream_handle_t *stream, bool blocking);
void picowjs_io_stream_read_start(picowjs_io_stream_handle_t *stream,
                             picowjs_io_stream_available_cb available_cb,
                             picowjs_io_stream_read_cb read_cb);
void picowjs_io_stream_read_stop(picowjs_io_stream_handle_t *stream);
void picowjs_io_stream_cleanup();
// int picowjs_io_stream_is_readable(picowjs_io_stream_handle_t *stream);
// int picowjs_io_stream_read(picowjs_io_stream_handle_t *stream);
// void picowjs_io_stream_push(picowjs_io_stream_handle_t *stream, uint8_t *buffer, size_t
// size); // push to read buffer

#endif /* ___PICOWJS_IO_H */
