/*
*   Created 19/06/23 by Mateusz Glab
*   The watchdog thread compares the current time with the last activity time from the
*   threads. If the difference between these times is greater than 2, it terminates the 
*   program with error.
*   To synchronize with other threads, the watchdog use mutexes.
*/

#include "functions.h"

void *watchdog(void *arg)
{
    Queue *queue = (Queue *)arg;
    queue->noRespond = false;

    while (1)
    {
        sleep(1);

        for (int i = 0; i < 3; i++)
        {
            if (pthread_mutex_lock(&queue->mutex[i]) != 0)
            {
                fprintf(stderr, "Mutex %d unlock error\n", i);
                pthread_exit(NULL);
            }

            if (time(NULL) - queue->lastActivity[i] > 2)
            {
                switch (i)
                {
                case 0:
                    fprintf(stderr, "\033[31mReader thread doesn't respond\033[0m\n");
                    break;

                case 1:
                    fprintf(stderr, "\033[31mAnalyzer thread doesn't respond\033[0m\n");
                    break;

                case 2:
                    fprintf(stderr, "\033[31mPrinter thread doesn't respond\033[0m\n");
                    break;
                }

                queue->whichOne = i;
                queue->noRespond = true;
                pthread_exit(NULL);
            }

            if (pthread_mutex_unlock(&queue->mutex[i]) != 0)
            {
                fprintf(stderr, "Mutex %d unlock error\n", i);
                pthread_exit(NULL);
            }
        }
    }

    return NULL;
}
