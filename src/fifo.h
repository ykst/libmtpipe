#ifndef _LIBMTPIPE_FIFO_H_
#define _LIBMTPIPE_FIFO_H_

#include <pthread.h>
#include "common.h"

EXTERN_C_BEGIN

typedef struct __fifo_obj *fifo_handle;

fifo_handle
fifo_create(void) __attribute__((warn_unused_result));

void 
fifo_delete(fifo_handle h);

bool
fifo_meet(
        fifo_handle h,
        struct timeval *tvp) __attribute__((warn_unused_result));

bool fifo_get(fifo_handle h, void **ptr);
bool fifo_put(fifo_handle h, void *ptr);
int fifo_get_num(fifo_handle h);
bool fifo_flush(fifo_handle h);
bool fifo_is_flushed(fifo_handle h);

EXTERN_C_END

#endif
