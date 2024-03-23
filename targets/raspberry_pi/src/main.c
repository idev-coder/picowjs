#include "board.h"
#include "gpio.h"
#include "io.h"
#include "repl.h"
#include "runtime.h"
#include "system.h"
#include "tty.h"

int main(void) {
  bool load = false;
  picowjs_system_init();
  load = picowjs_running_script_check();
  picowjs_tty_init();
  picowjs_io_init();
  picowjs_repl_init(true);
  picowjs_runtime_init(load, true);
  picowjs_io_run(true);
}
