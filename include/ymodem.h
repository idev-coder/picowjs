#ifndef __PICOWJS_YMODEM_H_
#define __PICOWJS_YMODEM_H_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef enum {
  PICOWJS_YMODEM_OK = 0,
  PICOWJS_YMODEM_ERROR,
  PICOWJS_YMODEM_ABORT,
  PICOWJS_YMODEM_TIMEOUT,
  PICOWJS_YMODEM_DATA,
  PICOWJS_YMODEM_LIMIT
} picowjs_ymodem_status_t;

typedef int (*picowjs_ymodem_header_cb)(uint8_t *file_name, size_t file_size);
typedef int (*picowjs_ymodem_packet_cb)(uint8_t *data, size_t len);
typedef void (*picowjs_ymodem_footer_cb)();
picowjs_ymodem_status_t picowjs_ymodem_receive(picowjs_ymodem_header_cb header_cb,
                                     picowjs_ymodem_packet_cb packet_cb,
                                     picowjs_ymodem_footer_cb footer_cb);

#endif /* __PICOWJS_YMODEM_H_ */
