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

#ifndef __PWJS_I2C_H
#define __PWJS_I2C_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
  PWJS_I2C_MASTER,
  PWJS_I2C_SLAVE,
  PWJS_I2C_NONE,
} pwjs_i2c_mode_t;

typedef struct {
  int8_t sda;
  int8_t scl;
} pwjs_i2c_pins_t;

/**
 * Return default I2C pins. -1 means there is no default value on that pin.
 */
pwjs_i2c_pins_t pwjs_i2c_get_default_pins(uint8_t bus);

/**
 * Initialize all I2C when system started
 */
void pwjs_i2c_init();

/**
 * Cleanup all I2C when system cleanup
 */
void pwjs_i2c_cleanup();

/**
 * Setup a I2C bus as master.
 *
 * @param bus The bus number.
 * @param speed i2c speed
 * @param pins pin numbers for the SDA/SCL
 * @return Returns 0 on success or -1 on failure.
 */
int pwjs_i2c_setup_master(uint8_t bus, uint32_t speed, pwjs_i2c_pins_t pins);

/**
 * Setup a I2C bus as slave.
 *
 * @param bus The bus number.
 * @param address Address of the slave.
 * @param pins pin numbers for the SDA/SCL
 * @return Returns 0 on success or -1 on failure.
 */
int pwjs_i2c_setup_slave(uint8_t bus, uint8_t address, pwjs_i2c_pins_t pins);

/**
 * Memory write a given buffer to the bus (in master mode).
 *
 * @param bus The bus number.
 * @param address Where the data sent to.
 * @param mem_addr Memory address
 * @param mem_addr_size set 16 if mem_address is 16 bit address, otherwise 8.
 * @param buf Buffer to write.
 * @param len The size of the buffer.
 * @param timeout Timeout in milliseconds.
 * @return The number of bytes written or -1 on timeout or failed to write.
 */
int pwjs_i2c_mem_write_master(uint8_t bus, uint8_t address, uint16_t mem_addr,
                            uint8_t mem_addr_size, uint8_t *buf, size_t len,
                            uint32_t timeout);

/**
 * Memory read bytes from the bus and store them into a given buffer (in master
 * mode).
 *
 * @param bus The bus number.
 * @param address Where the data read from.
 * @param mem_addr Memory address
 * @param mem_addr_size set 16 if mem_address is 16 bit address, otherwise 8.
 * @param buf Data buffer where the read data to be stored.
 * @param len The size of the data buffer.
 * @param timeout Timeout in milliseconds.
 * @return The number of bytes read or -1 on timeout or failed to read.
 */
int pwjs_i2c_mem_read_master(uint8_t bus, uint8_t address, uint16_t mem_addr,
                           uint8_t mem_addr_size, uint8_t *buf, size_t len,
                           uint32_t timeout);

/**
 * Write a given buffer to the bus (in master mode).
 *
 * @param bus The bus number.
 * @param address Where the data sent to.
 * @param buf Buffer to write.
 * @param len The size of the buffer.
 * @param timeout Timeout in milliseconds.
 * @return The number of bytes written or -1 on timeout or failed to write.
 */
int pwjs_i2c_write_master(uint8_t bus, uint8_t address, uint8_t *buf, size_t len,
                        uint32_t timeout);

/**
 * Write a given buffer to the bus (in slave mode).
 *
 * @param bus The bus number.
 * @param buf Buffer to write.
 * @param len The size of the buffer.
 * @param timeout Timeout in milliseconds.
 * @return The number of bytes written or -1 on timeout or failed to write.
 */
int pwjs_i2c_write_slave(uint8_t bus, uint8_t *buf, size_t len, uint32_t timeout);

/**
 * Read bytes from the bus and store them into a given buffer (in master mode).
 *
 * @param bus The bus number.
 * @param address Where the data read from.
 * @param buf Data buffer where the read data to be stored.
 * @param len The size of the data buffer.
 * @param timeout Timeout in milliseconds.
 * @return The number of bytes read or -1 on timeout or failed to read.
 */
int pwjs_i2c_read_master(uint8_t bus, uint8_t address, uint8_t *buf, size_t len,
                       uint32_t timeout);

/**
 * Read bytes from the bus and store them into a given buffer (in slave mode).
 *
 * @param bus The bus number.
 * @param buf Buffer where the read data to be stored.
 * @param len The size of the buffer.
 * @param timeout Timeout in milliseconds.
 * @return The number of bytes read or -1 on timeout or failed to read.
 */
int pwjs_i2c_read_slave(uint8_t bus, uint8_t *buf, size_t len, uint32_t timeout);

/**
 * Close the I2C bus.
 *
 * @param bus The bus number.
 * @return Returns 0 on success or -1 on failure.
 */
int pwjs_i2c_close(uint8_t bus);

#endif /* __PWJS_I2C_H */
