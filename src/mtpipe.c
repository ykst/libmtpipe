#include <stdio.h>
/* standard */
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
/* global */
#include "common.h"
#include "ds/list.h"
/* local */
#include "mtpipe.h"
#include "rendezvous.h"
#include "fifo.h"

struct __mtnode_obj {
    fifo_handle f_input;
    fifo_handle f_inget;
    fifo_handle f_output;
    fifo_handle f_outget;
    void *arg;
    void *priv;
    void *status;
    void *(*fxn)(void *);
    int priority;
    int id;
    bool started;
    char *name;
    pthread_t pthread;
    mtpipe_handle mtpipe;
    struct list_t list_entry;
}; 

struct __mtpipe_obj {
    void *arg;
    rendezvous_handle rv_init;
    rendezvous_handle rv_cleanup;
    struct list_head_t fifo_list;
    struct list_head_t node_list;
    int id_max;
    bool done;
}; 

struct __fifo_elem_t {
    struct list_t list_entry;
    fifo_handle fifo;
};

static int __fifo_connect(mtpipe_handle pool, fifo_handle *lhs, fifo_handle *rhs);
static bool __fifo_elem_cleaner(struct list_t *e);
static inline int __add_fifo_elem(mtpipe_handle pool, fifo_handle fifo);
static bool __start_thread(mtnode_handle node);

static bool __fifo_elem_cleaner(struct list_t *e)
{
    struct __fifo_elem_t *p;
    list_downcast(p,e);

    if(p->fifo) {
        fifo_delete(p->fifo);
    }

    FREE(p);

    return true;
}

static int __add_fifo_elem(mtpipe_handle h, fifo_handle fifo)
{
    struct list_head_t *p_head = &h->fifo_list;
    struct __fifo_elem_t *nh = NULL;

    TALLOC(nh, return false);

    nh->fifo = fifo;

    list_push(p_head,&nh->list_entry);

    return true;
}

void mtpipe_delete(mtpipe_handle h)
{
    if(!h) return;

    rendezvous_delete(h->rv_init);
    rendezvous_delete(h->rv_cleanup);

    list_destroy(&h->fifo_list);
    list_destroy(&h->node_list);

    FREE(h);
}

static bool __mtnode_cleaner(struct list_t *e)
{
    mtnode_handle h;

    list_downcast(h, e);

    FREE(h->priv);
    FREE(h->name);
    FREE(h);

    return true;
}

mtpipe_handle mtpipe_create(void *arg)
{
    mtpipe_handle nh = NULL;

    TALLOC(nh,return NULL);

    ASSERT(nh->rv_init = rendezvous_create(), goto error);
    ASSERT(nh->rv_cleanup = rendezvous_create(), goto error);

    nh->arg = arg;

    nh->fifo_list.cleaner = __fifo_elem_cleaner;
    nh->node_list.cleaner = __mtnode_cleaner;

    return nh;

error:
    mtpipe_delete(nh);
    return NULL;
}

bool mtpipe_start(mtpipe_handle h)
{
    DBG("Starting %d threads\n",h->id_max);

    ASSERT(rendezvous_init_count(h->rv_init, h->id_max),
            return false);

    ASSERT(rendezvous_init_count(h->rv_cleanup, h->id_max),
            return false);

    mtnode_handle node;

    list_foreach(&h->node_list, node) {
        ASSERT(__start_thread(node), return false);
    }

    return true;
}

bool mtpipe_converge(mtpipe_handle h)
{
    void *status;

    int ret = true;

    mtnode_handle node;

    list_foreach(&h->node_list, node) {

        status = NULL;

        if(node->started){
            if(pthread_join(node->pthread, &status) == 0) {
                if(status != MTPIPE_THREAD_SUCCESS) {
                    ret = false;
                }
            } else {
                ret = false;
            }

            node->started = false;

            DBG("join up %s thread (id.%d): status %p\n",node->name,node->id, status);

        } else {
            ERR("aborted %s thread\n",node->name);
            ret = false;
        }
    }

    return ret;
}

mtnode_handle mtnode_create(
        mtpipe_handle h,
        const char *name,
        void * (*fxn)(void *),
        int priority, 
        void * params)
{
    DASSERT(name, return NULL);
    DASSERT(fxn, return NULL);

    mtnode_handle nh = NULL;

    TALLOC(nh, return NULL);

    nh->fxn        = fxn;
    nh->priority   = priority;
    nh->name       = strdup(name);
    nh->arg        = h->arg;
    nh->priv       = params;
    nh->mtpipe     = h;
    nh->id         = h->id_max++;

    list_append(&h->node_list, &nh->list_entry);

    return nh;
}

bool mtpipe_sync_init(mtnode_handle h)
{
    ASSERT(h, return false);

    DBG("%s thread successfully initialized\n",h->name);

    ASSERT(rendezvous_meet(h->mtpipe->rv_init), return false);

    DBG("entering %s thread main loop\n",h->name);

    return true;
}

    static bool 
