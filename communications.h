
#define COMMUNICATIONS_H

#ifndef GLOBALS_H
#include "globals.h"
#endif

#include <semaphore.h>

#define N 1024

typedef int message[3];

typedef struct
{
    message buffer[N];
    int slotsCheios, slotsVazios;
    int c, p; // c -- consumer index; p -- producer index
} queue;

//TODO define the sincronization structures
struct CPUqueuePair
{
    queue rdQ;
    sem_t rdS1;
    sem_t rdS2;
    sem_t rdS3;

    queue wrQ;
    sem_t wrS1;
    sem_t wrS2;
    sem_t wrS3;
};

extern struct CPUqueuePair CPUqueues[MAX_CPUs]; //[MAX_CPUs];

void putInQueue(message msg, queue *q);
void getFromQueue(message msg, queue *q);

void init(int cpu);

/* **
  Prototypes of communications functions used by the Scheduler program
** */
void toCPU(int cpu, int jID, int jDuration);

void fromCPU(int cpu, int *cpuID, int *jID, int *jDuration);