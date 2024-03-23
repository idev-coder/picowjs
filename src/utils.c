#include "utils.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void picowjs_list_init(picowjs_list_t *list) {
  list->head = NULL;
  list->tail = NULL;
}

void picowjs_list_append(picowjs_list_t *list, picowjs_list_node_t *node) {
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

void picowjs_list_remove(picowjs_list_t *list, picowjs_list_node_t *node) {
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

uint8_t picowjs_hex1(char hex) {
  if (hex >= 'a') {
    return (hex - 'a' + 10);
  } else if (hex >= 'A') {
    return (hex - 'A' + 10);
  } else {
    return (hex - '0');
  }
}

uint8_t picowjs_hex2bin(unsigned char *hex) {
  uint8_t hh = picowjs_hex1(hex[0]);
  uint8_t hl = picowjs_hex1(hex[1]);
  return hh << 4 | hl;
}
