#ifndef __PICOWJS_SPI_H
#define __PICOWJS_SPI_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
  PICOWJS_SPI_MODE_0,  // (CPOL=0/CPHA=0)
  PICOWJS_SPI_MODE_1,  // (CPOL=0/CPHA=1)
  PICOWJS_SPI_MODE_2,  // (CPOL=1/CPHA=0)
  PICOWJS_SPI_MODE_3   // (CPOL=1/CPHA=1)
} picowjs_spi_mode_t;

typedef enum { PICOWJS_SPI_BITORDER_MSB, PICOWJS_SPI_BITORDER_LSB } picowjs_spi_bitorder_t;

typedef struct {
  int8_t miso;
  int8_t mosi;
  int8_t sck;
} picowjs_spi_pins_t;

/**
 * Return default UART pins. -1 means there is no default value on that pin.
 */
picowjs_spi_pins_t picowjs_spi_get_default_pins(uint8_t bus);
/**
 * Initialize all SPI when system started
 */
void picowjs_spi_init();

/**
 * Cleanup all SPI when system cleanup
 */
void picowjs_spi_cleanup();

/**
 * Setup SPI bus as the master device
 *
 * @param bus The bus number.
 * @param mode SPI mode of clock polarity and phase.
 * @param baudrate Baud rate.
 * @param bit_order Bit order (MSB or LSB).
 * @param bits Number of bits in each transferred word.
 * @param pins pin numbers for the SCK/MISO/MOSI
 * @param miso_pullup true when MISO internal pull up is needed.
 * @return Returns 0 on success or minus value (err) on failure.
 */
int picowjs_spi_setup(uint8_t bus, picowjs_spi_mode_t mode, uint32_t baudrate,
                 picowjs_spi_bitorder_t bitorder, picowjs_spi_pins_t pins,
                 bool miso_pullup);

/**
 * Send and receive data simultaneously to the SPI bus
 *
 * @param bus
 * @param tx_buf
 * @param rx_buf
 * @param len
 * @param timeout
 * @return the number of bytes read or minus value (err) on timeout or nothing
 * written.
 */
int picowjs_spi_sendrecv(uint8_t bus, uint8_t *tx_buf, uint8_t *rx_buf, size_t len,
                    uint32_t timeout);

/**
 * Send data to the SPI bus
 *
 * @param bus
 * @param buf
 * @param len
 * @param timeout
 * @return the number of bytes written or -1 on timeout or nothing written.
 */
int picowjs_spi_send(uint8_t bus, uint8_t *buf, size_t len, uint32_t timeout);

/**
 * Receive data from the SPI bus and store them into a given buffer.
 *
 * @param {uint8_t} bus
 * @param {uint8_t} send_byte byte to send
 * @param {uint8_t*} buf
 * @param {size_t} len
 * @param {uint32_t} timeout
 * @return {int} the number of bytes read
 */
int picowjs_spi_recv(uint8_t bus, uint8_t send_byte, uint8_t *buf, size_t len,
                uint32_t timeout);

/**
 * Set SPI baudrate - change the clock frequency
 *
 * @param bus The bus number.
 * @param baudrate Baud rate.
 * @return int Returns 0 on success or minus value (err) on failure.
 */
int picowjs_set_spi_baudrate(uint8_t bus, uint32_t baudrate);

/**
 * Close the SPI bus
 */
int picowjs_spi_close(uint8_t bus);

#endif /* __PICOWJS_SPI_H */
