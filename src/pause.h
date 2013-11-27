#ifndef _LIBMTPIPE_PAUSE_H_
#define _LIBMTPIPE_PAUSE_H_

#include <pthread.h>
#include "common.h"

EXTERN_C_BEGIN

typedef struct __pause_obj *pause_handle;

pause_handle
pause_create(void) __attribute__((warn_unused_result));

void 
pause_on(pause_handle h);

void 
pause_off(pause_handle h);

void 
pause_check_wait(pause_handle h);

bool
pause_check(pause_handle h);

void
pause_delete(
        pause_handle h);

EXTERN_C_END

#endif
