#include "fifo.h"

struct __fifo_obj {
    pthread_mutex_t mutex;
    bool flush;
    int rfd;
    int wfd;
    int num_bufs;
    int pipes[2];
};

fifo_handle fifo_create(void)
{
    fifo_handle     h = NULL;

    TALLOC(h, goto error);

    ASSERT(pipe(h->pipes) == 0,
            goto error);

    h->rfd = h->pipes[0];
    h->wfd = h->pipes[1];

    pthread_mutex_init(&h->mutex, NULL);

    return h;
error:

    fifo_delete(h);

    return NULL;
}

void fifo_delete(fifo_handle h)
{

    if (!h) return;

    close(h->pipes[0]);
    close(h->pipes[1]);

    pthread_mutex_destroy(&h->mutex);

    FREE(h);
}

bool fifo_get(fifo_handle h, void **ptr)
{
    size_t  num_bytes;
    bool flush;

    DASSERT(h,return false);
    DASSERT(ptr, return false);

    pthread_mutex_lock(&h->mutex);
    flush = h->flush;
    pthread_mutex_unlock(&h->mutex);

    if (flush) {
        return false;
    }

    num_bytes = read(h->pipes[0], ptr, sizeof(ptr));

    if (num_bytes != sizeof(ptr)) {
        return false;
    }

    pthread_mutex_lock(&h->mutex);
    --h->num_bufs;
    pthread_mutex_unlock(&h->mutex);

    return true;
}

bool fifo_is_flushed(fifo_handle h)
{
    return h->flush;
}

bool fifo_flush(fifo_handle h)
{
    char ch = 0xff;

    DASSERT(h, return false);

    pthread_mutex_lock(&h->mutex);
    h->flush = true;
    pthread_mutex_unlock(&h->mutex);

    if (write(h->pipes[1], &ch, 1) != 1) {
        return false;
    }

    return true;
}

bool fifo_put(fifo_handle h, void *ptr)
{
    DASSERT(h,return false);
    DASSERT(ptr,return false);

    pthread_mutex_lock(&h->mutex);
    h->num_bufs++;
    pthread_mutex_unlock(&h->mutex);

    if (write(h->pipes[1], &ptr, sizeof(ptr)) != sizeof(ptr)) {
        return false;
    }

    return true;
}

int fifo_get_num(fifo_handle h)
{
    int             num_bufs;

    ASSERT(h,return 0);

    num_bufs = h->num_bufs;

    return num_bufs;
}
