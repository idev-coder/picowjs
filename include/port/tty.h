#ifndef __PICOWJS_TTY_H
#define __PICOWJS_TTY_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * Initialize TTY
 */
void picowjs_tty_init();

/**
 * Check the number of bytes available to read.
 *
 * @return the number of bytes in TTY read buffer.
 */
uint32_t picowjs_tty_available();

/**
 * Read bytes from TTY read buffer.
 *
 * @param buf
 * @param len
 * @return the number of bytes read
 */
uint32_t picowjs_tty_read(uint8_t *buf, size_t len);

/**
 * Read bytes synchronously from TTY read buffer.
 *
 * @param buf
 * @param len
 * @param timeout
 * @return the number of bytes read
 */
uint32_t picowjs_tty_read_sync(uint8_t *buf, size_t len, uint32_t timeout);

/**
 * Read a char from TTY
 *
 * @return char
 */
uint8_t picowjs_tty_getc();

/**
 * Write a char to TTY
 *
 * @param ch a character to write
 */
void picowjs_tty_putc(char ch);

/**
 * Write a formatted string to TTY
 *
 * @param fmt a string format
 * @param ... arguments for the format
 */
void picowjs_tty_printf(const char *fmt, ...);

#endif /* __PICOWJS_TTY_H */
