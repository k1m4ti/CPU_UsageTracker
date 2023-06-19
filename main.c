/*
*   Created 19/06/23 by Mateusz Glab
*   Program creates 5 threads: reader, analyzer, printer, watchdog, logger.
*   Each thread performs its tasks while the program wait for a signal.
*   To terminate program send SIGTERM or SIGINT (^C).
*/

#include <signal.h>
#include <string.h>
#include "functions.h"

static Queue *queue;
static pthread_t readerThread, analyzerThread, printerThread, watchdogThread, loggerThread;

static void cleanUp(void)
{
    // unlock mutexes
    for (int i = 0; i < 6; i++)
    {
        if (pthread_mutex_unlock(&queue->mutex[i]) != 0)
        {
            fprintf(stderr, "Failed to unlock %d mutex\n", i);
            _exit(1);
        }
    }

    // exit threads
    if (pthread_cancel(readerThread) != 0)
    {
        fprintf(stderr, "Failed to cancel reader thread\n");
        _exit(1);
    }

    if (pthread_cancel(analyzerThread) != 0)
    {
        fprintf(stderr, "Failed to cancel analyzer thread\n");
        _exit(1);
    }
    if (pthread_cancel(printerThread) != 0)
    {
        fprintf(stderr, "Failed to cancel printer thread\n");
        _exit(1);
    }
    if (pthread_cancel(loggerThread) != 0)
    {
        fprintf(stderr, "Failed to cancel logger thread\n");
        _exit(1);
    }

    if (!queue->noRespond)
    {
        if (pthread_cancel(watchdogThread) != 0)
        {
            fprintf(stderr, "Failed to cancel watchdog thread\n");
            _exit(1);
        }
    }

    // wait for threads
    if (pthread_join(readerThread, NULL) != 0)
    {
        fprintf(stderr, "Failed to join reader thread\n");
        _exit(1);
    }

    if (pthread_join(analyzerThread, NULL) != 0)
    {
        fprintf(stderr, "Failed to join analyzer thread\n");
        _exit(1);
    }

    if (pthread_join(printerThread, NULL) != 0)
    {
        fprintf(stderr, "Failed to join printer thread\n");
        _exit(1);
    }

    if (pthread_join(loggerThread, NULL) != 0)
    {
        fprintf(stderr, "Failed to join logger thread\n");
        _exit(1);
    }

    if (!queue->noRespond)
    {
        if (pthread_join(watchdogThread, NULL) != 0)
        {
            fprintf(stderr, "Failed to join watchdog thread\n");
            _exit(1);
        }
    }

    freeQueue((void *)queue);
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

    // function called at exit
    if (atexit(cleanUp) != 0)
    {
        fprintf(stderr, "Atexit error\n");
        exit(EXIT_FAILURE);
    }

    // handle signals
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);

    cores = (unsigned)sysconf(_SC_NPROCESSORS_ONLN) + 1; // get amount of processor cores for iterate loops

    queue = (Queue *)allocateQueue(cores);
    if (queue == NULL)
    {
        fprintf(stderr, "Failed to allocate queue\n");
        exit(EXIT_FAILURE);
    }

    queue->signal = false; // set false value waiting for signal

    queue->cores = cores; // assigning the number of cores to a variable in the queue

    // initate the semaphores
    if (sem_init(&queue->semaphore[0], 0, 2) != 0)
    {
        fprintf(stderr, "Failed to init semaphore 0\n");
        exit(EXIT_FAILURE);
    }

    if (sem_init(&queue->semaphore[1], 0, 0) != 0)
    {
        fprintf(stderr, "Failed to init semaphore 1\n");
        exit(EXIT_FAILURE);
    }

    if (sem_init(&queue->semaphore[2], 0, 1) != 0)
    {
        fprintf(stderr, "Failed to init semaphore 2\n");
        exit(EXIT_FAILURE);
    }

    if (sem_init(&queue->semaphore[3], 0, 0) != 0)
    {
        fprintf(stderr, "Failed to init semaphore 3\n");
        exit(EXIT_FAILURE);
    }

    // initiate the mutexes
    for (int i = 0; i < 6; i++)
    {
        if (pthread_mutex_init(&queue->mutex[i], NULL) != 0)
        {
            fprintf(stderr, "Failed to init mutex %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // create threads
    if (pthread_create(&readerThread, NULL, reader, (void *)queue) != 0)
    {
        fprintf(stderr, "Failed to create reader thread\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&analyzerThread, NULL, analyzer, (void *)queue) != 0)
    {
        fprintf(stderr, "Failed to create analyzer thread\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&printerThread, NULL, printer, (void *)queue) != 0)
    {
        fprintf(stderr, "Failed to create printer thread\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&watchdogThread, NULL, watchdog, (void *)queue) != 0)
    {
        fprintf(stderr, "Failed to create watchdog thread\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&loggerThread, NULL, logger, (void *)queue) != 0)
    {
        fprintf(stderr, "Failed to create logger thread\n");
        exit(EXIT_FAILURE);
    }

    // wait for threads
    if (pthread_join(watchdogThread, NULL) != 0)
    {
        fprintf(stderr, "Failed to join watchdog thread\n");
        exit(EXIT_FAILURE);
    }

    if (queue->noRespond) // if watchdog exit program
        exit(EXIT_FAILURE);

    if (pthread_join(readerThread, NULL) != 0)
    {
        fprintf(stderr, "Failed to join reader thread\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(analyzerThread, NULL) != 0)
    {
        fprintf(stderr, "Failed to join analyzer thread\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(printerThread, NULL) != 0)
    {
        fprintf(stderr, "Failed to join printer thread\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(loggerThread, NULL) != 0)
    {
        fprintf(stderr, "Failed to join logger thread\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
