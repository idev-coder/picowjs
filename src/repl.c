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

#include "repl.h"

#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "jerryscript.h"
#include "picowjs_config.h"
#include "prog.h"
#include "runtime.h"
#include "system.h"
#include "tty.h"
#include "utils.h"
#include "ymodem.h"

// --------------------------------------------------------------------------
// FORWARD DECLARATIONS
// --------------------------------------------------------------------------

static void cmd_echo(pwjs_repl_state_t *state, char *arg);
static void cmd_reset(pwjs_repl_state_t *state, char *arg);
static void cmd_flash(pwjs_repl_state_t *state, char *arg);
static void cmd_load(pwjs_repl_state_t *state, char *arg);
static void cmd_mem(pwjs_repl_state_t *state, char *arg);
static void cmd_gc(pwjs_repl_state_t *state, char *arg);
static void cmd_hi(pwjs_repl_state_t *state, char *arg);
static void cmd_help(pwjs_repl_state_t *state, char *arg);

// --------------------------------------------------------------------------
// PRIVATE VARIABLES
// --------------------------------------------------------------------------

/**
 * TTY handle for REPL
 */
static pwjs_io_tty_handle_t tty;

/**
 * REPL state
 */
static pwjs_repl_state_t state;

// --------------------------------------------------------------------------
// PRIVATE FUNCTIONS
// --------------------------------------------------------------------------

/**
 * Inject a char to REPL
 */
static void tty_read_cb(uint8_t *buf, size_t len) {
  if (state.handler != NULL) {
    (*state.handler)(&state, buf, len); /* call handler */
  }
}

/**
 * Push a command to history
 */
static void history_push(char *cmd) {
  if (state.history_size < PICOWJS_REPL_HISTORY_SIZE) {
    state.history[state.history_size] = cmd;
    state.history_size++;
  } else {
    // free memory of history[0]
    free(state.history[0]);
    // Shift history array to left (e.g. 1 to 0, 2 to 1, ...)
    for (int i = 0; i < (state.history_size - 1); i++) {
      state.history[i] = state.history[i + 1];
    }
    // Put to the last of history
    state.history[state.history_size - 1] = cmd;
  }
  state.history_position = state.history_size;
}

/**
 * Reset the REPL commands
 */
static void reset_commands() {
  pwjs_repl_clear_commands();
  pwjs_repl_register_command(".help", "Print this help message", cmd_help);
  pwjs_repl_register_command(".hi", "Print welcome message", cmd_hi);
  pwjs_repl_register_command(".echo", "Echo on/off", cmd_echo);
  pwjs_repl_register_command(".reset", "Soft reset", cmd_reset);
  pwjs_repl_register_command(".flash", "Commands for flash", cmd_flash);
  pwjs_repl_register_command(".load", "Load code from flash", cmd_load);
  pwjs_repl_register_command(".mem", "Heap memory status", cmd_mem);
  pwjs_repl_register_command(".gc", "Perform garbage collection", cmd_gc);
}

/**
 * Run the REPL command in the buffer
 */
static void run_command() {
  if (state.buffer_length > 0) {
    /* copy buffer to data */
    char *data = malloc(state.buffer_length + 1);
    state.buffer[state.buffer_length] = '\0';
    strcpy(data, state.buffer);
    state.buffer_length = 0;
    state.position = 0;

    /* push to history */
    history_push(data);

    /* tokenize command */
    char *tokenv[5];
    unsigned int tokenc = 0;
    tokenv[0] = strtok(state.buffer, " ");
    while (tokenv[tokenc] != NULL && tokenc < 5) {
      tokenc++;
      tokenv[tokenc] = strtok(NULL, " ");
    }

    /* run command */
    pwjs_repl_command_t *cmd = (pwjs_repl_command_t *)state.commands.head;
    while (cmd != NULL) {
      pwjs_repl_command_t *next =
          (pwjs_repl_command_t *)((pwjs_list_node_t *)cmd)->next;
      if (strcmp(tokenv[0], cmd->name) == 0) {
        cmd->cb(&state, tokenv[1]);
        break;
      }
      cmd = next;
    }
    if (cmd == NULL) {  // unknown command
      pwjs_repl_set_output(PWJS_REPL_OUTPUT_ERROR);
      pwjs_repl_printf("Unknown command: %s\r\n", tokenv[0]);
      pwjs_repl_set_output(PWJS_REPL_OUTPUT_NORMAL);
    }
  } else {
    pwjs_repl_set_output(PWJS_REPL_OUTPUT_NORMAL);
  }
}

