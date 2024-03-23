#include "adc.h"

#include <stdint.h>

#include "board.h"
#include "err.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

/**
 * Get ADC index
 *
 * @param pin Pin number.
 * @return Returns index on success or EINVPIN on failure.
 */
static int __get_adc_index(uint8_t pin) {
  if ((pin >= 26) && (pin <= 30)) {
    return pin - 26;  // GPIO 26 is channel 0
  }
  return EINVPIN;
}

/**
 * Initialize all ADC channels when system started
 */
void picowjs_adc_init() { adc_init(); }

/**
 * Cleanup all ADC channels when system cleanup
 */
void picowjs_adc_cleanup() {
  // adc pins will be reset at the GPIO cleanup function.
}

/**
 * Read value from the ADC channel
 *
 * @param {uint8_t} adcIndex
 * @return {double}
 */
double picowjs_adc_read(uint8_t adcIndex) {
  return (double)adc_read() / (1 << ADC_RESOLUTION_BIT);
}

int picowjs_adc_setup(uint8_t pin) {


  
  int ch = __get_adc_index(pin);
  if (ch < 0) {
    return EINVPIN;
  } else if (ch == 4) {
    adc_set_temp_sensor_enabled(true);
  } else {
    adc_gpio_init(pin);
  }
  adc_select_input(ch);
  return 0;
}

int picowjs_adc_close(uint8_t pin) {
  int ch = __get_adc_index(pin);
  if (ch < 0) {
    return EINVPIN;
  } else if (ch == 4) {
    adc_set_temp_sensor_enabled(false);
  }
  return 0;
}
