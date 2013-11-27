#include "pause.h"

struct __pause_obj {
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    bool            enabled;
};

    pause_handle
pause_create(void)
{
    pause_handle h = NULL;

    TALLOC(h, return NULL);

    ASSERT(pthread_mutex_init(&h->mutex, NULL) == 0,
            goto error);

    ASSERT(pthread_cond_init(&h->cond, NULL) == 0,
            goto error);

    return h;
error:

    pause_delete(h);

    return NULL;
}

    void 
pause_delete(pause_handle h)
{
    if(!h) return;

    pthread_mutex_destroy(&h->mutex);
    pthread_cond_destroy(&h->cond);

    FREE(h);
}

    void
pause_on(
        pause_handle h)
{
    pthread_mutex_lock(&h->mutex);

    h->enabled = true;

    pthread_mutex_unlock(&h->mutex);
}

    void
pause_off(
        pause_handle h)
{
    pthread_mutex_lock(&h->mutex);

    if (h->enabled) {
        h->enabled = false;
        pthread_cond_broadcast(&h->cond);
    }

    pthread_mutex_unlock(&h->mutex);
}

    void 
pause_check_wait(pause_handle h)
{
    pthread_mutex_lock(&h->mutex);

    if (h->enabled) {
        pthread_cond_wait(&h->cond, &h->mutex);
    }

    pthread_mutex_unlock(&h->mutex);
}

    bool
pause_check(pause_handle h)
{
    return h->enabled;
}
