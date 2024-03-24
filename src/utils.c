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

#include "utils.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void pwjs_list_init(pwjs_list_t *list) {
  list->head = NULL;
  list->tail = NULL;
}

void pwjs_list_append(pwjs_list_t *list, pwjs_list_node_t *node) {
  if (list->tail == NULL && list->head == NULL) {
    list->head = node;
    list->tail = node;
    node->next = NULL;
    node->prev = NULL;
  } else {
    list->tail->next = node;
    node->prev = list->tail;
    node->next = NULL;
    list->tail = node;
  }
}

void pwjs_list_remove(pwjs_list_t *list, pwjs_list_node_t *node) {
  if (list->head == node) {
    list->head = node->next;
  }
  if (list->tail == node) {
    list->tail = node->prev;
  }
  if (node->prev != NULL) {
    node->prev->next = node->next;
  }
  if (node->next != NULL) {
    node->next->prev = node->prev;
  }
}

uint8_t pwjs_hex1(char hex) {
  if (hex >= 'a') {
    return (hex - 'a' + 10);
  } else if (hex >= 'A') {
    return (hex - 'A' + 10);
  } else {
    return (hex - '0');
  }
}

uint8_t pwjs_hex2bin(unsigned char *hex) {
  uint8_t hh = pwjs_hex1(hex[0]);
  uint8_t hl = pwjs_hex1(hex[1]);
  return hh << 4 | hl;
}
