/*
*   Created 19/06/23 by Mateusz Glab
*   The logger thread writes information from each thread, as well the program
*   termination status to a file.
*   To synchronize with other threads, the logger use mutexes.
*/

#include "functions.h"

static void cleanUpThread(void *arg)
{
    Queue *queue = (Queue *)arg;
    if (queue->signal)
        fprintf(queue->log, "\nProgram finished by signal.\n");
    if (queue->noRespond)
        fprintf(queue->log, "\nProgram finished by watchdog, thread %d doesn't respond\n", queue->whichOne);
    fclose(queue->log);
}

static void writeReaderData(void *arg)
{
    Queue *queue = (Queue *)arg;

    for (unsigned i = 0; i < queue->cores; i++)
    {
        if (i == 0)
            fprintf(queue->log, "cpu user=%llu nice=%llu system=%llu idle=%llu iowait=%llu irq=%llu softirq=%llu steal=%llu\n",
                    queue->data[queue->insert].user[i], queue->data[queue->insert].nice[i], queue->data[queue->insert].system[i],
                    queue->data[queue->insert].idle[i], queue->data[queue->insert].iowait[i], queue->data[queue->insert].irq[i], queue->data[queue->insert].softirq[i],
                    queue->data[queue->insert].steal[i]);

        fprintf(queue->log, "cpu%u user=%llu nice=%llu system=%llu idle=%llu iowait=%llu irq=%llu softirq=%llu steal=%llu\n", i - 1,
                queue->data[queue->insert].user[i], queue->data[queue->insert].nice[i], queue->data[queue->insert].system[i],
                queue->data[queue->insert].idle[i], queue->data[queue->insert].iowait[i], queue->data[queue->insert].irq[i], queue->data[queue->insert].softirq[i],
                queue->data[queue->insert].steal[i]);
    }
}

static void writeAnalyzerData(void *arg)
{
    Queue *queue = (Queue *)arg;

    for (unsigned i = 0; i < queue->cores; i++)
    {
        if (i == 0)
            fprintf(queue->log, "cpu idle=%llu nonIdle=%llu\n", queue->count[queue->extract].idle[i], queue->count[queue->extract].nonIdle[i]);

        fprintf(queue->log, "cpu%u idle=%llu nonIdle%llu\n", i - 1, queue->count[queue->extract].idle[i], queue->count[queue->extract].nonIdle[i]);
    }
}

static void writePrinterData(void *arg)
{
    Queue *queue = (Queue *)arg;

    for (unsigned i = 0; i < queue->cores; i++)
    {
        if (i == 0)
            fprintf(queue->log, "cpu usage=%.3f%%\n", queue->CPU_Usage[i]);

        fprintf(queue->log, "cpu%u usage%.3f%%\n", i - 1, queue->CPU_Usage[i]);
    }
}

void *logger(void *arg)
{
    Queue *queue = (Queue *)arg;
    assert(queue == arg);

    pthread_cleanup_push(cleanUpThread, queue)

        queue->log = fopen("log.txt", "w");
    if (queue->log == NULL)
    {
        fprintf(stderr, "Failed to open log.txt\n");
        pthread_exit(NULL);
    }

    while (1)
    {
        if (pthread_mutex_lock(&queue->mutex[3]) != 0)
        {
            fprintf(stderr, "Mutex 3 lock error\n");
            pthread_exit(NULL);
        }

        writeReaderData((void *)queue);

        if (pthread_mutex_unlock(&queue->mutex[3]) != 0)
        {
            fprintf(stderr, "Mutex 3 unlock error\n");
            pthread_exit(NULL);
        }

        if (pthread_mutex_lock(&queue->mutex[4]) != 0)
        {
            fprintf(stderr, "Mutex 4 lock error\n");
            pthread_exit(NULL);
        }

        writeAnalyzerData((void *)queue);

        if (pthread_mutex_unlock(&queue->mutex[4]) != 0)
        {
            fprintf(stderr, "Mutex 4 unlock error\n");
            pthread_exit(NULL);
        }

        if (pthread_mutex_lock(&queue->mutex[5]) != 0)
        {
            fprintf(stderr, "Mutex 5 lock error\n");
            pthread_exit(NULL);
        }

        writePrinterData((void *)queue);

        if (pthread_mutex_unlock(&queue->mutex[5]) != 0)
        {
            fprintf(stderr, "Mutex 5 unlock error\n");
            pthread_exit(NULL);
        }
    }
    pthread_cleanup_pop(1);

    return NULL;
}
