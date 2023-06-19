/*
*   Created 19/06/23 by Mateusz Glab
*   The reader gets informations from /proc/stat file and using a buffer ring
*   sends it to the analyzer thread.
*   To synchronize with other threads, the reader thread utilizes semaphores and mutexes.
*/

#include "functions.h"

// uncomment below to simmulate unresponsive
// #define DELAY

static void cleanUpThread(void *arg)
{
    Queue *queue = (Queue *)arg;
    if (queue->isFileOpen)
        fclose(queue->file);
}

static void getDataFromProc(void *arg)
{
    int result;
    Queue *queue = (Queue *)arg;

    queue->file = fopen("/proc/stat", "r");
    if (queue->file == NULL)
    {
        fprintf(stderr, "Failed to open /proc/stat\n");
        pthread_exit(NULL);
    }

    queue->isFileOpen = true; // file is open set flag for true

    for (unsigned i = 0; i < queue->cores; i++)
    {
        if (sem_wait(&queue->semaphore[0]) != 0)
        {
            fprintf(stderr, "Semaphore 0 wait error\n");
            pthread_exit(NULL);
        }

        // get stats
        result = fscanf(queue->file, "%*s %llu %llu %llu %llu %llu %llu %llu %llu %*s %*s",
                        &queue->data[queue->insert].user[i], &queue->data[queue->insert].nice[i], &queue->data[queue->insert].system[i],
                        &queue->data[queue->insert].idle[i], &queue->data[queue->insert].iowait[i], &queue->data[queue->insert].irq[i], &queue->data[queue->insert].softirq[i],
                        &queue->data[queue->insert].steal[i]);

        if (result != -1)
        {
            fprintf(stderr, "Fscanf error\n");
            pthread_exit(NULL);
        }

        if (sem_post(&queue->semaphore[1]) != 0)
        {
            fprintf(stderr, "Semaphore 1 post error\n");
            pthread_exit(NULL);
        }
    }

    queue->isFileOpen = fclose(queue->file); // close file set flag for false
}

void *reader(void *arg)
{
    Queue *queue = (Queue *)arg;

    queue->insert = 0; // set buffer intex

    pthread_cleanup_push(cleanUpThread, queue) while (1)
    {
        if (pthread_mutex_lock(&queue->mutex[0]) != 0)
        {
            fprintf(stderr, "Mutex 0 lock error\n");
            pthread_exit(NULL);
        }

        queue->lastActivity[0] = time(NULL); // send current time to watchdog

        if (pthread_mutex_unlock(&queue->mutex[0]) != 0)
        {
            fprintf(stderr, "Mutex 0 unlock error\n");
            pthread_exit(NULL);
        }

// simulate unresponsive
#ifdef DELAY
        sleep(4);
#endif

        if(pthread_mutex_lock(&queue->mutex[3]) != 0)
        {
            fprintf(stderr, "Mutex 3 lock error\n");
            pthread_exit(NULL);
        }

        getDataFromProc((void *)queue);

        if(pthread_mutex_unlock(&queue->mutex[3]) != 0)
        {
            fprintf(stderr, "Mutex 3 unlock error\n");
            pthread_exit(NULL);
        }

        sleep(1); // sleep for next measure

        CHANGE_INDEX(queue->insert);
    }
    pthread_cleanup_pop(1);

    return NULL;
}