/**
 * Evaluate JS code in the buffer
 */
static void run_code() {
  if (state.buffer_length > 0) {
    /* copy buffer to data */
    char *data = malloc(state.buffer_length + 1);
    state.buffer[state.buffer_length] = '\0';
    strcpy(data, state.buffer);
    state.buffer_length = 0;
    state.position = 0;

    /* push to history */
    history_push(data);

    /* evaluate code */
    jerry_value_t parsed_code =
        jerry_parse(NULL, 0, (const jerry_char_t *)data, strlen(data),
                    JERRY_PARSE_STRICT_MODE);
    if (jerry_value_is_error(parsed_code)) {
      jerryxx_print_error(parsed_code, false);
    } else {
      jerry_value_t ret_value = jerry_run(parsed_code);
      if (jerry_value_is_error(ret_value)) {
        jerryxx_print_error(ret_value, false);
      } else {
        pwjs_repl_pretty_print(0, 3, ret_value);
        pwjs_repl_println();
      }
      jerry_release_value(ret_value);
    }
    jerry_release_value(parsed_code);
  }
}

/**
 * Move cursor to state.position in consideration with state.width
 */
static void set_cursor_to_position() {
  int horz = (state.position + 2) % state.width;
  int vert = (state.position + 2) / state.width;
  if (horz > 0) {
    if (vert > 0) {
      pwjs_repl_printf("\0338\033[%dC\033[%dB", horz, vert);
    } else {
      pwjs_repl_printf("\0338\033[%dC", horz);
    }
  } else {
    if (vert > 0) {
      pwjs_repl_printf("\0338\033[%dB", vert);
    } else {
      pwjs_repl_printf("\0338");
    }
  }
}

/**
 * Handler for normal mode
 */
static void handle_normal(char ch) {
  switch (ch) {
    case '\r': /* carrage return */
      if (state.echo) {
        pwjs_repl_printf("\r\n");
      }
      if (state.buffer_length > 0 && state.buffer[0] == '.') {
        run_command();
      } else {
        run_code();
      }
      pwjs_repl_print_prompt();
      break;
    case 0x01: /* Ctrl + A */
      state.position = 0;
      set_cursor_to_position();
      break;
    case 0x04: /* Ctrl + D */
      cmd_reset(&state, NULL);
      pwjs_repl_print_prompt();
      break;
    case 0x05: /* Ctrl + E */
      state.position = state.buffer_length;
      set_cursor_to_position();
      break;
    case 0x08: /* backspace */
    case 0x7f: /* also backspace in some terminal */
      if (state.buffer_length > 0 && state.position > 0) {
        state.position--;
        for (int i = state.position; i < state.buffer_length - 1; i++) {
          state.buffer[i] = state.buffer[i + 1];
        }
        state.buffer_length--;
        state.buffer[state.buffer_length] = '\0';
        if (state.echo) {
          pwjs_repl_printf("\033[D\033[K\033[J");
          pwjs_repl_printf("\0338> %s", state.buffer);
          set_cursor_to_position();
        }
      }
      break;
    case 0x1b: /* escape char */
      state.mode = PWJS_REPL_MODE_ESCAPE;
      state.escape_length = 0;
      break;
    default:
      // check buffer overflow
      if (state.buffer_length < (PICOWJS_REPL_BUFFER_SIZE - 1)) {
        if (state.position == state.buffer_length) {
          state.buffer[state.position] = ch;
          state.buffer_length++;
          state.position++;
          if (state.echo) {
            pwjs_repl_putc(ch);
          }
        } else {
          for (int i = state.buffer_length; i > state.position; i--) {
            state.buffer[i] = state.buffer[i - 1];
          }
          state.buffer[state.position] = ch;
          state.buffer_length++;
          state.position++;
          state.buffer[state.buffer_length] = '\0';
          if (state.echo) {
            pwjs_repl_printf("\0338> %s", state.buffer);
            set_cursor_to_position();
          }
        }
      } else {
        pwjs_repl_set_output(PWJS_REPL_OUTPUT_ERROR);
        pwjs_repl_printf("%s\r\n", "REPL buffer overflow");
        pwjs_repl_set_output(PWJS_REPL_OUTPUT_NORMAL);
      }
      break;
  }
}

