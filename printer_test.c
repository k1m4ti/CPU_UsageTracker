#include "functions.h"
#include <assert.h>

void *printer(void *arg)
{
    Queue *queue = (Queue *)arg; 
    assert(queue == arg);

    printf("\n\033[34m========\033[35m CPU Usage \033[34m========\033[0m\n");

    while (1)
    {

        for (unsigned i = 0; i < queue->cores; i++)
        {
            assert (sem_wait(&queue->semaphore[3]) == 0);
           
            queue->CPU_Usage[i] = queue->CPU_Percentage[i]; // get data from analyzer
            
            assert (sem_post(&queue->semaphore[2]) == 0);
          
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
