/* ------------------------------------------------------------- */
/* Messages between Scheduler and CPU are always 3 integers long */
/* ------------------------------------------------------------- */

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <semaphore.h>

#ifndef GLOBALS_H
#include "globals.h"
#endif

#ifndef COMMUNICATIONS_H
#include "communications.h"
#endif

//int myCPUid;	// myCPUid não pode ser global

// pthread_mutex_t ex = PTHREAD_MUTEX_INITIALIZER;

/* fromSCH: receive a message (from the Scheduler process)
input parameter: the cpu identifier
 return:
 - the cpu number (for verification by the CPU receiving function)
 - the job ID
 - the job duration (expected = real in this simulation)
*/
void fromSCH(int myCPUid, int *cpuID, int *jID, int *jDuration)
{
    struct CPUqueuePair *cpuQ = CPUqueues + myCPUid;

    message msg;

    sem_wait(cpuQ->wrS + 1);
    getFromQueue(msg, &cpuQ->wrQ);
    sem_post(cpuQ->wrS + 0);

    *cpuID = msg[0];
    *jID = msg[1];
    *jDuration = msg[2];

    assert(myCPUid == *cpuID);
}

/* toSCH: send a message (to the Scheduler process)
 parameters:
 - the cpu number (i.e., myCPUid or another variable with the same value)
 - the EOSIM tag
 - the ammount of work this CPU has done, i.e., SUM of all durations
*/
void toSCH(int cpu, int jID, int jDuration)
{
    struct CPUqueuePair *cpuQ = CPUqueues + cpu;

    message msg;
    msg[0] = cpu;
    msg[1] = jID;
    msg[2] = jDuration;

    sem_wait(cpuQ->rdS + 0);
    putInQueue(msg, &cpuQ->rdQ);
    sem_post(cpuQ->rdS + 1);
}

/* ---------------------------------------------- 
   
 ------------------------------------------------ */

void *CPU(void *id)
{
    int cpuID, jID, jDuration;
    unsigned int CPUconsumed = 0;
    int myCPUid;

    myCPUid = (int)(long)id;

    // printf("Hello, I am thread %d\n", myCPUid);
    // sleep(5);

    for (;;)
    {
        fromSCH(myCPUid, &cpuID, &jID, &jDuration);
        // printf("cpuID:%d jID:%d duration:%d\n", cpuID, jID, jDuration);

        if (jID == EOSIM)
            break;

        usleep(jDuration * 1000);
        //sleep(jDuration);
        CPUconsumed += jDuration;
    }
    //printf("Thread: myCPUid:%d cpuID:%d antes de toSCH final\n",myCPUid,cpuID);
    toSCH(cpuID, EOSIM, CPUconsumed);
    //printf("Thread: cpuID:%d vou terminar\n",myCPUid);

    return NULL;
}
