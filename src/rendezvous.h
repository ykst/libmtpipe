#ifndef _LIBMTPIPE_RENDEZVOUS_H_
#define _LIBMTPIPE_RENDEZVOUS_H_

#include <pthread.h>
#include "common.h"

EXTERN_C_BEGIN

typedef struct __rendezvous_obj *rendezvous_handle;

rendezvous_handle
rendezvous_create(void) __attribute__((warn_unused_result));

void 
rendezvous_delete(rendezvous_handle h);

void 
rendezvous_force(rendezvous_handle h);

void 
rendezvous_reset(rendezvous_handle h);

bool
rendezvous_meet(
        rendezvous_handle h) __attribute__((warn_unused_result));

bool
rendezvous_init_count(rendezvous_handle h, int count) __attribute__((warn_unused_result));


EXTERN_C_END

#endif
