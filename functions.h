#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>

#define CHANGE_INDEX(index) (index) = ((index + 1) % 2)

//struct for storeage data from file 
typedef struct 
{
    unsigned long long *user, *nice, *system, *idle, *iowait, *irq, *softirq, *steal;
} Data;

//struct for storeage results
typedef struct
{
    unsigned long long *idle, *nonIdle;
} Count;

#pragma pack(push, 1)
typedef struct 
{
    Data *data;
    Count *count;
    FILE *file, *log;
    sem_t *semaphore;
    pthread_mutex_t *mutex;
    time_t *lastActivity;
    int insert, extract, whichOne;
    unsigned cores;
    bool isFileOpen, signal, noRespond;
    double *CPU_Percentage, *CPU_Usage;
}Queue;
#pragma pack(pop)

void *allocateQueue(unsigned cores);

void freeQueue(void *arg);

void *reader(void *arg);

void *analyzer(void *arg);

void *printer(void *arg);

void *watchdog(void *arg);

void *logger(void *arg);

#endif
