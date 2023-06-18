#include "functions.h"
#include <assert.h>

//#define DELAY

static void cleanUpThread(void *arg)
{
    Queue *queue = (Queue *) arg;
    assert(queue == arg);
    if(queue->isFileOpen)
        fclose(queue->file);   
}

void getDataFromProc(void *arg)
{
    int result;
    Queue *queue = (Queue *)arg;
    assert(queue == arg);

    assert((queue->file = fopen("/proc/stat", "r")) != NULL);

    queue->isFileOpen = true; // file is open set flag for true

    for (unsigned i = 0; i < queue->cores; i++)
    {
        assert(sem_wait(&queue->semaphore[0]) == 0);

        // get stats
        result = fscanf(queue->file, "%*s %llu %llu %llu %llu %llu %llu %llu %llu %*s %*s",
                        &queue->data[queue->insert].user[i], &queue->data[queue->insert].nice[i], &queue->data[queue->insert].system[i],
                        &queue->data[queue->insert].idle[i], &queue->data[queue->insert].iowait[i], &queue->data[queue->insert].irq[i], &queue->data[queue->insert].softirq[i],
                        &queue->data[queue->insert].steal[i]);

        assert(result != -1);

        
        assert(sem_post(&queue->semaphore[1]) == 0);
    }

    queue->isFileOpen = fclose(queue->file); // close file set flag for false
    assert(queue->isFileOpen == 0);
}

void *reader(void *arg)
{
    Queue *queue = (Queue *)arg;
    assert(queue == arg);

    queue->insert = 0; // set buffer intex

    pthread_cleanup_push(cleanUpThread, queue)
    while (1)
    {
        assert(pthread_mutex_lock(&queue->mutex[0]) == 0);
        queue->lastActivity[0] = time(NULL); // send current time to watchdog
        assert(pthread_mutex_unlock(&queue->mutex[0]) == 0);

        //simulate unresponsive
        #ifdef DELAY
            sleep(4);
        #endif  

        getDataFromProc((void *)queue);

        sleep(1); // sleep for next measure

        CHANGE_INDEX(queue->insert);
    }
    pthread_cleanup_pop(1);

    return NULL;
}
