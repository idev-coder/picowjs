#ifndef __PICOWJS_REPL_H
#define __PICOWJS_REPL_H

#include "board.h"
#include "jerryscript.h"
#include "jerryxx.h"
#include "utils.h"

typedef enum { PICOWJS_REPL_MODE_NORMAL, PICOWJS_REPL_MODE_ESCAPE } picowjs_repl_mode_t;

typedef enum {
  PICOWJS_REPL_OUTPUT_NORMAL,
  PICOWJS_REPL_OUTPUT_INFO,
  PICOWJS_REPL_OUTPUT_ERROR
} picowjs_repl_output_t;

typedef struct picowjs_repl_state_s picowjs_repl_state_t;
typedef struct picowjs_repl_command_s picowjs_repl_command_t;
typedef void (*picowjs_repl_handler_t)(picowjs_repl_state_t *, uint8_t *, size_t);

// repl state

struct picowjs_repl_state_s {
  picowjs_repl_mode_t mode;
  bool echo;
  picowjs_repl_handler_t handler;
  char buffer[KALUMA_REPL_BUFFER_SIZE + 1];
  unsigned int buffer_length;
  unsigned int position;
  unsigned int width;
  char escape[16];
  unsigned int escape_length;
  char *history[KALUMA_REPL_HISTORY_SIZE];
  unsigned int history_size;
  unsigned int history_position;
  uint8_t ymodem_state;  // 0=stopped, 1=transfering
  picowjs_list_t commands;
};

// repl commands

typedef void (*picowjs_repl_command_cb)(picowjs_repl_state_t *state, char *arg);

struct picowjs_repl_command_s {
  picowjs_list_node_t base;
  char name[8];   // max name is 7.
  char desc[32];  // max desc is 31.
  picowjs_repl_command_cb cb;
};

void picowjs_repl_init(bool hi);
void picowjs_repl_cleanup();
picowjs_repl_state_t *picowjs_get_repl_state();

void picowjs_repl_set_output(picowjs_repl_output_t output);
void picowjs_repl_print_prompt();
#define picowjs_repl_printf(format, args...) picowjs_tty_printf(format, ##args)
#define picowjs_repl_print_value(value) jerryxx_print_value(value)
#define picowjs_repl_putc(ch) picowjs_tty_putc(ch)
void picowjs_repl_pretty_print(uint8_t indent, uint8_t depth, jerry_value_t value);
void picowjs_repl_println();

void picowjs_repl_register_command(char *name, char *desc, picowjs_repl_command_cb cb);
void picowjs_repl_unregister_command(char *name);
void picowjs_repl_clear_commands();

#endif /* __PICOWJS_REPL_H */