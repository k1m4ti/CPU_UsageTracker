/*
*   Created 19/06/23 by Mateusz Glab
*   The printer thread prints the received data from the buffer on the screen. 
*   To synchronize with other threads, the printer thread utilizes semaphores and mutexes.
*/

#include "functions.h"

// uncomment below to simmulate unresponsive
// #define DELAY

void *printer(void *arg)
{
    Queue *queue = (Queue *)arg;

    printf("\n\033[34m========\033[35m CPU Usage \033[34m========\033[0m\n");

    while (1)
    {
        if (pthread_mutex_lock(&queue->mutex[2]) != 0)
        {
            fprintf(stderr, "Mutex 2 lock error\n");
            pthread_exit(NULL);
        }

        queue->lastActivity[2] = time(NULL); // send current time to watchdog

        if (pthread_mutex_unlock(&queue->mutex[2]) != 0)
        {
            fprintf(stderr, "Mutex 2 unlock error\n");
            pthread_exit(NULL);
        }

// simulate unresponsive
#ifdef DELAY
        sleep(4);
#endif

        if (pthread_mutex_lock(&queue->mutex[5]) != 0)
        {
            fprintf(stderr, "Mutex 5 lock error\n");
            pthread_exit(NULL);
        }

        for (unsigned i = 0; i < queue->cores; i++)
        {
            if (sem_wait(&queue->semaphore[3]) != 0)
            {
                fprintf(stderr, "Semaphore 3 wait error\n");
                pthread_exit(NULL);
            }

            queue->CPU_Usage[i] = queue->CPU_Percentage[i]; // get data from analyzer

            if (sem_post(&queue->semaphore[2]) != 0)
            {
                fprintf(stderr, "Semaphore 2 post error\n");
                pthread_exit(NULL);
            }
        }

        if (pthread_mutex_unlock(&queue->mutex[5]) != 0)
        {
            fprintf(stderr, "Mutex 5 unlock error\n");
            pthread_exit(NULL);
        }

        for (unsigned i = 0; i < queue->cores; i++)
        {
            if (i == 0)
                printf("\033[36mCpu\033[0m %.3f%%\n", queue->CPU_Usage[0]);

            printf("\033[36mCpu%d\033[0m %.3f%%\n", i, queue->CPU_Usage[i]);
        }

        printf("\033[34m============================\033[0m\033[%dA\033[28D", queue->cores + 1);
    }

    return NULL;
}