/**
 * Handle char in escape sequence
 */
static void handle_escape(char ch) {
  state.escape[state.escape_length] = ch;
  state.escape_length++;

  // if ch is last char (a-zA-Z) of escape sequence
  if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
    state.mode = PWJS_REPL_MODE_NORMAL;

    /* up key */
    if (state.escape_length == 2 && state.escape[0] == 0x5b &&
        state.escape[1] == 0x41) {
      if (state.history_position > 0) {
        state.history_position--;
        char *cmd = state.history[state.history_position];
        pwjs_repl_printf("\33[2K\r> %s", cmd);
        strcpy(state.buffer, cmd);
        state.buffer_length = strlen(cmd);
        state.position = state.buffer_length;
      }

      /* down key */
    } else if (state.escape_length == 2 && state.escape[0] == 0x5b &&
               state.escape[1] == 0x42) {
      if (state.history_position == state.history_size) {
        /* do nothing */
      } else if (state.history_position == (state.history_size - 1)) {
        state.history_position++;
        pwjs_repl_printf("\33[2K\r> ");
        state.buffer_length = 0;
        state.position = 0;
      } else {
        state.history_position++;
        char *cmd = state.history[state.history_position];
        pwjs_repl_printf("\33[2K\r> %s", cmd);
        strcpy(state.buffer, cmd);
        state.buffer_length = strlen(cmd);
        state.position = state.buffer_length;
      }

      /* left key */
    } else if (state.escape_length == 2 && state.escape[0] == 0x5b &&
               state.escape[1] == 0x44) {
      if (state.position > 0) {
        state.position--;
        set_cursor_to_position();
      }

      /* right key */
    } else if (state.escape_length == 2 && state.escape[0] == 0x5b &&
               state.escape[1] == 0x43) {
      if (state.position < state.buffer_length) {
        state.position++;
        set_cursor_to_position();
      }

      /* receive cursor position and update screen width */
    } else if (state.escape[state.escape_length - 1] == 'R') {
      int pos = 0;
      for (int i = 0; i < state.escape_length; i++) {
        if (state.escape[i] == ';') {
          pos = i + 1;
          break;
        }
      }
      state.escape[state.escape_length - 1] = '\0';
      state.width = atoi(state.escape + pos);

      // Run original escape sequence
    } else {
      pwjs_repl_putc('\033');
      for (int i = 0; i < state.escape_length; i++) {
        pwjs_repl_putc(state.escape[i]);
      }
    }
  } else if (ch == 0x7e) { /* special key */
    state.mode = PWJS_REPL_MODE_NORMAL;

    /* delete key */
    if (state.escape_length == 3 && state.escape[0] == 0x5b &&
        state.escape[1] == 0x33) {
      if (state.buffer_length > 0 && state.position < state.buffer_length) {
        for (int i = state.position; i < state.buffer_length; i++) {
          state.buffer[i] = state.buffer[i + 1];
        }
        state.buffer_length--;
        state.buffer[state.buffer_length] = '\0';
        if (state.echo) {
          pwjs_repl_printf("\033[K\033[J");
          pwjs_repl_printf("\0338> %s", state.buffer);
          set_cursor_to_position();
        }
      }
    }
    state.escape_length = 0;
  }
}

/**
 * Default handler
 */
static void default_handler(pwjs_repl_state_t *state, uint8_t *buf, size_t len) {
  for (int i = 0; i < len; i++) {
    char ch = buf[i];
    switch (state->mode) {
      case PWJS_REPL_MODE_NORMAL:
        handle_normal(ch);
        break;
      case PWJS_REPL_MODE_ESCAPE:
        handle_escape(ch);
        break;
    }
  }
}

#if 0  // Never used.
/**
 * Change handler
 */
static void set_handler(pwjs_repl_handler_t handler) {
  if (handler != NULL) {
    state.handler = handler;
  } else {
    state.handler = &default_handler;
  }
}
#endif

/**
 * .echo command
 */
static void cmd_echo(pwjs_repl_state_t *state, char *arg) {
  if (strcmp(arg, "on") == 0) {
    state->echo = true;
  } else if (strcmp(arg, "off") == 0) {
    state->echo = false;
  }
}

/**
 * .reset command
 */
