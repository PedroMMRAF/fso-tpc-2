#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>

#include <pthread.h>

// Global definitions
#ifndef GLOBALS_H
#include "globals.h"
#endif

// Definitions for scheduler/cpu communications
#ifndef COMMUNICATIONS_H
#include "communications.h"
#endif

// Definitions for scheduler data structures
#ifndef SCHEDULER_H
#include "scheduler.h"
#endif

// Definitions for workers/threads
#ifndef CPU_H
#include "CPU.h"
#endif

pthread_t *ids;

struct CPUqueuePair CPUqueues[];

/* -------------------------------------------- */
// Creation of threads/workers
void createCPUs(int nCPUs)
{
    // workers/threads' identifiers
    ids = (pthread_t *)malloc(nCPUs * sizeof(pthread_t));

    for (int cpu = 0; cpu < nCPUs; cpu++)
    {
        // init communication and synchronization structures
        init(cpu);

        // Thread/worker creation
        pthread_create(ids + cpu, NULL, &CPU, (void*)(long)cpu);
    }
}

#define MAX(x, y) (x > y ? x : y)

void report(int nCPUs, struct cpuInfo cpuData[])
{
    int cpuID, jID, jDuration, totConsumed = 0, totalSimulationTime = 0;

    for (int cpu = 0; cpu < nCPUs; cpu++)
    {
        fromCPU(cpu, &cpuID, &jID, &jDuration);

        cpuData[cpu].usage += jDuration;
        totConsumed += jDuration;
        totalSimulationTime = MAX(totalSimulationTime, cpuData[cpu].usage);

        printf("CPU[%d] usage= %d\n", cpu, cpuData[cpu].usage);
    }

    printf("Total Simulation Time=%d\n", totalSimulationTime);
    printf("Average CPU consumed for %d CPUs= %.1f%%\n",
           nCPUs, 100 * (float)totConsumed / totalSimulationTime);
}

int cmpFunc(const void *x, const void *y)
{
    struct jobQ *cx = (struct jobQ *)x;
    struct jobQ *cy = (struct jobQ *)y;

    if (cx->duration > cy->duration)
        return 1;
    if (cx->duration < cy->duration)
        return -1;
    if (cx->jID > cy->jID)
        return 1; // = duração decisão jID
    if (cx->jID < cy->jID)
        return -1; // idem

    return 0; // NUNCA deve acontecer!
}

int main(int argc, char *argv[])
{
    char policy = argv[1][0]; // F or S
    int nCPUs = atoi(argv[2]);
    char *filename = argv[3];

    int l, cpu;

    struct cpuInfo cpuData[nCPUs];
    memset(cpuData, 0, sizeof(struct cpuInfo) * nCPUs);

    struct jobQ jobQueue[MAX_JQ_SIZE];
    memset(jobQueue, 0, sizeof(struct jobQ) * MAX_JQ_SIZE);

    createCPUs(nCPUs);

    int lines = readJobSubmission(filename, jobQueue);

    if (policy == 'S')
    {
        // printf("I am calling qsort\n");
        qsort(jobQueue, lines, sizeof(struct jobQ), cmpFunc);
    }

    // Schedule the jobs (FIFO version, assumes job runtime == duration)
    l = 0;
    cpu = 0;
    while (l < lines)
    {
        printf("jQ[%d] jID %d run %d\n", l, jobQueue[l].jID,
               jobQueue[l].duration);
        dispatch(l, jobQueue, cpu, cpuData);
        l++;
        cpu = (cpu + 1) % nCPUs;
    }

    // send SIMulation END messages
    for (int cpu = 0; cpu < nCPUs; cpu++)
        toCPU(cpu, EOSIM, EOSIM);

    // Wait for all threads/workers to conclude
    for (int i = 0; i < nCPUs; i++)
    {
        if (pthread_join(ids[i], NULL) != 0)
        {
            perror("Error in join: ");
            fflush(stderr);
        }
    }

    report(nCPUs, cpuData);

    return 0;
}
