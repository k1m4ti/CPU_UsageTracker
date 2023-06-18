#include "functions.h"
#include <assert.h>

void getDataFromReader(void *arg)
{
    Queue *queue = (Queue *)arg; // rzutowanie
    assert(queue == arg);

    for (unsigned i = 0; i < queue->cores; i++)
    {
        // opusc semafor B
        
        queue->count[queue->extract].idle[i] = queue->data[queue->extract].idle[i] + queue->data[queue->extract].iowait[i];

        
        queue->count[queue->extract].nonIdle[i] = queue->data[queue->extract].user[i] + queue->data[queue->extract].nice[i] + queue->data[queue->extract].system[i] +
                                                  queue->data[queue->extract].irq[i] + queue->data[queue->extract].softirq[i] + queue->data[queue->extract].steal[i];

                                               
        // ponies semafor A
        
    }
}

void sendDataToPrinter(void *arg)
{
    Queue *queue = (Queue *)arg; // rzutowanie
    assert(queue == arg);

    for (unsigned i = 0; i < queue->cores; i++)
    {
        // opusc semafor C
        
        
        // oblicz procent
        queue->CPU_Percentage[i] = 1.0 - (double)(queue->count[1].idle[i] - queue->count[0].idle[i]) / (double)(queue->count[1].idle[i] - queue->count[0].idle[i] +
                                queue->count[1].nonIdle[i] - queue->count[0].nonIdle[i]);


        // podnies semafor D
        
    }
}


void *analyzer(void *arg)
{
    Queue *queue = (Queue *)arg;
    assert(queue == arg);

    queue->extract = 0; // set index for first element of buffer
    
    while (1)
    {

        getDataFromReader((void *)queue);

        
        // if counted send to printer
        if (queue->extract == 1)
            sendDataToPrinter((void *)queue);

        CHANGE_INDEX(queue->extract);
    }

    return NULL;
}