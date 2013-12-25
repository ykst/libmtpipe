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
/* Handle allocation/deallocation */
mtpipe_handle mtpipe_create(void *arg) __attribute__((warn_unused_result));

mtnode_handle mtnode_create(
        mtpipe_handle h,
        const char *name,
        void * (*fxn)(void *),
        int priority, 
        void * params) __attribute__((warn_unused_result));

void mtpipe_delete(mtpipe_handle h);
/* Pipeline construction */
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
/* Pipeline condition query */
bool mtnode_is_done(mtnode_handle h) __attribute__((warn_unused_result));
bool mtpipe_is_done(mtpipe_handle h) __attribute__((warn_unused_result));
/* Local parameter */
void *mtnode_priv(mtnode_handle);
/* Dataflow */
bool mtnode_inget(mtnode_handle h, void **ptr) __attribute__((warn_unused_result));
bool mtnode_outget(mtnode_handle h, void **ptr) __attribute__((warn_unused_result));
bool mtnode_input(mtnode_handle h, void *ptr) __attribute__((warn_unused_result));
bool mtnode_output(mtnode_handle h, void *ptr) __attribute__((warn_unused_result));
int mtnode_num_inget(mtnode_handle h) __attribute__((warn_unused_result));
int mtnode_num_outget(mtnode_handle h) __attribute__((warn_unused_result));
int mtnode_num_input(mtnode_handle h) __attribute__((warn_unused_result));
int mtnode_num_output(mtnode_handle h) __attribute__((warn_unused_result));
/* Terminate pipeline */
bool mtnode_done(mtnode_handle h);
bool mtpipe_done(mtpipe_handle h);
/* Pause pipeline */
void mtnode_pause_wait(mtnode_handle h);
void mtpipe_pause_on(mtpipe_handle h);
void mtpipe_pause_off(mtpipe_handle h);
bool mtpipe_is_pause(mtpipe_handle h);
/* Synchronize control points */
bool mtpipe_sync_init(mtnode_handle h);
bool mtpipe_sync_finish(mtnode_handle h);
/* Activate Pipeline */
bool mtpipe_start(mtpipe_handle h) __attribute__((warn_unused_result));
/* Wait for pipeline */
bool mtpipe_converge(mtpipe_handle h) __attribute__((warn_unused_result));

#ifdef __cplusplus
}
#endif

#endif