__start_thread(mtnode_handle node)
{
    struct sched_param  schedParam;
    pthread_attr_t attr;

    DASSERT(node->fxn,return false);

    /* Initialize the thread attributes */
    ASSERT(pthread_attr_init(&attr) == 0, return false);

    /* Force the thread to use custom scheduling attributes */
    ASSERT(pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED) == 0,
            return false);

    if(node->priority > 0) {
        /* Set the thread to be fifo real time scheduled */
        ASSERT(pthread_attr_setschedpolicy(&attr, SCHED_FIFO) == 0,
                return false);

        schedParam.sched_priority = node->priority;

        ASSERT(pthread_attr_setschedparam(&attr, &schedParam) == 0,
                return false);

        DBG("%s set to SCHED_FIFO, priority = %d\n",node->name, node->priority);
    } else {
        /* Set the thread to be time-slice based */
        ASSERT(pthread_attr_setschedpolicy(&attr, SCHED_OTHER) == 0,
                return false);

        DBG("%s set to SCHED_OTHER\n",node->name);
    }

    ASSERT(pthread_create(&node->pthread, &attr, node->fxn, node) == 0,
            return false);

    node->started = true;

    return true;
}

    bool
mtpipe_sync_finish(mtnode_handle h)
{
    ASSERT(h, return false);

    DBG("%s thread waiting for cleanup\n",h->name);

    mtnode_done(h);

    if(h->f_output != NULL) {
        fifo_flush(h->f_output);
    }

    if(h->f_input != NULL) {
        fifo_flush(h->f_input);
    }

    rendezvous_force(h->mtpipe->rv_init);

    ASSERT(rendezvous_meet(h->mtpipe->rv_cleanup), return false);

    return true;
}

static int __fifo_connect(mtpipe_handle pool, fifo_handle *lhs, fifo_handle *rhs)
{
    if(*lhs == NULL && *rhs == NULL) {
        ASSERT(*lhs = fifo_create(),return false);
        ASSERT(__add_fifo_elem(pool,*lhs), return false);
        *rhs = *lhs;
    } else if(*lhs != NULL) {
        *rhs = *lhs;
    } else if(*rhs != NULL) {
        *lhs = *rhs;
    } else {
        ERR("duplicated thread connection detected\n");
        return false;
    }

    return true;
}


/* connect OUTPUT<->INGET and OUTGET<->INPUT 
 * the method is actually the composition of 'chain' and 'band'. 
 * it is only introduced because the connection is the most widely used method of piplined threads */
bool mtpipe_joint(mtnode_handle lhs, mtnode_handle rhs) 
{
    ASSERT(lhs,return false);
    ASSERT(rhs,return false);
    ASSERT(lhs->mtpipe, return false);

    ASSERT(__fifo_connect(lhs->mtpipe,&lhs->f_output,&rhs->f_inget), return false);
    ASSERT(__fifo_connect(lhs->mtpipe,&lhs->f_outget,&rhs->f_input), return false);

    return true;
}

/* connect OUTPUT<->INGET 
 * 'chain' means the start of new subgraph in threads connection, */
bool mtpipe_chain(mtnode_handle lhs, mtnode_handle rhs) 
{
    ASSERT(lhs,return false);
    ASSERT(rhs,return false);
    ASSERT(lhs->mtpipe, return false);

    ASSERT(__fifo_connect(lhs->mtpipe, &lhs->f_output,&rhs->f_inget), return false);

    return true;
}

/* connect INPUT<->INGET 
 * 'extend' means the extention of a subgraph with then same input buffers circurated in the ring */
bool mtpipe_extend(mtnode_handle lhs, mtnode_handle rhs) 
{
    ASSERT(lhs,return false);
    ASSERT(rhs,return false);
    ASSERT(lhs->mtpipe, return false);

    ASSERT(__fifo_connect(lhs->mtpipe, &lhs->f_input,&rhs->f_inget), return false);

    return true;
}

/* connect INPUT<->OUTGET
 * 'band' means the conjucted end of subgraph, which started by 'chain' and extended by 'extend'.*/
bool mtpipe_bind(mtnode_handle lhs, mtnode_handle rhs) 
{
    ASSERT(lhs,return false);
    ASSERT(rhs,return false);
    ASSERT(lhs->mtpipe, return false);

    ASSERT(__fifo_connect(lhs->mtpipe, &lhs->f_input,&rhs->f_outget), return false);

    return true;
}

bool mtnode_is_done(mtnode_handle h)
{
    return (h != NULL) ? mtpipe_is_done(h->mtpipe) : true;
}

bool mtpipe_is_done(mtpipe_handle h)
{
    return (h != NULL) ? h->done : true;
}

bool mtnode_done(mtnode_handle h)
{
    return h != NULL ? mtpipe_done(h->mtpipe) : true;
}

bool mtpipe_done(mtpipe_handle h)
{
    ASSERT(h, return false);

    h->done = true;

    return true;
}

bool mtnode_inget(mtnode_handle h, void **ptr)
{
    DASSERT(h, return false);

    return fifo_get(h->f_inget, ptr);
}

bool mtnode_outget(mtnode_handle h, void **ptr)
{
    DASSERT(h, return false);

    return fifo_get(h->f_outget, ptr);
}

bool mtnode_input(mtnode_handle h, void *ptr)
{
    DASSERT(h, return false);

    return fifo_put(h->f_input, ptr);
}

bool mtnode_output(mtnode_handle h, void *ptr)
{
    DASSERT(h, return false);

    return fifo_put(h->f_output, ptr);
}
