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

#ifndef __PWJS_REPL_H
#define __PWJS_REPL_H

#include "board.h"
#include "jerryscript.h"
#include "jerryxx.h"
#include "utils.h"

typedef enum { PWJS_REPL_MODE_NORMAL, PWJS_REPL_MODE_ESCAPE } pwjs_repl_mode_t;

typedef enum {
  PWJS_REPL_OUTPUT_NORMAL,
  PWJS_REPL_OUTPUT_INFO,
  PWJS_REPL_OUTPUT_ERROR
} pwjs_repl_output_t;

typedef struct pwjs_repl_state_s pwjs_repl_state_t;
typedef struct pwjs_repl_command_s pwjs_repl_command_t;
typedef void (*pwjs_repl_handler_t)(pwjs_repl_state_t *, uint8_t *, size_t);

// repl state

struct pwjs_repl_state_s {
  pwjs_repl_mode_t mode;
  bool echo;
  pwjs_repl_handler_t handler;
  char buffer[PICOWJS_REPL_BUFFER_SIZE + 1];
  unsigned int buffer_length;
  unsigned int position;
  unsigned int width;
  char escape[16];
  unsigned int escape_length;
  char *history[PICOWJS_REPL_HISTORY_SIZE];
  unsigned int history_size;
  unsigned int history_position;
  uint8_t ymodem_state;  // 0=stopped, 1=transfering
  pwjs_list_t commands;
};

// repl commands

typedef void (*pwjs_repl_command_cb)(pwjs_repl_state_t *state, char *arg);

struct pwjs_repl_command_s {
  pwjs_list_node_t base;
  char name[8];   // max name is 7.
  char desc[32];  // max desc is 31.
  pwjs_repl_command_cb cb;
};

void pwjs_repl_init(bool hi);
void pwjs_repl_cleanup();
pwjs_repl_state_t *pwjs_get_repl_state();

void pwjs_repl_set_output(pwjs_repl_output_t output);
void pwjs_repl_print_prompt();
#define pwjs_repl_printf(format, args...) pwjs_tty_printf(format, ##args)
#define pwjs_repl_print_value(value) jerryxx_print_value(value)
#define pwjs_repl_putc(ch) pwjs_tty_putc(ch)
void pwjs_repl_pretty_print(uint8_t indent, uint8_t depth, jerry_value_t value);
void pwjs_repl_println();

void pwjs_repl_register_command(char *name, char *desc, pwjs_repl_command_cb cb);
void pwjs_repl_unregister_command(char *name);
void pwjs_repl_clear_commands();

#endif /* __PWJS_REPL_H */