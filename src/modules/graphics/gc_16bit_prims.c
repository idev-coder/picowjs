/* Copyright (c) 2019 Kaluma
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
 *
 *
 */

#include "gc_16bit_prims.h"

#include <stdlib.h>
#include <string.h>

#include "font.h"
#include "gc.h"
#include "jerryscript.h"

/**
 * Graphic primitive functions for 16-bits color graphic buffer
 */

void gc_prim_16bit_set_pixel(gc_handle_t *handle, int16_t x, int16_t y,
                             uint16_t color) {
  if ((x >= 0) && (x < handle->width) && (y >= 0) && (y < handle->height)) {
    switch (handle->rotation) {
      case 1:
        SWAP_INT16(x, y)
        x = handle->device_width - x - 1;
        break;
      case 2:
        x = handle->device_width - x - 1;
        y = handle->device_height - y - 1;
        break;
      case 3:
        SWAP_INT16(x, y)
        y = handle->device_height - y - 1;
        break;
    }
    uint32_t idx = ((y * handle->device_width) + x) * 2;
    handle->buffer[idx] = color >> 8;
    handle->buffer[idx + 1] = color & 0xFF;
  }
}

void gc_prim_16bit_get_pixel(gc_handle_t *handle, int16_t x, int16_t y,
                             uint16_t *color) {
  if ((x >= 0) && (x < handle->width) && (y >= 0) && (y < handle->height)) {
    switch (handle->rotation) {
      case 1:
        SWAP_INT16(x, y)
        x = handle->device_width - x - 1;
        break;
      case 2:
        x = handle->device_width - x - 1;
        y = handle->device_height - y - 1;
        break;
      case 3:
        SWAP_INT16(x, y)
        y = handle->device_height - y - 1;
        break;
    }
    uint32_t idx = ((y * handle->device_width) + x) * 2;
    *color = handle->buffer[idx] << 8 | handle->buffer[idx + 1];
  }
}

void gc_prim_16bit_draw_vline(gc_handle_t *handle, int16_t x, int16_t y,
                              int16_t h, uint16_t color) {
  for (int16_t i = y; i < y + h; i++) {
    gc_prim_16bit_set_pixel(handle, x, i, color);
  }
}

void gc_prim_16bit_draw_hline(gc_handle_t *handle, int16_t x, int16_t y,
                              int16_t w, uint16_t color) {
  for (int16_t i = x; i < x + w; i++) {
    gc_prim_16bit_set_pixel(handle, i, y, color);
  }
}

void gc_prim_16bit_fill_rect(gc_handle_t *handle, int16_t x, int16_t y,
                             int16_t w, int16_t h, uint16_t color) {
  for (int16_t i = x; i < x + w; i++) {
    gc_prim_16bit_draw_vline(handle, i, y, h, color);
  }
}

void gc_prim_16bit_fill_screen(gc_handle_t *handle, uint16_t color) {
  for (int16_t y = 0; y < handle->device_height; y++) {
    for (int16_t x = 0; x < handle->device_width; x++) {
      uint32_t idx = ((y * handle->device_width) + x) * 2;
      handle->buffer[idx] = color >> 8;
      handle->buffer[idx + 1] = color;
    }
  }
}
