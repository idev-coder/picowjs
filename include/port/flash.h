#ifndef __PICOWJS_FLASH_H
#define __PICOWJS_FLASH_H

#include <stdint.h>
#include <stdio.h>

extern const uint8_t *picowjs_flash_addr;

/**
 * @brief Initialize flash
 */
void picowjs_flash_init();

/**
 * @brief Cleanup flash
 */
void picowjs_flash_cleanup();

/**
 * @brief Program data to internal flash
 *
 * @param sector sector number to program
 * @param offset offset to the sector (multiple of KALUMA_FLASH_PAGE_SIZE)
 * @param buffer buffer to write
 * @param size size of buffer to write (multiple of KALUMA_FLASH_PAGE_SIZE)
 * @return negative on error
 */
int picowjs_flash_program(uint32_t sector, uint32_t offset, uint8_t *buffer,
                     size_t size);

/**
 * @brief Erase data in internal flash
 *
 * @param sector sector number to erase
 * @param count how many sectors to erase from the sector number
 * @return negative on error
 */
int picowjs_flash_erase(uint32_t sector, size_t count);

#endif /* __PICOWJS_FLASH_H */
