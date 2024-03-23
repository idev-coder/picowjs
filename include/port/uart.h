#ifndef __PICOWJS_UART_H
#define __PICOWJS_UART_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
  PICOWJS_UART_PARITY_TYPE_NONE = 0,
  PICOWJS_UART_PARITY_TYPE_ODD,
  PICOWJS_UART_PARITY_TYPE_EVEN
} picowjs_uart_parity_type_t;

typedef enum {
  PICOWJS_UART_FLOW_NONE = 0,
  PICOWJS_UART_FLOW_RTS,
  PICOWJS_UART_FLOW_CTS,
  PICOWJS_UART_FLOW_RTS_CTS
} picowjs_uart_flow_control_t;

typedef struct {
  int8_t tx;
  int8_t rx;
  int8_t cts;
  int8_t rts;
} picowjs_uart_pins_t;

/**
 * Return default UART pins. -1 means there is no default value on that pin.
 */
picowjs_uart_pins_t picowjs_uart_get_default_pins(uint8_t port);

/**
 * Initialize all UART when system started
 */
void picowjs_uart_init();

/**
 * Cleanup all UART when system cleanup
 */
void picowjs_uart_cleanup();

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
int picowjs_uart_setup(uint8_t port, uint32_t baudrate, uint8_t bits,
                  picowjs_uart_parity_type_t parity, uint8_t stop,
                  picowjs_uart_flow_control_t flow, size_t buffer_size,
                  picowjs_uart_pins_t pins);

/**
 * Write a given buffer to the port.
 *
 * @param port
 * @param buf
 * @param len
 * @return the number of bytes written or -1 if nothing written.
 */
int picowjs_uart_write(uint8_t port, uint8_t *buf, size_t len);

/**
 * Check the number of bytes available to read.
 *
 * @param port
 * @return the number of bytes in read buffer.
 */
uint32_t picowjs_uart_available(uint8_t port);

/**
 * Read bytes from the port and store them into a given buffer.
 *
 * @param port
 * @param buf
 * @param len
 * @return the number of bytes read
 */
uint32_t picowjs_uart_read(uint8_t port, uint8_t *buf, size_t len);

/**
 * Close the UART port
 *
 * @param port
 */
int picowjs_uart_close(uint8_t port);

#endif /* __PICOWJS_UART_H */
