/* standard */
#include <signal.h>
#include <pthread.h>
/* cinclude */
#include "common.h"
#include "bm/timer.h"
/* global */
#include "mtpipe.h"

void *thr_1(void *arg)
{
    mtnode_handle h = arg;

    int * p_num = NULL;

    TALLOC(p_num, goto done);

    CHECK(mtnode_output(h, p_num), goto done);

    mtpipe_sync_init(h);

    while(!mtnode_is_done(h)) {

        int * ptr;

        CHECK(mtnode_outget(h, (void **)&ptr), break);

        DBG("pid %d: num %d\n",
                pthread_mach_thread_np(pthread_self()),
                *ptr);

        *ptr += 1;

        CHECK(mtnode_output(h, ptr), break);
    }

done:
    mtpipe_sync_finish(h);

    FREE(p_num);

    return MTPIPE_THREAD_SUCCESS;
}

void *thr_2(void *arg)
{
    mtnode_handle h = arg;

    mtpipe_sync_init(h);

    while(!mtnode_is_done(h)) {

        int *ptr;

        CHECK(mtnode_inget(h, (void **)&ptr), break);

        DBG("pid %d: num %d\n",
                pthread_mach_thread_np(pthread_self()),
                *ptr);

        *ptr += 1;

        if(*ptr > 100*10000) {
            break;
        }

        CHECK(mtnode_input(h, ptr), break);

    }

    mtpipe_sync_finish(h);

    kill(getpid(), SIGINT);

    return MTPIPE_THREAD_SUCCESS;
}

    static bool
__build_pipeline(mtpipe_handle mtp)
{
    mtnode_handle node1 = 
        mtnode_create(mtp, "node1", thr_1, 0, NULL);

    mtnode_handle node2 = 
        mtnode_create(mtp, "node2", thr_2, 0, NULL);

    mtnode_handle node3 = 
        mtnode_create(mtp, "node3", thr_2, 0, NULL);

    ASSERT(mtpipe_chain(node1, node2), return false);
    ASSERT(mtpipe_extend(node2, node3), return false);
    ASSERT(mtpipe_bind(node3, node1), return false);

    return true;
}

int main(int argc, char **argv)
{
    mtpipe_handle mtp = mtpipe_create(NULL);

    ASSERT(__build_pipeline(mtp), return 1);

    sigset_t mask;
    sigfillset(&mask);

    ASSERT(pthread_sigmask(SIG_SETMASK, &mask, NULL) == 0,
            return 1);

    struct bmtimer_obj bmtimer = {};

    bmtimer_lap_start(&bmtimer);

    ASSERT(mtpipe_start(mtp), return 1);

    sigset_t wait;
    sigemptyset(&wait);
    sigaddset(&wait, SIGTERM);
    sigaddset(&wait, SIGINT);

    int sig;

    sigwait(&wait, &sig);

    bmtimer_lap_end(&bmtimer);

    bmtimer_print(&bmtimer, __FUNCTION__);

    mtpipe_done(mtp);

    DUMPD(sig);

    ASSERT(mtpipe_converge(mtp), NOP);

    mtpipe_delete(mtp);

    return 0;
}