static void cmd_reset(pwjs_repl_state_t *state, char *arg) {
  pwjs_runtime_cleanup();
  reset_commands();
  pwjs_runtime_init(false, false);
  pwjs_repl_printf("\r\nsoft reset\r\n");
}

static size_t bytes_remained = 0;

static int header_cb(uint8_t *file_name, size_t file_size) {
  pwjs_prog_begin();
  bytes_remained = file_size;
  return 0;
}

static int packet_cb(uint8_t *data, size_t len) {
  if (bytes_remained < len) {
    len = bytes_remained;
    bytes_remained = 0;
  } else {
    bytes_remained = bytes_remained - len;
  }
  int ret = pwjs_prog_write(data, len);
  if (ret < 0) {
    return -1;
  }
  return 0;
}

static void footer_cb() {
  pwjs_prog_end();
  bytes_remained = 0;
}

/**
 * .flash command
 */
static void cmd_flash(pwjs_repl_state_t *state, char *arg) {
  /* erase flash */
  if (strcmp(arg, "-e") == 0) {
    pwjs_prog_clear();
    pwjs_repl_printf("Flash has erased\r\n");

    /* get total size of flash */
  } else if (strcmp(arg, "-t") == 0) {
    uint32_t size = pwjs_prog_max_size();
    pwjs_repl_printf("%u\r\n", size);

    /* get data size in flash */
  } else if (strcmp(arg, "-s") == 0) {
    uint32_t data_size = pwjs_prog_get_size();
    pwjs_repl_printf("%u\r\n", data_size);

    /* read data from flash */
  } else if (strcmp(arg, "-r") == 0) {
    uint32_t sz = pwjs_prog_get_size();
    uint8_t *ptr = pwjs_prog_addr();
    for (int i = 0; i < sz; i++) {
      if (ptr[i] == '\n') { /* convert "\n" to "\r\n" */
        pwjs_repl_putc('\r');
      }
      pwjs_repl_putc(ptr[i]);
    }
    pwjs_repl_println();
    /* write a file to flash via Ymodem */
  } else if (strcmp(arg, "-w") == 0) {
    state->ymodem_state = 1;  // transfering
    pwjs_tty_printf("Transfer a file via YMODEM... (press 'a' to abort)\r\n");
    pwjs_io_tty_read_stop(&tty);
    pwjs_ymodem_status_t result =
        pwjs_ymodem_receive(header_cb, packet_cb, footer_cb);
    pwjs_io_tty_read_start(&tty, tty_read_cb);
    pwjs_delay(500);
    switch (result) {
      case PWJS_YMODEM_OK:
        pwjs_tty_printf("\r\nDone\r\n");
        break;
      case PWJS_YMODEM_LIMIT:
        pwjs_tty_printf("\r\nThe file size is too large\r\n");
        break;
      case PWJS_YMODEM_DATA:
        pwjs_tty_printf("\r\nVerification failed\r\n");
        break;
      case PWJS_YMODEM_ABORT:
        pwjs_tty_printf("\r\nAborted\r\n");
        break;
      default:
        pwjs_tty_printf("\r\nFailed to receive\r\n");
        break;
    }
    state->ymodem_state = 0;  // stopped
    /* no option is given */
  } else {
    pwjs_repl_printf(".flash command options:\r\n");
    pwjs_repl_printf("-w\tWrite code to flash via YMODEM\r\n");
    pwjs_repl_printf("-e\tErase the code in flash\r\n");
    pwjs_repl_printf("-t\tPrint total size of flash for code\r\n");
    pwjs_repl_printf("-s\tPrint the size of the code in flash\r\n");
    pwjs_repl_printf("-r\tPrint the code in flash\r\n");
  }
}

/**
 * .load command
 */
static void cmd_load(pwjs_repl_state_t *state, char *arg) {
  pwjs_runtime_cleanup();
  pwjs_runtime_init(true, false);
}

/**
 * .mem command
 */
static void cmd_mem(pwjs_repl_state_t *state, char *arg) {
  jerry_heap_stats_t stats = {0};
  bool stats_ret = jerry_get_memory_stats(&stats);
  if (stats_ret) {
    pwjs_repl_printf("total: %u, occupied: %u, peak: %u\r\n", stats.size,
                   stats.allocated_bytes, stats.peak_allocated_bytes);
  } else {
    pwjs_repl_printf("Mem stat feature is not enabled\r\n");
  }
}

