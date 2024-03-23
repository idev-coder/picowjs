#include "runtime.h"

#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "gpio.h"
#include "io.h"
#include "jerryscript-ext/handler.h"
#include "jerryscript-port.h"
#include "jerryscript.h"
#include "jerryxx.h"
#include "kaluma_magic_strings.h"
#include "prog.h"
#include "repl.h"
#include "system.h"
#include "tty.h"

// --------------------------------------------------------------------------
// PRIVATE VARIABLES
// --------------------------------------------------------------------------

/**
 * Runtime VM stop
 * - 0: normal
 * - 1: break VM execution
 */
static uint8_t picowjs_runtime_vm_stop = 0;

/**
 * idle handle for processing enqueued jobs
 */
static picowjs_io_idle_handle_t idler;

// --------------------------------------------------------------------------
// PRIVATE FUNCTIONS
// --------------------------------------------------------------------------

static jerry_value_t vm_exec_stop_callback(void *user_p) {
  if (picowjs_runtime_vm_stop > 0) {
    picowjs_runtime_vm_stop = 0;
    return jerry_create_string((const jerry_char_t *)"Aborted");
  }
  return jerry_create_undefined();
}

static void idler_cb() {
  jerry_value_t ret_val = jerry_run_all_enqueued_jobs();
  if (jerry_value_is_error(ret_val)) {
    jerryxx_print_error(ret_val, true);
  }
  jerry_release_value(ret_val);
#ifdef _TARGET_FREERTOS_
  // ESP32 Kick the dog
  vTaskDelay(10);
#endif
}

// --------------------------------------------------------------------------
// PUBLIC FUNCTIONS
// --------------------------------------------------------------------------

void picowjs_runtime_init(bool load, bool first) {
  jerry_init(JERRY_INIT_EMPTY);
  jerry_set_vm_exec_stop_callback(vm_exec_stop_callback, &picowjs_runtime_vm_stop,
                                  16);
  jerry_register_magic_strings(magic_string_items, num_magic_string_items,
                               magic_string_lengths);
  picowjs_global_init();
  jerry_gc(JERRY_GC_PRESSURE_HIGH);
  if (load) {
    picowjs_runtime_load();
  }
  if (first) {
    // Initialize idler handle for queued jobs in jerryscript
    picowjs_io_idle_init(&idler);
    picowjs_io_idle_start(&idler, idler_cb);
  }
}

void picowjs_runtime_cleanup() {
  jerry_cleanup();
  picowjs_system_cleanup();
  picowjs_io_cleanup();
}

void picowjs_runtime_load() {
  uint32_t size = picowjs_prog_get_size();
  if (size > 0) {
    uint8_t *script = picowjs_prog_addr();
    jerry_value_t parsed_code =
        jerry_parse(NULL, 0, script, size, JERRY_PARSE_STRICT_MODE);
    if (!jerry_value_is_error(parsed_code)) {
      jerry_value_t ret_value = jerry_run(parsed_code);
      if (jerry_value_is_error(ret_value)) {
        jerryxx_print_error(ret_value, true);
        picowjs_runtime_cleanup();
        picowjs_runtime_init(false, false);
        return;
      }
      jerry_release_value(ret_value);
    } else {
      jerryxx_print_error(parsed_code, true);
    }
    jerry_release_value(parsed_code);
  }
}

void picowjs_runtime_set_vm_stop(uint8_t stop) { picowjs_runtime_vm_stop = stop; }
