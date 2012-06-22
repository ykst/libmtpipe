#include "rendezvous.h"

struct __rendezvous_obj {
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    int             orig;
    int             count;
    bool            force;
};

rendezvous_handle
rendezvous_create(void)
{
    rendezvous_handle h = NULL;

    TALLOC(h, return NULL);

    ASSERT(pthread_mutex_init(&h->mutex, NULL) == 0,
            goto error);

    ASSERT(pthread_cond_init(&h->cond, NULL) == 0,
            goto error);

    return h;
error:

    rendezvous_delete(h);

    return NULL;
}

void 
rendezvous_delete(rendezvous_handle h)
{
    if(!h) return;

    pthread_mutex_destroy(&h->mutex);
    pthread_cond_destroy(&h->cond);

    FREE(h);
}

bool
rendezvous_init_count(rendezvous_handle h, int count)
{
	ASSERT(h, return false);

	h->count = count;

	return true;
}

bool
rendezvous_meet(
        rendezvous_handle h)
{
    DASSERT(h != NULL, return false);

    bool ret = false;

    pthread_mutex_lock(&h->mutex);
    
    if(!h->force) {
    
        if(h->count > 0) {
            --h->count;
        }
    
        if(h->count > 0 ) {

            pthread_cond_wait(&h->cond, &h->mutex);

        } else {

            pthread_cond_broadcast(&h->cond);

            h->count = h->orig;
        }
    
        ret = true;
    }

    pthread_mutex_unlock(&h->mutex);

    return ret;
}

void rendezvous_force(rendezvous_handle h)
{
    ASSERT(h,return);

    pthread_mutex_lock(&h->mutex);

    h->force = true;

    pthread_cond_broadcast(&h->cond);

    pthread_mutex_unlock(&h->mutex);
}

void rendezvous_reset(rendezvous_handle h)
{
    ASSERT(h,return);

    pthread_mutex_lock(&h->mutex);

    pthread_cond_broadcast(&h->cond);

    h->count = h->orig;
    h->force = false;

    pthread_mutex_unlock(&h->mutex);
}
