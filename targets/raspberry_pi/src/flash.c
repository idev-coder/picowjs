#include "flash.h"

#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"

const uint8_t *picowjs_flash_addr =
    (const uint8_t *)(XIP_BASE + KALUMA_FLASH_OFFSET);

void picowjs_flash_init() { return; }

void picowjs_flash_cleanup() { return; }

int picowjs_flash_program(uint32_t sector, uint32_t offset, uint8_t *buffer,
                     size_t size) {
  const uint32_t _base =
      KALUMA_FLASH_OFFSET + (sector * KALUMA_FLASH_SECTOR_SIZE) + offset;

  // base should be multiple of KALUMA_FLASH_PAGE_SIZE
  if (_base % KALUMA_FLASH_PAGE_SIZE > 0) {
    return -22;  // EINVAL
  }

  // size should be multiple of KALUMA_FLASH_PAGE_SIZE
  if (size % KALUMA_FLASH_PAGE_SIZE > 0) {
    return -22;  // EINVAL
  }

  uint32_t saved_irq = save_and_disable_interrupts();
  flash_range_program(_base, buffer, size);
  restore_interrupts(saved_irq);
  return 0;
}

int picowjs_flash_erase(uint32_t sector, size_t count) {
  const uint32_t _base =
      KALUMA_FLASH_OFFSET + (sector * KALUMA_FLASH_SECTOR_SIZE);
  const uint32_t _size = count * KALUMA_FLASH_SECTOR_SIZE;

  // _base should be multiple of KALUMA_FLASH_PAGE_SIZE
  if (_base % KALUMA_FLASH_SECTOR_SIZE > 0) {
    return -22;  // EINVAL
  }

  // _size should be multiple of KALUMA_FLASH_PAGE_SIZE
  if (_size % KALUMA_FLASH_SECTOR_SIZE > 0) {
    return -22;  // EINVAL
  }

  uint32_t saved_irq = save_and_disable_interrupts();
  flash_range_erase(_base, _size);
  restore_interrupts(saved_irq);
  return 0;
}
