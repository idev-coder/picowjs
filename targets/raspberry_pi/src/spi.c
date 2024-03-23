#include "spi.h"

#include "board.h"
#include "err.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

struct __spi_status_s {
  bool enabled;
} __spi_status[SPI_NUM];

static bool __check_spi_pins(uint8_t bus, picowjs_spi_pins_t pins) {
  if (bus == 0) {
    if ((pins.miso >= 0) && (pins.miso != 0) && (pins.miso != 4) &&
        (pins.miso != 16)) {
      return false;
    }
    if ((pins.mosi >= 0) && (pins.mosi != 3) && (pins.mosi != 7) &&
        (pins.mosi != 19)) {
      return false;
    }
    if ((pins.sck >= 0) && (pins.sck != 2) && (pins.sck != 6) &&
        (pins.sck != 18)) {
      return false;
    }
  } else if (bus == 1) {
    if ((pins.miso >= 0) && (pins.miso != 8) && (pins.miso != 12)) {
      return false;
    }
    if ((pins.mosi >= 0) && (pins.mosi != 11) && (pins.mosi != 15)) {
      return false;
    }
    if ((pins.sck >= 0) && (pins.sck != 10) && (pins.sck != 14)) {
      return false;
    }
  } else {
    return false;
  }
  return true;
}

/**
 * Return default SPI pins. -1 means there is no default value on that pin.
 */
picowjs_spi_pins_t picowjs_spi_get_default_pins(uint8_t bus) {
  picowjs_spi_pins_t pins;
  if (bus == 0) {
    pins.miso = 16;
    pins.mosi = 19;
    pins.sck = 18;
  } else if (bus == 1) {
    pins.miso = 12;
    pins.mosi = 11;
    pins.sck = 10;
  } else {
    pins.miso = -1;
    pins.mosi = -1;
    pins.sck = -1;
  }
  return pins;
}

static spi_inst_t *__get_spi_no(uint8_t bus) {
  if (bus == 0) {
    return spi0;
  } else if (bus == 1) {
    return spi1;
  } else {
    return NULL;
  }
}

/**
 * Initialize all SPI when system started
 */
void picowjs_spi_init() {
  for (int i = 0; i < SPI_NUM; i++) {
    __spi_status[i].enabled = false;
  }
}

/**
 * Cleanup all SPI when system cleanup
 */
void picowjs_spi_cleanup() {
  spi_deinit(spi0);
  spi_deinit(spi1);
  picowjs_spi_init();
}

/** SPI Setup
 */
int picowjs_spi_setup(uint8_t bus, picowjs_spi_mode_t mode, uint32_t baudrate,
                 picowjs_spi_bitorder_t bitorder, picowjs_spi_pins_t pins,
                 bool miso_pullup) {
  spi_inst_t *spi = __get_spi_no(bus);
  if ((spi == NULL) || (__spi_status[bus].enabled) ||
      (__check_spi_pins(bus, pins) == false)) {
    return EDEVINIT;
  }
  spi_cpol_t pol = SPI_CPOL_0;
  spi_cpha_t pha = SPI_CPHA_0;
  spi_order_t order;
  switch (mode) {
    case PICOWJS_SPI_MODE_0:
      pol = SPI_CPOL_0;
      pha = SPI_CPHA_0;
      break;
    case PICOWJS_SPI_MODE_1:
      pol = SPI_CPOL_0;
      pha = SPI_CPHA_1;
      break;
    case PICOWJS_SPI_MODE_2:
      pol = SPI_CPOL_1;
      pha = SPI_CPHA_0;
      break;
    case PICOWJS_SPI_MODE_3:
      pol = SPI_CPOL_1;
      pha = SPI_CPHA_1;
      break;
  }
  if (bitorder == PICOWJS_SPI_BITORDER_MSB) {
    order = SPI_MSB_FIRST;
  } else {
    order = SPI_LSB_FIRST;
  }
  spi_init(spi, baudrate);
  spi_set_format(spi, 8, pol, pha, order);
  if (pins.miso >= 0) {
    gpio_set_function(pins.miso, GPIO_FUNC_SPI);
  }
  if (pins.mosi >= 0) {
    gpio_set_function(pins.mosi, GPIO_FUNC_SPI);
  }
  if (pins.sck >= 0) {
    gpio_set_function(pins.sck, GPIO_FUNC_SPI);
  }
  if (miso_pullup) {
    gpio_pull_up(pins.miso);
  }
  __spi_status[bus].enabled = true;
  return 0;
}

int picowjs_spi_sendrecv(uint8_t bus, uint8_t *tx_buf, uint8_t *rx_buf, size_t len,
                    uint32_t timeout) {
  spi_inst_t *spi = __get_spi_no(bus);
  if ((spi == NULL) || (__spi_status[bus].enabled == false)) {
    return EDEVREAD;
  }
  (void)timeout;  // timeout is not supported.
  return spi_write_read_blocking(spi, tx_buf, rx_buf, len);
}

int picowjs_spi_send(uint8_t bus, uint8_t *buf, size_t len, uint32_t timeout) {
  spi_inst_t *spi = __get_spi_no(bus);
  if ((spi == NULL) || (__spi_status[bus].enabled == false)) {
    return EDEVWRITE;
  }
  (void)timeout;  // timeout is not supported.
  return spi_write_blocking(spi, buf, len);
}

int picowjs_spi_recv(uint8_t bus, uint8_t send_byte, uint8_t *buf, size_t len,
                uint32_t timeout) {
  spi_inst_t *spi = __get_spi_no(bus);
  if ((spi == NULL) || (__spi_status[bus].enabled == false)) {
    return EDEVREAD;
  }
  (void)timeout;  // timeout is not supported.
  return spi_read_blocking(spi, send_byte, buf, len);
}

int picowjs_set_spi_baudrate(uint8_t bus, uint32_t baudrate) {
  spi_inst_t *spi = __get_spi_no(bus);
  if ((spi == NULL) || (__spi_status[bus].enabled == false)) {
    return ENODEV;
  }
  spi_set_baudrate(spi, baudrate);
  return 0;
}

int picowjs_spi_close(uint8_t bus) {
  spi_inst_t *spi = __get_spi_no(bus);
  if ((spi == NULL) || (__spi_status[bus].enabled == false)) {
    return EDEVINIT;
  }
  spi_deinit(spi);
  __spi_status[bus].enabled = false;
  return 0;
}
