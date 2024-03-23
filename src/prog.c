#include "prog.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "flash.h"

static uint8_t *page_buffer = NULL;
static uint32_t page_written = 0;
static uint32_t total_written = 0;

static const uint32_t KALUMA_PROG_MAX =
    (KALUMA_PROG_SECTOR_COUNT * KALUMA_FLASH_SECTOR_SIZE);

static int page_buffer_flush() {
  int sector = (total_written - 1) / KALUMA_FLASH_SECTOR_SIZE;
  int offset = (((total_written - 1) % KALUMA_FLASH_SECTOR_SIZE) /
                KALUMA_FLASH_PAGE_SIZE) *
               KALUMA_FLASH_PAGE_SIZE;
  int ret = picowjs_flash_program(KALUMA_PROG_SECTOR_BASE + sector, offset,
                             page_buffer, KALUMA_FLASH_PAGE_SIZE);
  if (ret < 0) return ret;
  memset(page_buffer, 0, KALUMA_FLASH_PAGE_SIZE);
  page_written = 0;
  return 0;
}

static int page_buffer_push(uint8_t byte) {
  page_buffer[page_written] = byte;
  page_written++;
  total_written++;

  // flash quota exceeded
  if (total_written >= KALUMA_PROG_MAX) {
    return -122;  // EDQUOT
  }

  // flush when page_buffer is full
  if (page_written >= KALUMA_FLASH_PAGE_SIZE) {
    int ret = page_buffer_flush();
    if (ret < 0) return ret;
  }
  return 0;
}

void picowjs_prog_clear() {
  picowjs_flash_erase(KALUMA_PROG_SECTOR_BASE, KALUMA_PROG_SECTOR_COUNT);
}

void picowjs_prog_begin() {
  picowjs_prog_clear();
  page_buffer = malloc(KALUMA_FLASH_PAGE_SIZE);
  memset(page_buffer, 0, KALUMA_FLASH_PAGE_SIZE);
  page_written = 0;
  total_written = 0;
}

int picowjs_prog_write(uint8_t *buffer, int size) {
  for (int i = 0; i < size; i++) {
    int ret = page_buffer_push(buffer[i]);
    if (ret < 0) {
      return ret;
    }
  }
  return 0;
}

int picowjs_prog_end() {
  // push end of string
  page_buffer_push(0);

  // flush if buffer has data
  if (page_written > 0) {
    int ret = page_buffer_flush();
    if (ret < 0) return -1;
  }

  if (page_buffer != NULL) {
    free(page_buffer);
  }
  total_written = 0;
  return 0;
}

uint32_t picowjs_prog_get_size() {
  char *prog = (char *)picowjs_prog_addr();
  if (prog[0] == '\xFF') {  // flash erased (no code written)
    return 0;
  }
  return strlen(prog);
}

uint32_t picowjs_prog_max_size() {
  return (KALUMA_PROG_SECTOR_COUNT * KALUMA_FLASH_SECTOR_SIZE);
}

uint8_t *picowjs_prog_addr() {
  return (uint8_t *)(picowjs_flash_addr +
                     (KALUMA_PROG_SECTOR_BASE * KALUMA_FLASH_SECTOR_SIZE));
}
