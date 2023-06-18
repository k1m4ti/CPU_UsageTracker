#include "functions.h"
#include <assert.h>

void *watchdog(void *arg)
{
    Queue *queue = (Queue *)arg; 
    assert(queue == arg);
    queue->noRespond = false;  

    while(1)
    {
        sleep(1);

        for (int i = 0; i < 3; i++)
        {
            assert(pthread_mutex_lock(&queue->mutex[i]) == 0);
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
            assert(pthread_mutex_unlock(&queue->mutex[i]) == 0);
        }
    }

    return NULL;
}