/**
 * .gc command
 */
static void cmd_gc(pwjs_repl_state_t *state, char *arg) {
  jerry_gc(JERRY_GC_PRESSURE_HIGH);
}

/**
 * .hi command
 */
static void cmd_hi(pwjs_repl_state_t *state, char *arg) {
  pwjs_repl_printf("         ___\r\n");
  pwjs_repl_printf("   _____/ . \\   Welcome to Kaluma v%s\r\n", PICOWJS_VERSION);
  pwjs_repl_printf("  /        __>  https://kaluma.io\r\n");
  pwjs_repl_printf(" |/\\________/\r\n");
  pwjs_repl_printf("  \\__@  v  v    Type '.help' for more info.\r\n");
  pwjs_repl_printf("-----------------------------------------------\r\n");
  pwjs_repl_println();
}

/**
 * .help command
 */
static void cmd_help(pwjs_repl_state_t *state, char *arg) {
  // print commands
  pwjs_repl_command_t *cmd = (pwjs_repl_command_t *)state->commands.head;
  while (cmd != NULL) {
    pwjs_repl_command_t *next =
        (pwjs_repl_command_t *)((pwjs_list_node_t *)cmd)->next;
    pwjs_repl_printf("%s\t%s\r\n", cmd->name, cmd->desc);
    cmd = next;
  }

  // print shortcuts
  pwjs_repl_printf("\r\n");
  pwjs_repl_printf("CTRL+C\tAbort running code\r\n");
  pwjs_repl_printf("CTRL+D\tSoft reset\r\n");
}

// --------------------------------------------------------------------------
// PUBLIC FUNCTIONS
// --------------------------------------------------------------------------

/**
 * Initialize the REPL
 */
void pwjs_repl_init(bool hi) {
  // initialize tty
  pwjs_io_tty_init(&tty);
  pwjs_io_tty_read_start(&tty, tty_read_cb);

  // initialize repl states
  state.mode = PWJS_REPL_MODE_NORMAL;
  state.echo = true;
  state.buffer_length = 0;
  state.position = 0;
  state.width = 80;
  state.escape_length = 0;
  state.history_size = 0;
  state.history_position = 0;
  state.handler = &default_handler;
  state.ymodem_state = 0;

  // initialize commands
  reset_commands();

  // print welcome
  if (hi) {
    cmd_hi(&state, NULL);
  }
}

void pwjs_repl_cleanup() { pwjs_io_tty_cleanup(); }

pwjs_repl_state_t *pwjs_get_repl_state() { return &state; }

void pwjs_repl_set_output(pwjs_repl_output_t output) {
  switch (output) {
    case PWJS_REPL_OUTPUT_NORMAL:
      pwjs_tty_printf("\33[0m"); /* set to normal color */
      break;
    case PWJS_REPL_OUTPUT_INFO:
      pwjs_tty_printf("\33[90m"); /* set to dark gray color */
      break;
    case PWJS_REPL_OUTPUT_ERROR:
      pwjs_tty_printf("\33[31m"); /* set to red color */
      break;
  }
}

static void pwjs_repl_pretty_print_indent(uint8_t indent) {
  for (uint8_t i = 0; i < indent; i++) pwjs_tty_putc(' ');
}

struct pwjs_repl_pretty_print_object_foreach_data {
  uint8_t indent;
  uint8_t depth;
  uint16_t count;
};

static bool pwjs_repl_pretty_print_object_foreach_count(
    const jerry_value_t prop_name, const jerry_value_t prop_value,
    void *user_data_p) {
  if (jerry_value_is_string(prop_name)) {
    struct pwjs_repl_pretty_print_object_foreach_data *data =
        (struct pwjs_repl_pretty_print_object_foreach_data *)user_data_p;
    data->count++;
  }
  return true;
}

