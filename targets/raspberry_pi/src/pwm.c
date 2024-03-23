#include "pwm.h"

#include <stdint.h>

#include "board.h"
#include "err.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"

static struct __pwm_config_s {
  double freq;
  double duty;
  uint16_t period;
  bool enabled;
} __pwm_config[PWM_NUM];

static int __get_pwm_index(uint8_t pin) {
  if (pin <= 22) {
    return pin;
  } else if ((pin >= 25) && (pin <= 28)) {
    return pin - 2;
  }
  return EINVPIN;
}

/**
 * Initialize all PWM when system started
 */
void picowjs_pwm_init() {
  for (int i = 0; i < PWM_NUM; i++) {
    __pwm_config[i].freq = 0;
    __pwm_config[i].duty = 0;
    __pwm_config[i].period = 0;
    __pwm_config[i].enabled = false;
  }
}

/**
 * Cleanup all PWM when system cleanup
 */
void picowjs_pwm_cleanup() { picowjs_pwm_init(); }

uint16_t __get_period(double frequency) {
  int ref_div = 1;
  int duty_mul = 1;
  if (frequency > 100000) {
    ref_div = 10;
    if (frequency > 1000000) {
      frequency = 1000000;
    }
  } else if (frequency < 25) {
    duty_mul = 32;
  } else if (frequency < 50) {
    duty_mul = 16;
  } else if (frequency < 100) {
    duty_mul = 8;
  } else if (frequency < 200) {
    duty_mul = 4;
  } else if (frequency < 400) {
    duty_mul = 2;
  };
  return (PWM_CLK_REF / ref_div) * duty_mul;
}

int picowjs_pwm_set_inversion(uint8_t pin, uint8_t inv_pin) {
  int pwm_index = __get_pwm_index(pin);
  int pwm_inv_index = __get_pwm_index(inv_pin);
  if ((pwm_index < 0) || (pwm_inv_index < 0)) {
    return EINVPIN;  // Error
  }
  uint16_t uint_duty =
      (uint16_t)(__pwm_config[pwm_index].duty * __pwm_config[pwm_index].period);
  bool inv_a = false;
  bool inv_b = false;
  if (pwm_gpio_to_channel(inv_pin) == PWM_CHAN_A) {
    inv_a = true;
  } else {
    inv_b = true;
  }
  pwm_set_chan_level(pwm_gpio_to_slice_num(inv_pin),
                     pwm_gpio_to_channel(inv_pin), uint_duty);
  pwm_set_output_polarity(pwm_gpio_to_slice_num(inv_pin), inv_a, inv_b);
  gpio_set_function(inv_pin, GPIO_FUNC_PWM);
  return 0;
}

/**
 * return Returns 0 on success or -1 on failure.
 */
int picowjs_pwm_setup(uint8_t pin, double frequency, double duty) {
  int pwm_index = __get_pwm_index(pin);
  if (pwm_index < 0) {
    return EINVPIN;  // Error
  }
  if (frequency < 13) {
    frequency = 13;  // Min is 13Hz
  }
  uint16_t period = __get_period(frequency);
  double clk_div = clock_get_hz(clk_sys) / (frequency * period);
  bool enabled = false;
  uint16_t uint_duty = (uint16_t)(duty * period);
  gpio_set_function(pin, GPIO_FUNC_PWM);
  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, clk_div);
  pwm_config_set_wrap(&config, period - 1);
  if (__pwm_config[pwm_index].enabled) {
    enabled = true;
  }
  pwm_init(pwm_gpio_to_slice_num(pin), &config, enabled);
  pwm_set_chan_level(pwm_gpio_to_slice_num(pin), pwm_gpio_to_channel(pin),
                     uint_duty);
  __pwm_config[pwm_index].freq = frequency;
  __pwm_config[pwm_index].duty = duty;
  __pwm_config[pwm_index].period = period;
  return 0;
}

int picowjs_check_pwm_inv_port(uint8_t pin, int8_t inv_pin) {
  int pwm_index = __get_pwm_index(pin);
  if ((pwm_index < 0) || (pin == inv_pin) ||
      (pwm_gpio_to_channel(pin) == pwm_gpio_to_channel(inv_pin)) ||
      (pwm_gpio_to_slice_num(pin) != pwm_gpio_to_slice_num(inv_pin))) {
    return EINVPIN;
  }
  return 0;
}

/**
 */
int picowjs_pwm_start(uint8_t pin) {
  int pwm_index = __get_pwm_index(pin);
  if (pwm_index < 0) {
    return EINVPIN;  // Error
  }
  pwm_set_enabled(pwm_gpio_to_slice_num(pin), true);
  __pwm_config[pwm_index].enabled = true;
  return 0;
}

/**
 */
int picowjs_pwm_stop(uint8_t pin) {
  int pwm_index = __get_pwm_index(pin);
  if (pwm_index < 0) {
    return EINVPIN;  // Error
  }
  pwm_set_enabled(pwm_gpio_to_slice_num(pin), false);
  __pwm_config[pwm_index].enabled = false;
  return 0;
}

/**
 */
double picowjs_pwm_get_frequency(uint8_t pin) {
  int pwm_index = __get_pwm_index(pin);
  if (pwm_index < 0) {
    return EINVPIN;  // Error
  }
  return __pwm_config[pwm_index].freq;
}

/**
 */
double picowjs_pwm_get_duty(uint8_t pin) {
  int pwm_index = __get_pwm_index(pin);
  if (pwm_index < 0) {
    return EINVPIN;  // Error
  }
  return __pwm_config[pwm_index].duty;
}

/**
 */
int picowjs_pwm_set_duty(uint8_t pin, double duty) {
  int pwm_index = __get_pwm_index(pin);
  if (pwm_index < 0) {
    return EINVPIN;  // Error
  }
  uint16_t uint_duty = (uint16_t)(duty * __pwm_config[pwm_index].period);
  if (__pwm_config[pwm_index].enabled) {
    while (pwm_get_counter(pwm_gpio_to_slice_num(pin)) != 0)
      ;
  }
  pwm_set_chan_level(pwm_gpio_to_slice_num(pin), pwm_gpio_to_channel(pin),
                     uint_duty);
  __pwm_config[pwm_index].duty = duty;
  return 0;
}

/**
 */
int picowjs_pwm_set_frequency(uint8_t pin, double frequency) {
  int pwm_index = __get_pwm_index(pin);
  if (pwm_index < 0) {
    return EINVPIN;  // Error
  }
  double previous_duty = picowjs_pwm_get_duty(pin);
  /* The previous duty ratio must be hold up regardless of changing frequency */
  if (__pwm_config[pwm_index].enabled) {
    while (pwm_get_counter(pwm_gpio_to_slice_num(pin)) != 0)
      ;
  }
  picowjs_pwm_setup(pin, frequency, previous_duty);
  return 0;
}

/**
 */
int picowjs_pwm_close(uint8_t pin) {
  int pwm_index = __get_pwm_index(pin);
  if (pwm_index < 0) {
    return EINVPIN;  // Error
  }
  if (__pwm_config[pwm_index].enabled) {
    picowjs_pwm_stop(pin);
  }
  return 0;
}
