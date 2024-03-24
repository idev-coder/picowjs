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

#include "flash.h"

#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"

const uint8_t *pwjs_flash_addr =
    (const uint8_t *)(XIP_BASE + PICOWJS_FLASH_OFFSET);

void pwjs_flash_init() { return; }

void pwjs_flash_cleanup() { return; }

int pwjs_flash_program(uint32_t sector, uint32_t offset, uint8_t *buffer,
                     size_t size) {
  const uint32_t _base =
      PICOWJS_FLASH_OFFSET + (sector * PICOWJS_FLASH_SECTOR_SIZE) + offset;

  // base should be multiple of PICOWJS_FLASH_PAGE_SIZE
  if (_base % PICOWJS_FLASH_PAGE_SIZE > 0) {
    return -22;  // EINVAL
  }

  // size should be multiple of PICOWJS_FLASH_PAGE_SIZE
  if (size % PICOWJS_FLASH_PAGE_SIZE > 0) {
    return -22;  // EINVAL
  }

  uint32_t saved_irq = save_and_disable_interrupts();
  flash_range_program(_base, buffer, size);
  restore_interrupts(saved_irq);
  return 0;
}

int pwjs_flash_erase(uint32_t sector, size_t count) {
  const uint32_t _base =
      PICOWJS_FLASH_OFFSET + (sector * PICOWJS_FLASH_SECTOR_SIZE);
  const uint32_t _size = count * PICOWJS_FLASH_SECTOR_SIZE;

  // _base should be multiple of PICOWJS_FLASH_PAGE_SIZE
  if (_base % PICOWJS_FLASH_SECTOR_SIZE > 0) {
    return -22;  // EINVAL
  }

  // _size should be multiple of PICOWJS_FLASH_PAGE_SIZE
  if (_size % PICOWJS_FLASH_SECTOR_SIZE > 0) {
    return -22;  // EINVAL
  }

  uint32_t saved_irq = save_and_disable_interrupts();
  flash_range_erase(_base, _size);
  restore_interrupts(saved_irq);
  return 0;
}