static bool pwjs_repl_pretty_print_object_foreach(const jerry_value_t prop_name,
                                                const jerry_value_t prop_value,
                                                void *user_data_p) {
  if (jerry_value_is_string(prop_name)) {
    jerry_char_t buf[128];
    jerry_size_t len =
        jerry_substring_to_char_buffer(prop_name, 0, 127, buf, 127);
    buf[len] = '\0';
    struct pwjs_repl_pretty_print_object_foreach_data *data =
        (struct pwjs_repl_pretty_print_object_foreach_data *)user_data_p;
    pwjs_repl_pretty_print_indent(data->indent + 2);
    pwjs_tty_printf((const char *)buf);
    pwjs_tty_printf(": ");
    pwjs_repl_pretty_print(data->indent + 2, data->depth - 1, prop_value);
    if (data->count > 1) {
      pwjs_tty_printf(",");
    }
    pwjs_tty_printf("\r\n");
    data->count--;
  }
  return true;
}

void pwjs_repl_pretty_print(uint8_t indent, uint8_t depth, jerry_value_t value) {
  if (depth < 0) {
    return;
  } else if (depth == 0) {
    if (jerry_value_is_abort(value)) {
      pwjs_tty_printf("\33[31m");  // red
      pwjs_tty_printf("[Abort]");
      pwjs_tty_printf("\33[0m");
    } else if (jerry_value_is_array(value)) {
      pwjs_tty_printf("\33[96m");  // cyan
      pwjs_tty_printf("[Array]");
      pwjs_tty_printf("\33[0m");
    } else if (jerry_value_is_typedarray(value)) {
      pwjs_tty_printf("\33[96m");  // cyan
      jerry_typedarray_type_t type = jerry_get_typedarray_type(value);
      switch (type) {
        case JERRY_TYPEDARRAY_UINT8:
          pwjs_tty_printf("[Uint8Array]");
          break;
        case JERRY_TYPEDARRAY_UINT8CLAMPED:
          pwjs_tty_printf("[Uint8ClampedArray]");
          break;
        case JERRY_TYPEDARRAY_INT8:
          pwjs_tty_printf("[Int8Array]");
          break;
        case JERRY_TYPEDARRAY_UINT16:
          pwjs_tty_printf("[Uint16Array]");
          break;
        case JERRY_TYPEDARRAY_INT16:
          pwjs_tty_printf("[Int16Array]");
          break;
        case JERRY_TYPEDARRAY_UINT32:
          pwjs_tty_printf("[Uint32Array]");
          break;
        case JERRY_TYPEDARRAY_INT32:
          pwjs_tty_printf("[Int32Array]");
          break;
        case JERRY_TYPEDARRAY_FLOAT32:
          pwjs_tty_printf("[Float32Array]");
          break;
        case JERRY_TYPEDARRAY_FLOAT64:
          pwjs_tty_printf("[Float64Array]");
          break;
        default:
          pwjs_tty_printf("[TypedArray]");
          break;
      }
      pwjs_tty_printf("\33[0m");
    } else if (jerry_value_is_arraybuffer(value)) {
      jerry_length_t len = jerry_get_arraybuffer_byte_length(value);
      pwjs_tty_printf("ArrayBuffer { byteLength:");
      pwjs_tty_printf("\33[95m");  // magenta
      pwjs_tty_printf("%d", len);
      pwjs_tty_printf("\33[0m");
      pwjs_tty_printf("}");
    } else if (jerry_value_is_boolean(value)) {
      pwjs_tty_printf("\33[95m");  // magenta
      pwjs_repl_print_value(value);
      pwjs_tty_printf("\33[0m");
    } else if (jerry_value_is_constructor(value)) {
      pwjs_tty_printf("\33[96m");  // cyan
      pwjs_tty_printf("[Function]");
      pwjs_tty_printf("\33[0m");
    } else if (jerry_value_is_dataview(value)) {
      pwjs_tty_printf("\33[96m");  // cyan
      pwjs_tty_printf("[DataView]");
      pwjs_tty_printf("\33[0m");
    } else if (jerry_value_is_error(value)) {
      pwjs_tty_printf("\33[31m");  // red
      pwjs_tty_printf("[Error]");
      pwjs_tty_printf("\33[0m");
    } else if (jerry_value_is_function(value)) {
      pwjs_tty_printf("\33[96m");  // cyan
      pwjs_tty_printf("[Function]");
      pwjs_tty_printf("\33[0m");
    } else if (jerry_value_is_number(value)) {
      pwjs_tty_printf("\33[95m");  // magenda
      pwjs_repl_print_value(value);
      pwjs_tty_printf("\33[0m");
    } else if (jerry_value_is_null(value)) {
      pwjs_tty_printf("\33[90m");  // dark gray
      pwjs_tty_printf("null");
      pwjs_tty_printf("\33[0m");
    } else if (jerry_value_is_promise(value)) {
      pwjs_tty_printf("\33[96m");  // cyan
      pwjs_tty_printf("[Promise]");
      pwjs_tty_printf("\33[0m");
    } else if (jerry_value_is_object(value)) {
      pwjs_tty_printf("\33[96m");  // cyan
      pwjs_tty_printf("[Object]");
      pwjs_tty_printf("\33[0m");
    } else if (jerry_value_is_string(value)) {
      pwjs_tty_printf("\33[93m");  // yellow
      pwjs_tty_printf("'");
      pwjs_repl_print_value(value);
      pwjs_tty_printf("'");
      pwjs_tty_printf("\33[0m");
    } else if (jerry_value_is_symbol(value)) {
      pwjs_tty_printf("\33[96m");  // cyan
      pwjs_tty_printf("[Symbol]");
      pwjs_tty_printf("\33[0m");
    } else if (jerry_value_is_undefined(value)) {
      pwjs_tty_printf("\33[90m");  // dark gray
      pwjs_tty_printf("undefined");
      pwjs_tty_printf("\33[0m");
    }
  } else {
    if (jerry_value_is_abort(value)) {
      pwjs_repl_pretty_print(indent, 0, value);
    } else if (jerry_value_is_array(value)) {
      uint32_t len = jerry_get_array_length(value);
      pwjs_tty_printf("[");
      if (len > 0) {
        pwjs_tty_printf("\r\n");
        for (int i = 0; i < len; i++) {
          jerry_value_t item = jerry_get_property_by_index(value, i);
          pwjs_repl_pretty_print_indent(indent + 2);
          pwjs_repl_pretty_print(indent + 2, depth - 1, item);
          jerry_release_value(item);
          if (i < len - 1) pwjs_tty_printf(",");
          pwjs_tty_printf("\r\n");
        }
        pwjs_repl_pretty_print_indent(indent);
      }
      pwjs_tty_printf("]");
    } else if (jerry_value_is_typedarray(value)) {
      jerry_typedarray_type_t type = jerry_get_typedarray_type(value);
      switch (type) {
        case JERRY_TYPEDARRAY_UINT8:
          pwjs_tty_printf("Uint8Array [");
          break;
        case JERRY_TYPEDARRAY_UINT8CLAMPED:
          pwjs_tty_printf("Uint8ClampedArray [");
          break;
        case JERRY_TYPEDARRAY_INT8:
          pwjs_tty_printf("Int8Array [");
          break;
        case JERRY_TYPEDARRAY_UINT16:
          pwjs_tty_printf("Uint16Array [");
          break;
        case JERRY_TYPEDARRAY_INT16:
          pwjs_tty_printf("Int16Array [");
          break;
        case JERRY_TYPEDARRAY_UINT32:
          pwjs_tty_printf("Uint32Array [");
          break;
        case JERRY_TYPEDARRAY_INT32:
          pwjs_tty_printf("Int32Array [");
          break;
        case JERRY_TYPEDARRAY_FLOAT32:
          pwjs_tty_printf("Float32Array [");
          break;
        case JERRY_TYPEDARRAY_FLOAT64:
          pwjs_tty_printf("Float64Array [");
          break;
        default:
          pwjs_tty_printf("TypedArray [");
          break;
      }
      uint32_t len = jerry_get_typedarray_length(value);
      if (len > 0) {
        pwjs_tty_printf("\r\n");
        for (int i = 0; i < len; i++) {
          jerry_value_t item = jerry_get_property_by_index(value, i);
          pwjs_repl_pretty_print_indent(indent + 2);
          pwjs_repl_pretty_print(indent + 2, depth - 1, item);
          if (i < len - 1) pwjs_tty_printf(",");
          pwjs_tty_printf("\r\n");
          jerry_release_value(item);
        }
        pwjs_repl_pretty_print_indent(indent);
      }
      pwjs_tty_printf("]");
    } else if (jerry_value_is_arraybuffer(value)) {
      pwjs_repl_pretty_print(indent, 0, value);
    } else if (jerry_value_is_boolean(value)) {
      pwjs_repl_pretty_print(indent, 0, value);
    } else if (jerry_value_is_constructor(value)) {
      pwjs_repl_pretty_print(indent, 0, value);
    } else if (jerry_value_is_dataview(value)) {
      pwjs_repl_pretty_print(indent, 0, value);
    } else if (jerry_value_is_error(value)) {
      pwjs_repl_pretty_print(indent, 0, value);
    } else if (jerry_value_is_function(value)) {
      pwjs_repl_pretty_print(indent, 0, value);
    } else if (jerry_value_is_number(value)) {
      pwjs_repl_pretty_print(indent, 0, value);
    } else if (jerry_value_is_null(value)) {
      pwjs_repl_pretty_print(indent, 0, value);
    } else if (jerry_value_is_promise(value)) {
      pwjs_repl_pretty_print(indent, 0, value);
    } else if (jerry_value_is_object(value)) {
      // Check value is instanceof Error
      jerry_value_t global = jerry_get_global_object();
      jerry_value_t error_ctr = jerryxx_get_property(global, "Error");
      jerry_value_t result =
          jerry_binary_operation(JERRY_BIN_OP_INSTANCEOF, value, error_ctr);
      if (jerry_get_boolean_value(result)) {
        pwjs_tty_printf("\33[31m");  // red
        jerry_value_t tostr_fun = jerryxx_get_property(value, "toString");
        jerry_value_t ret_val = jerry_call_function(tostr_fun, value, NULL, 0);
        pwjs_repl_print_value(ret_val);
        jerry_release_value(ret_val);
        jerry_release_value(tostr_fun);
        pwjs_tty_printf("\33[0m");
      } else {
        struct pwjs_repl_pretty_print_object_foreach_data foreach_data = {indent,
                                                                        depth};
        jerry_foreach_object_property(
            value, pwjs_repl_pretty_print_object_foreach_count, &foreach_data);
        pwjs_tty_printf("{");
        if (foreach_data.count > 0) {
          pwjs_tty_printf("\r\n");
          jerry_foreach_object_property(
              value, pwjs_repl_pretty_print_object_foreach, &foreach_data);
          pwjs_repl_pretty_print_indent(indent);
        }
        pwjs_tty_printf("}");
      }
      jerry_release_value(result);
      jerry_release_value(error_ctr);
      jerry_release_value(global);
    } else if (jerry_value_is_string(value)) {
      pwjs_repl_pretty_print(indent, 0, value);
    } else if (jerry_value_is_symbol(value)) {
      pwjs_repl_pretty_print(indent, 0, value);
    } else if (jerry_value_is_undefined(value)) {
      pwjs_repl_pretty_print(indent, 0, value);
    }
  }
}

