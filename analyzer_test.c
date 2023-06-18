#include "functions.h"
#include <assert.h>

// #define DELAY

static void getDataFromReader(void *arg)
{
    Queue *queue = (Queue *)arg; // rzutowanie
    assert(queue == arg);

    for (unsigned i = 0; i < queue->cores; i++)
    {
        assert(sem_wait(&queue->semaphore[1]) == 0);

        queue->count[queue->extract].idle[i] = queue->data[queue->extract].idle[i] + queue->data[queue->extract].iowait[i];

        queue->count[queue->extract].nonIdle[i] = queue->data[queue->extract].user[i] + queue->data[queue->extract].nice[i] + queue->data[queue->extract].system[i] +
                                                  queue->data[queue->extract].irq[i] + queue->data[queue->extract].softirq[i] + queue->data[queue->extract].steal[i];

        assert(sem_post(&queue->semaphore[0]) == 0);
    }
}

static void sendDataToPrinter(void *arg)
{
    Queue *queue = (Queue *)arg;
    assert(queue == arg);

    for (unsigned i = 0; i < queue->cores; i++)
    {
        assert(sem_wait(&queue->semaphore[2]) == 0);

        // count percent
        queue->CPU_Percentage[i] = 1.0 - (double)(queue->count[1].idle[i] - queue->count[0].idle[i]) / (double)(queue->count[1].idle[i] - queue->count[0].idle[i] +
                                                                                                                queue->count[1].nonIdle[i] - queue->count[0].nonIdle[i]);

        assert(sem_post(&queue->semaphore[3]) == 0);
    }
}

void *analyzer(void *arg)
{
    Queue *queue = (Queue *)arg;
    assert(queue == arg);

    queue->extract = 0; // set index for first element of buffer

    while (1)
    {
        assert(pthread_mutex_lock(&queue->mutex[1]) == 0);
        queue->lastActivity[1] = time(NULL); // send current time to watchdog
        assert(pthread_mutex_unlock(&queue->mutex[1]) == 0);

// simulate unresponsive
#ifdef DELAY
        sleep(4);
#endif

        assert(pthread_mutex_lock(&queue->mutex[4]) == 0);
        getDataFromReader((void *)queue);
        assert(pthread_mutex_unlock(&queue->mutex[4]) == 0);

        // counted send to printer
        if (queue->extract == 1)
            sendDataToPrinter((void *)queue);

        CHANGE_INDEX(queue->extract);
    }

    return NULL;
}
