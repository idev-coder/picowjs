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

#ifndef __PWJS_UART_H
#define __PWJS_UART_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
  PWJS_UART_PARITY_TYPE_NONE = 0,
  PWJS_UART_PARITY_TYPE_ODD,
  PWJS_UART_PARITY_TYPE_EVEN
} pwjs_uart_parity_type_t;

typedef enum {
  PWJS_UART_FLOW_NONE = 0,
  PWJS_UART_FLOW_RTS,
  PWJS_UART_FLOW_CTS,
  PWJS_UART_FLOW_RTS_CTS
} pwjs_uart_flow_control_t;

typedef struct {
  int8_t tx;
  int8_t rx;
  int8_t cts;
  int8_t rts;
} pwjs_uart_pins_t;

/**
 * Return default UART pins. -1 means there is no default value on that pin.
 */
pwjs_uart_pins_t pwjs_uart_get_default_pins(uint8_t port);

/**
 * Initialize all UART when system started
 */
void pwjs_uart_init();

/**
 * Cleanup all UART when system cleanup
 */
void pwjs_uart_cleanup();

/**
 * Setup a UART port. This have to manage an internal read buffer.
 *
 * @param port
 * @param baudrate
 * @param bits databits 8 or 9
 * @param parity
 * @param stop stopbits 1 or 2
 * @param flow
 * @param buffer_size The size of read buffer
 * @param pins pin numbers for the Tx/Rx/CTS/RTS
 * @return Positive number if successfully setup, negative otherwise.
 */
int pwjs_uart_setup(uint8_t port, uint32_t baudrate, uint8_t bits,
                  pwjs_uart_parity_type_t parity, uint8_t stop,
                  pwjs_uart_flow_control_t flow, size_t buffer_size,
                  pwjs_uart_pins_t pins);

/**
 * Write a given buffer to the port.
 *
 * @param port
 * @param buf
 * @param len
 * @return the number of bytes written or -1 if nothing written.
 */
int pwjs_uart_write(uint8_t port, uint8_t *buf, size_t len);

/**
 * Check the number of bytes available to read.
 *
 * @param port
 * @return the number of bytes in read buffer.
 */
uint32_t pwjs_uart_available(uint8_t port);

/**
 * Read bytes from the port and store them into a given buffer.
 *
 * @param port
 * @param buf
 * @param len
 * @return the number of bytes read
 */
uint32_t pwjs_uart_read(uint8_t port, uint8_t *buf, size_t len);

/**
 * Close the UART port
 *
 * @param port
 */
int pwjs_uart_close(uint8_t port);

#endif /* __PWJS_UART_H */
