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

#ifndef __PWJS_GPIO_H
#define __PWJS_GPIO_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  PWJS_GPIO_IO_MODE_INPUT,
  PWJS_GPIO_IO_MODE_OUTPUT,
  PWJS_GPIO_IO_MODE_INPUT_PULLUP,
  PWJS_GPIO_IO_MODE_INPUT_PULLDOWN,
} pwjs_gpio_io_mode_t;

#define PWJS_GPIO_LOW 0
#define PWJS_GPIO_HIGH 1

#define PWJS_GPIO_PULL_UP 0
#define PWJS_GPIO_PULL_DOWN 1

typedef void (*pwjs_gpio_irq_callback_t)(uint8_t pin, pwjs_gpio_io_mode_t mode);

/**
 * Initialize all GPIO on system boot
 */
void pwjs_gpio_init();

/**
 * Cleanup all GPIO on soft reset
 */
void pwjs_gpio_cleanup();

int pwjs_gpio_set_io_mode(uint8_t pin, pwjs_gpio_io_mode_t mode);
int pwjs_gpio_write(uint8_t pin, uint8_t value);
int pwjs_gpio_toggle(uint8_t pin);
int pwjs_gpio_read(uint8_t pin);
void pwjs_gpio_irq_set_callback(pwjs_gpio_irq_callback_t cb);
int pwjs_gpio_irq_attach(uint8_t pin, uint8_t events);
int pwjs_gpio_irq_detach(uint8_t pin);
void pwjs_gpio_irq_enable();
void pwjs_gpio_irq_disable();

#endif /* __PWJS_GPIO_H */
