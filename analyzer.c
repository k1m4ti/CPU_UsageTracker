/*
*   Created 19/06/23 by Mateusz Glab
*   The analyzer gets informations from the reader thread thanks a buffer ring.
*   Then calculates the CPU usage based on the received data. And then it sends
*   CPU usage data to the printer thread.
*   To synchronize with other threads, the analyzer thread utilizes semaphores and mutexes.
*/

#include "functions.h"

// uncomment below to simmulate unresponsive
// #define DELAY

static void getDataFromReader(void *arg)
{
    Queue *queue = (Queue *)arg; // rzutowanie

    for (unsigned i = 0; i < queue->cores; i++)
    {
        if (sem_wait(&queue->semaphore[1]) != 0)
        {
            fprintf(stderr, "Semaphore 1 wait error\n");
            pthread_exit(NULL);
        }

        queue->count[queue->extract].idle[i] = queue->data[queue->extract].idle[i] + queue->data[queue->extract].iowait[i];

        queue->count[queue->extract].nonIdle[i] = queue->data[queue->extract].user[i] + queue->data[queue->extract].nice[i] + queue->data[queue->extract].system[i] +
                                                  queue->data[queue->extract].irq[i] + queue->data[queue->extract].softirq[i] + queue->data[queue->extract].steal[i];

        if (sem_post(&queue->semaphore[0]) != 0)
        {
            fprintf(stderr, "Semaphore 0 post error\n");
            pthread_exit(NULL);
        }
    }
}

static void sendDataToPrinter(void *arg)
{
    Queue *queue = (Queue *)arg;

    for (unsigned i = 0; i < queue->cores; i++)
    {
        if (sem_wait(&queue->semaphore[2]) != 0)
        {
            fprintf(stderr, "Semaphore 2 wait error\n");
            pthread_exit(NULL);
        }

        // count percent
        queue->CPU_Percentage[i] = 1.0 - (double)(queue->count[1].idle[i] - queue->count[0].idle[i]) / (double)(queue->count[1].idle[i] - queue->count[0].idle[i] +
                                                                                                                queue->count[1].nonIdle[i] - queue->count[0].nonIdle[i]);

        if (sem_post(&queue->semaphore[3]) != 0)
        {
            fprintf(stderr, "Semaphore 3 post error\n");
            pthread_exit(NULL);
        }
    }
}

void *analyzer(void *arg)
{
    Queue *queue = (Queue *)arg;

    queue->extract = 0; // set index for first element of buffer

    while (1)
    {
        if (pthread_mutex_lock(&queue->mutex[1]) != 0)
        {
            fprintf(stderr, "Mutex 1 lock error\n");
            pthread_exit(NULL);
        }

        queue->lastActivity[1] = time(NULL); // send current time to watchdog

        if (pthread_mutex_unlock(&queue->mutex[1]) != 0)
        {
            fprintf(stderr, "Mutex 1 unlock error\n");
            pthread_exit(NULL);
        }

// simulate unresponsive
#ifdef DELAY
        sleep(4);
#endif

        if (pthread_mutex_lock(&queue->mutex[4]) != 0)
        {
            fprintf(stderr, "Mutex 4 lock error\n");
            pthread_exit(NULL);
        }

        getDataFromReader((void *)queue);

        if (pthread_mutex_unlock(&queue->mutex[4]) != 0)
        {
            fprintf(stderr, "Mutex 4 unlock error\n");
            pthread_exit(NULL);
        }

        // counted send to printer
        if (queue->extract == 1)
            sendDataToPrinter((void *)queue);

        CHANGE_INDEX(queue->extract);
    }

    return NULL;
}
