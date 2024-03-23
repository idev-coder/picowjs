#include "system.h"

#include "adc.h"
#include "board.h"
#include "flash.h"
#include "gpio.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pll.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/xosc.h"
#include "hardware/pio.h"
#include "i2c.h"
#include "io.h"
#include "pico/stdlib.h"
#include "pico/unique_id.h"
#include "pwm.h"
#include "rtc.h"
#include "spi.h"
#include "tty.h"
#include "tusb.h"
#include "uart.h"
#ifdef PICO_CYW43
#include "module_pico_cyw43.h"
#include <pico/cyw43_arch.h>
#endif /* PICO_CYW43 */

static char serial[2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1];

/**
 * Delay in milliseconds
 */
void picowjs_delay(uint32_t msec) { sleep_ms(msec); }

/**
 * Return current time (UNIX timestamp in milliseconds)
 */
uint64_t picowjs_gettime() { return to_ms_since_boot(get_absolute_time()); }

/**
 * Return uid of device
 */
char *picowjs_getuid() {
  return serial;
}

/**
 * Return MAX of the microsecond counter 44739242
 */
uint64_t picowjs_micro_maxtime() {
  return 0xFFFFFFFFFFFFFFFF;  // Max of the uint64()
}

/**
 * Return microsecond counter
 */
#ifdef NDEBUG
uint64_t picowjs_micro_gettime() { return get_absolute_time(); }
#else
uint64_t picowjs_micro_gettime() { return get_absolute_time()._private_us_since_boot; }
#endif

/**
 * microsecond delay
 */
void picowjs_micro_delay(uint32_t usec) { sleep_us(usec); }

static void picowjs_uid_init() {
  pico_get_unique_board_id_string(serial, sizeof(serial));
}

static void picowjs_pio_init() {
  for (int i = 0; i < PIO_SM_NUM; i++) {
    if (pio_sm_is_claimed(pio0, i))
      pio_sm_unclaim(pio0, i);
    if (pio_sm_is_claimed(pio1, i))
      pio_sm_unclaim(pio1, i);
  }
  pio_clear_instruction_memory(pio0);
  pio_clear_instruction_memory(pio1);
}

/**
 * Kaluma Hardware System Initializations
 */
void picowjs_system_init() {
  stdio_init_all();
  picowjs_uid_init();
  picowjs_pio_init();
  picowjs_gpio_init();
  picowjs_adc_init();
  picowjs_pwm_init();
  picowjs_i2c_init();
  picowjs_spi_init();
  picowjs_uart_init();
  picowjs_rtc_init();
  picowjs_flash_init();
}

void picowjs_system_cleanup() {
#ifdef PICO_CYW43
  picowjs_cyw43_deinit();
#endif
  picowjs_adc_cleanup();
  picowjs_pwm_cleanup();
  picowjs_i2c_cleanup();
  picowjs_spi_cleanup();
  picowjs_uart_cleanup();
  picowjs_gpio_cleanup();
  picowjs_rtc_cleanup();
  picowjs_flash_cleanup();
}

uint8_t picowjs_running_script_check() {
  gpio_set_pulls(SCR_LOAD_GPIO, true, false);
  sleep_us(100);
  bool load_state = gpio_get(SCR_LOAD_GPIO);
  gpio_set_pulls(SCR_LOAD_GPIO, false, false);
  return load_state;
}

void picowjs_custom_infinite_loop() {
#ifdef PICO_CYW43
  picowjs_cyw43_infinite_loop();
#endif
}
