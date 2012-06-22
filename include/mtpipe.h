#ifndef _LIBMTPIPE_MTPIPE_H_
#define _LIBMTPIPE_MTPIPE_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __mtpipe_obj * mtpipe_handle;

typedef struct __mtnode_obj * mtnode_handle;

#define MTPIPE_THREAD_SUCCESS ((void*)0)
#define MTPIPE_THREAD_FAILURE ((void*)-1)

mtpipe_handle mtpipe_create(void *arg) __attribute__((warn_unused_result));

mtnode_handle mtnode_create(
        mtpipe_handle h,
        const char *name,
        void * (*fxn)(void *),
        int priority, 
        void * params) __attribute__((warn_unused_result));

bool mtpipe_chain(
        mtnode_handle lhs,
        mtnode_handle rhs) __attribute__((warn_unused_result));

bool mtpipe_extend(
        mtnode_handle lhs,
        mtnode_handle rhs) __attribute__((warn_unused_result));

bool mtpipe_bind(
        mtnode_handle lhs,
        mtnode_handle rhs) __attribute__((warn_unused_result));

bool mtpipe_joint(
        mtnode_handle lhs,
        mtnode_handle rhs) __attribute__((warn_unused_result));

bool mtpipe_start(mtpipe_handle h) __attribute__((warn_unused_result));

bool mtpipe_sync_init(mtnode_handle h);

bool mtnode_is_done(mtnode_handle h) __attribute__((warn_unused_result));
bool mtpipe_is_done(mtpipe_handle h) __attribute__((warn_unused_result));

bool mtnode_inget(mtnode_handle h, void **ptr) __attribute__((warn_unused_result));
bool mtnode_outget(mtnode_handle h, void **ptr) __attribute__((warn_unused_result));
bool mtnode_input(mtnode_handle h, void *ptr) __attribute__((warn_unused_result));
bool mtnode_output(mtnode_handle h, void *ptr) __attribute__((warn_unused_result));

bool mtnode_done(mtnode_handle h);
bool mtpipe_done(mtpipe_handle h);

bool mtpipe_sync_finish(mtnode_handle h);

bool mtpipe_converge(mtpipe_handle h) __attribute__((warn_unused_result));

void mtpipe_delete(mtpipe_handle h);

#ifdef __cplusplus
}
#endif

#endif
