#include "functions.h"

void *allocateQueue(unsigned cores)
{
    Queue *queue = (Queue *)malloc(sizeof(Queue)); // allocate memory for queue struct
    if (queue == NULL)
    {
        fprintf(stderr, "Failed to allocate queue struct\n");
        exit(EXIT_FAILURE);
    }

    queue->data = (Data *)calloc(2, sizeof(Data)); // allocate memory for data struct
    if (queue->data == NULL)
    {
        fprintf(stderr, "Failed to allocate data struct\n");
        exit(EXIT_FAILURE);
    }

    queue->count = (Count *)calloc(2, sizeof(Count)); // allocate memory for count struct
    if (queue->count == NULL)
    {
        fprintf(stderr, "Failed to allocate count struct\n");
        exit(EXIT_FAILURE);
    }

    // allocate memory for inside pointers
    for (int i = 0; i < 2; i++)
    {
        queue->data[i].user = (unsigned long long *)calloc(cores, sizeof(unsigned long long));
        if (queue->data[i].user == NULL)
        {
            fprintf(stderr, "Failed to allocate user %d\n", i);
            exit(EXIT_FAILURE);
        }

        queue->data[i].nice = (unsigned long long *)calloc(cores, sizeof(unsigned long long));
        if (queue->data[i].nice == NULL)
        {
            fprintf(stderr, "Failed to allocate nice %d\n", i);
            exit(EXIT_FAILURE);
        }

        queue->data[i].system = (unsigned long long *)calloc(cores, sizeof(unsigned long long));
        if (queue->data[i].system == NULL)
        {
            fprintf(stderr, "Failed to allocate system %d\n", i);
            exit(EXIT_FAILURE);
        }

        queue->data[i].idle = (unsigned long long *)calloc(cores, sizeof(unsigned long long));
        if (queue->data[i].idle == NULL)
        {
            fprintf(stderr, "Failed to allocate idle data %d\n", i);
            exit(EXIT_FAILURE);
        }

        queue->data[i].iowait = (unsigned long long *)calloc(cores, sizeof(unsigned long long));
        if (queue->data[i].iowait == NULL)
        {
            fprintf(stderr, "Failed to allocate iowait %d\n", i);
            exit(EXIT_FAILURE);
        }

        queue->data[i].irq = (unsigned long long *)calloc(cores, sizeof(unsigned long long));
        if (queue->data[i].irq == NULL)
        {
            fprintf(stderr, "Failed to allocate irq %d\n", i);
            exit(EXIT_FAILURE);
        }

        queue->data[i].softirq = (unsigned long long *)calloc(cores, sizeof(unsigned long long));
        if (queue->data[i].softirq == NULL)
        {
            fprintf(stderr, "Failed to allocate softirq %d\n", i);
            exit(EXIT_FAILURE);
        }

        queue->data[i].steal = (unsigned long long *)calloc(cores, sizeof(unsigned long long));
        if (queue->data[i].steal == NULL)
        {
            fprintf(stderr, "Failed to allocate steal %d\n", i);
            exit(EXIT_FAILURE);
        }

        queue->count[i].idle = (unsigned long long *)calloc(cores, sizeof(unsigned long long));
        if (queue->count[i].idle == NULL)
        {
            fprintf(stderr, "Failed to allocate idle count %d\n", i);
            exit(EXIT_FAILURE);
        }

        queue->count[i].nonIdle = (unsigned long long *)calloc(cores, sizeof(unsigned long long));
        if (queue->count[i].nonIdle == NULL)
        {
            fprintf(stderr, "Failed to allocate nonIdle %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    queue->CPU_Percentage = (double *)calloc(cores, sizeof(double)); // allocate memory for printer, analyzer ring buff
    if (queue->CPU_Percentage == NULL)
    {
        fprintf(stderr, "Failed to allocate CPU_Percentage\n");
        exit(EXIT_FAILURE);
    }

    queue->CPU_Usage = (double *)calloc(cores, sizeof(double)); // allocate memory for printer buff
    if (queue->CPU_Usage == NULL)
    {
        fprintf(stderr, "Failed to allocate CPU_Usage\n");
        exit(EXIT_FAILURE);
    }

    queue->lastActivity = (time_t *)calloc(3, sizeof(time_t)); // allocate memory for watchdog
    if (queue->lastActivity == NULL)
    {
        fprintf(stderr, "Failed to allocate lastActivitye\n");
        exit(EXIT_FAILURE);
    }

    queue->mutex = (pthread_mutex_t *)calloc(6, sizeof(pthread_mutex_t)); // allocate memory for mutexes
    if (queue->mutex == NULL)
    {
        fprintf(stderr, "Failed to allocate mutex\n");
        exit(EXIT_FAILURE);
    }

    queue->semaphore = (sem_t *)calloc(4, sizeof(sem_t)); // allocate memory for semaphores
    if (queue->semaphore == NULL)
    {
        fprintf(stderr, "Failed to allocate semaphore\n");
        exit(EXIT_FAILURE);
    }

    return (void *)queue; // return pointer
}

void freeQueue(void *arg)
{
    Queue *queue = (Queue *)arg;

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
