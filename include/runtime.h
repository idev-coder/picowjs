#ifndef __PICOWJS_RUNTIME_H
#define __PICOWJS_RUNTIME_H

#include "jerryscript.h"

void picowjs_runtime_init(bool load, bool first);
void picowjs_runtime_cleanup();
void picowjs_runtime_load();
void picowjs_runtime_set_vm_stop(uint8_t stop);

#endif /* __PICOWJS_RUNTIME_H */