void pwjs_repl_println() { pwjs_tty_printf("\r\n"); }

void pwjs_repl_print_prompt() {
  pwjs_tty_printf("\33[0m");  // back to normal color
  if (state.echo) {
    state.buffer[state.buffer_length] = '\0';
    pwjs_tty_printf("\r\0337");  // save cursor position
    pwjs_tty_printf("> %s", &state.buffer);
    pwjs_tty_printf(
        "\33[H\33[900C\33[6n\0338\033[2C");  // query terminal screen width and
                                             // restore cursor position
  }
}

void pwjs_repl_register_command(char *name, char *desc, pwjs_repl_command_cb cb) {
  pwjs_repl_command_t *cmd = malloc(sizeof(pwjs_repl_command_t));
  strcpy(cmd->name, name);
  strcpy(cmd->desc, desc);
  cmd->cb = cb;
  pwjs_list_append(&state.commands, (pwjs_list_node_t *)cmd);
}

void pwjs_repl_unregister_command(char *name) {
  pwjs_repl_command_t *cmd = (pwjs_repl_command_t *)state.commands.head;
  while (strcmp(cmd->name, name) == 0) {
    pwjs_repl_command_t *next =
        (pwjs_repl_command_t *)((pwjs_list_node_t *)cmd)->next;
    pwjs_list_remove(&state.commands, (pwjs_list_node_t *)cmd);
    free(cmd);
    cmd = next;
  }
}

void pwjs_repl_clear_commands() {
  pwjs_repl_command_t *cmd = (pwjs_repl_command_t *)state.commands.head;
  while (cmd != NULL) {
    pwjs_repl_command_t *next =
        (pwjs_repl_command_t *)((pwjs_list_node_t *)cmd)->next;
    free(cmd);
    cmd = next;
  }
  pwjs_list_init(&state.commands);
}
