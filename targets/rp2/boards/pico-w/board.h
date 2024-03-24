#ifndef __RP2_PICO_W_H
#define __RP2_PICO_W_H

#include "jerryscript.h"

// system
#define PICOWJS_SYSTEM_ARCH "cortex-m0-plus"
#define PICOWJS_SYSTEM_PLATFORM "rp2"

// repl
#define PICOWJS_REPL_BUFFER_SIZE 1024
#define PICOWJS_REPL_HISTORY_SIZE 10

// Flash allocation map
//
// |         A        | B |     C     |     D     |
// |------------------|---|-----------|-----------|
// |      1008K       |16K|   512K    |   512K    |
//
// - A : binary (firmware)
// - B : storage (key-value database)
// - C : user program (js)
// - D : file system (lfs)
// (Total : 2MB)

// binary (1008KB)
#define PICOWJS_BINARY_MAX 0xFC000

// flash (B + C + D = 1040KB (=16KB + 1024KB))
#define PICOWJS_FLASH_OFFSET PICOWJS_BINARY_MAX
#define PICOWJS_FLASH_SECTOR_SIZE 4096
#define PICOWJS_FLASH_SECTOR_COUNT 260
#define PICOWJS_FLASH_PAGE_SIZE 256

// user program on flash (512KB)
#define PICOWJS_PROG_SECTOR_BASE 4
#define PICOWJS_PROG_SECTOR_COUNT 128

// storage on flash (16KB)
#define PICOWJS_STORAGE_SECTOR_BASE 0
#define PICOWJS_STORAGE_SECTOR_COUNT 4

// file system on flash (512K)
// - sector base : 132
// - sector count : 128
// - use block device : new Flash(132, 128)

// -----------------------------------------------------------------

#define PICOWJS_GPIO_COUNT 29
// #define ADC_NUM 3
#define PWM_NUM 27
#define I2C_NUM 2
#define SPI_NUM 2
#define UART_NUM 2
// #define LED_NUM 1
// #define BUTTON_NUM 0
#define PIO_NUM 2
#define PIO_SM_NUM 4

#define ADC_RESOLUTION_BIT 12
#define PWM_CLK_REF 1250
#define I2C_MAX_CLOCK 1000000
#define SCR_LOAD_GPIO 22  // GPIO 22
#define PICO_CYW43
#ifdef PICO_CYW43
#define WIFI_EN_GPIO 23  // GPIO 23
#endif                   /* PICO_CWY43 */
void board_init();

#endif /* __RP2_PICO_W_H */
