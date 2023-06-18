#include "functions.h"
#include <assert.h>

void getDataFromProc(void *arg)
{
    int result;
    Queue *queue = (Queue *)arg;
    assert(queue == arg);

    assert((queue->file = fopen("/proc/stat", "r")) != NULL);
    
    queue->isFileOpen = true;   // file is open set flag for true


    for (unsigned i = 0; i < queue->cores; i++)
    {
        // opuśc semafor A
        
        // get stats
        result = fscanf(queue->file, "%*s %llu %llu %llu %llu %llu %llu %llu %llu %*s %*s",
                            &queue->data[queue->insert].user[i], &queue->data[queue->insert].nice[i], &queue->data[queue->insert].system[i],
                            &queue->data[queue->insert].idle[i], &queue->data[queue->insert].iowait[i], &queue->data[queue->insert].irq[i], &queue->data[queue->insert].softirq[i],
                            &queue->data[queue->insert].steal[i]);

        assert(result != -1);

        // podnieś semafor B
        
    }

    queue->isFileOpen = fclose(queue->file); // close file set flag for false
    assert(queue->isFileOpen == 0);
}

void *reader(void *arg)
{
    Queue *queue = (Queue *)arg; 
    assert(queue == arg);
    
    queue->insert = 0; //set buffer intex 
    
    while (1)
    {     
        getDataFromProc((void *)queue);
       
        sleep(1); // wait for next measure

        CHANGE_INDEX(queue->insert);
        
    }

    return NULL;
}
