#ifndef __PICOWJS_UTILS_H
#define __PICOWJS_UTILS_H

#include <stdint.h>

typedef struct picowjs_list_node_s picowjs_list_node_t;
typedef struct picowjs_list_s picowjs_list_t;

struct picowjs_list_node_s {
  picowjs_list_node_t *prev;
  picowjs_list_node_t *next;
};

struct picowjs_list_s {
  picowjs_list_node_t *head;
  picowjs_list_node_t *tail;
};

void picowjs_list_init(picowjs_list_t *list);
void picowjs_list_append(picowjs_list_t *list, picowjs_list_node_t *node);
void picowjs_list_remove(picowjs_list_t *list, picowjs_list_node_t *node);

uint8_t picowjs_hex1(char hex);
uint8_t picowjs_hex2bin(unsigned char *hex);
#endif /* __PICOWJS_UTILS_H */