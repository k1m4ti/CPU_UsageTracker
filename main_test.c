#include <signal.h>
#include <string.h>
#include <assert.h>

#include "functions.h"

static Queue *queue;
static pthread_t readerThread, analyzerThread, printerThread, watchdogThread, loggerThread; 

static void cleanUp(void)
{
    freeQueue((void *)queue);

    //exit threads
    assert(pthread_cancel(readerThread) == 0);
    assert(pthread_cancel(analyzerThread) == 0);
    assert(pthread_cancel(printerThread) == 0);

    //wait for threads
    assert(pthread_join(readerThread, NULL) == 0);
    assert(pthread_join(analyzerThread, NULL) == 0);
    assert(pthread_join(printerThread, NULL) == 0);
}

static void term(int signum)
{
    queue->signal = true; // set a flag for information threads
    printf("Signal %d handled\n", signum);
    exit(EXIT_SUCCESS);
}

int main(void)
{
    struct sigaction action;
    unsigned cores;

    atexit(cleanUp); // initaion function called at exit

    // handle signals
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);

    cores = (unsigned)sysconf(_SC_NPROCESSORS_ONLN) + 1; // get amount of processor cores for iterate loops
    assert(cores != 0);

    queue = (Queue *)allocateQueue(cores);
    assert(queue != NULL);

    queue->signal = false; // set false value waiting for signal

    queue->cores = cores; // assigning the number of cores to a variable in the queue

    //initation the semaphores
    assert(sem_init(&queue->semaphore[0], 0, 2) == 0);
    assert(sem_init(&queue->semaphore[1], 0, 0) == 0);
    assert(sem_init(&queue->semaphore[2], 0, 1) == 0);
    assert(sem_init(&queue->semaphore[3], 0, 0) == 0);


    // create threads
    assert(pthread_create(&readerThread, NULL, reader, (void *)queue) == 0);
    assert(pthread_create(&analyzerThread, NULL, analyzer, (void *)queue) == 0);
    assert(pthread_create(&printerThread, NULL, printer, (void *)queue) == 0);

    // wait for threads
    assert(pthread_join(readerThread, NULL) == 0);
    assert(pthread_join(analyzerThread, NULL) == 0);
    assert(pthread_join(printerThread, NULL) == 0);

    return 0;
}
