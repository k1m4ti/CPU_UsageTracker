#include <signal.h>
#include <string.h>
#include <assert.h>

#include "functions.h"

static Queue *queue; // declarate queue

static void cleanUp(void)
{
    freeQueue((void *)queue);
}

static void term(int signum)
{
    queue->signal = true; // set a flag for information threads
    printf("Signal %d handled\n", signum);
    exit(EXIT_SUCCESS);
}

int main(void)
{
    struct sigaction action;
    unsigned cores;

    atexit(cleanUp); // initaion function called at exit

    // handle signals
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);

    cores = (unsigned)sysconf(_SC_NPROCESSORS_ONLN) + 1; // get amount of processor cores for iterate loops
    assert(cores != 0);

    queue = (Queue *)allocateQueue(cores);
    assert(queue != NULL);

    queue->signal = false; // set false value waiting for signal

    queue->cores = cores; // assigning the number of cores to a variable in the queue

    return 0;
}
