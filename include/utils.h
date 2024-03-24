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

#ifndef __PWJS_UTILS_H
#define __PWJS_UTILS_H

#include <stdint.h>

typedef struct pwjs_list_node_s pwjs_list_node_t;
typedef struct pwjs_list_s pwjs_list_t;

struct pwjs_list_node_s {
  pwjs_list_node_t *prev;
  pwjs_list_node_t *next;
};

struct pwjs_list_s {
  pwjs_list_node_t *head;
  pwjs_list_node_t *tail;
};

void pwjs_list_init(pwjs_list_t *list);
void pwjs_list_append(pwjs_list_t *list, pwjs_list_node_t *node);
void pwjs_list_remove(pwjs_list_t *list, pwjs_list_node_t *node);

uint8_t pwjs_hex1(char hex);
uint8_t pwjs_hex2bin(unsigned char *hex);
#endif /* __PWJS_UTILS_H */