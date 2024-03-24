#ifndef __PWJS_YMODEM_H_
#define __PWJS_YMODEM_H_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef enum {
  PWJS_YMODEM_OK = 0,
  PWJS_YMODEM_ERROR,
  PWJS_YMODEM_ABORT,
  PWJS_YMODEM_TIMEOUT,
  PWJS_YMODEM_DATA,
  PWJS_YMODEM_LIMIT
} pwjs_ymodem_status_t;

typedef int (*pwjs_ymodem_header_cb)(uint8_t *file_name, size_t file_size);
typedef int (*pwjs_ymodem_packet_cb)(uint8_t *data, size_t len);
typedef void (*pwjs_ymodem_footer_cb)();
pwjs_ymodem_status_t pwjs_ymodem_receive(pwjs_ymodem_header_cb header_cb,
                                     pwjs_ymodem_packet_cb packet_cb,
                                     pwjs_ymodem_footer_cb footer_cb);

#endif /* __PWJS_YMODEM_H_ */
