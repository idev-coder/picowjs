#ifndef __PICOWJS_GPIO_H
#define __PICOWJS_GPIO_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  PICOWJS_GPIO_IO_MODE_INPUT,
  PICOWJS_GPIO_IO_MODE_OUTPUT,
  PICOWJS_GPIO_IO_MODE_INPUT_PULLUP,
  PICOWJS_GPIO_IO_MODE_INPUT_PULLDOWN,
} picowjs_gpio_io_mode_t;

#define PICOWJS_GPIO_LOW 0
#define PICOWJS_GPIO_HIGH 1

#define PICOWJS_GPIO_PULL_UP 0
#define PICOWJS_GPIO_PULL_DOWN 1

typedef void (*picowjs_gpio_irq_callback_t)(uint8_t pin, picowjs_gpio_io_mode_t mode);

/**
 * Initialize all GPIO on system boot
 */
void picowjs_gpio_init();

/**
 * Cleanup all GPIO on soft reset
 */
void picowjs_gpio_cleanup();

int picowjs_gpio_set_io_mode(uint8_t pin, picowjs_gpio_io_mode_t mode);
int picowjs_gpio_write(uint8_t pin, uint8_t value);
int picowjs_gpio_toggle(uint8_t pin);
int picowjs_gpio_read(uint8_t pin);
void picowjs_gpio_irq_set_callback(picowjs_gpio_irq_callback_t cb);
int picowjs_gpio_irq_attach(uint8_t pin, uint8_t events);
int picowjs_gpio_irq_detach(uint8_t pin);
void picowjs_gpio_irq_enable();
void picowjs_gpio_irq_disable();

#endif /* __PICOWJS_GPIO_H */
