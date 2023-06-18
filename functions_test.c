#include "functions.h"
#include <assert.h>

void *allocateQueue(unsigned cores)
{
    Queue *queue = (Queue *)malloc(sizeof(Queue)); // allocate memory for queue struct
    assert(queue != NULL);

    queue->data = (Data *)calloc(2, sizeof(Data)); // allocate memory for data struct
    assert(queue->data != NULL);

    queue->count = (Count *)calloc(2, sizeof(Count)); // allocate memory for count struct
    assert(queue->count != NULL);

    // allocate memory for inside pointers
    for (int i = 0; i < 2; i++)
    {
        queue->data[i].user = (unsigned long long *)calloc(cores, sizeof(unsigned long long));
        assert(queue->data[i].user != NULL);

        queue->data[i].nice = (unsigned long long *)calloc(cores, sizeof(unsigned long long));
        assert(queue->data[i].nice != NULL);

        queue->data[i].system = (unsigned long long *)calloc(cores, sizeof(unsigned long long));
        assert(queue->data[i].system != NULL);

        queue->data[i].idle = (unsigned long long *)calloc(cores, sizeof(unsigned long long));
        assert(queue->data[i].idle != NULL);

        queue->data[i].iowait = (unsigned long long *)calloc(cores, sizeof(unsigned long long));
        assert(queue->data[i].iowait != NULL);

        queue->data[i].irq = (unsigned long long *)calloc(cores, sizeof(unsigned long long));
        assert(queue->data[i].irq != NULL);

        queue->data[i].softirq = (unsigned long long *)calloc(cores, sizeof(unsigned long long));
        assert(queue->data[i].softirq != NULL);

        queue->data[i].steal = (unsigned long long *)calloc(cores, sizeof(unsigned long long));
        assert(queue->data[i].steal != NULL);

        queue->count[i].idle = (unsigned long long *)calloc(cores, sizeof(unsigned long long));
        assert(queue->count[i].idle != NULL);

        queue->count[i].nonIdle = (unsigned long long *)calloc(cores, sizeof(unsigned long long));
        assert(queue->count[i].nonIdle != NULL);
    }

    queue->CPU_Percentage = (double *)calloc(cores, sizeof(double)); // allocate memory for printer, analyzer ring buff
    assert(queue->CPU_Percentage != NULL);

    queue->CPU_Usage = (double *)calloc(cores, sizeof(double)); // allocate memory for printer buff
    assert(queue->CPU_Usage != NULL);

    queue->lastActivity = (time_t *)calloc(3, sizeof(time_t)); // allocate memory for watchdog
    assert(queue->lastActivity != NULL);

    queue->mutex = (pthread_mutex_t *)calloc(6, sizeof(pthread_mutex_t)); // allocate memory for mutexes
    assert(queue->mutex != NULL);

    queue->semaphore = (sem_t *)calloc(4, sizeof(sem_t)); // allocate memory for semaphores
    assert(queue->semaphore != NULL);

    return (void *)queue; // return pointer
}

void freeQueue(void *arg)
{
    Queue *queue = (Queue *)arg;
    assert(queue == arg);

    // free memory inside structs
    for (int i = 0; i < 2; i++)
    {
        // data
        free(queue->data[i].user);
        free(queue->data[i].nice);
        free(queue->data[i].system);
        free(queue->data[i].idle);
        free(queue->data[i].iowait);
        free(queue->data[i].irq);
        free(queue->data[i].softirq);
        free(queue->data[i].steal);

        // count
        free(queue->count[i].idle);
        free(queue->count[i].nonIdle);
    }

    free(queue->data);
    free(queue->count);
    free(queue->CPU_Percentage);
    free(queue->CPU_Usage);
    free(queue->lastActivity);
    free(queue->mutex);
    free(queue->semaphore);
    free(queue);
}
