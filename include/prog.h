#ifndef __PICOWJS_PROG_H
#define __PICOWJS_PROG_H

#include <stdint.h>

#include "board.h"
#include "flash.h"

void picowjs_prog_clear();
void picowjs_prog_begin();
int picowjs_prog_write(uint8_t *buffer, int size);
int picowjs_prog_end();
uint32_t picowjs_prog_get_size();
uint32_t picowjs_prog_max_size();
uint8_t *picowjs_prog_addr();

#endif /* __PICOWJS_PROG_H */
