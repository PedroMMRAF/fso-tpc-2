/* ------------------------------------------------------------- */
/* Messages between Scheduler and CPU are always 3 integers long */
/* ------------------------------------------------------------- */

#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <semaphore.h>
#include <stdio.h>

#ifndef COMMUNICATIONS_H
#include "communications.h"
#endif

void putInQueue(message msg, queue *q)
{
    memcpy(q->buffer[q->p], msg, sizeof(message));
    q->p = (q->p + 1) % N;
    q->slotsCheios++;
    q->slotsVazios--;
}

void getFromQueue(message msg, queue *q)
{
    memcpy(msg, q->buffer[q->c], sizeof(message));
    q->c = (q->c + 1) % N;
    q->slotsVazios++;
    q->slotsCheios--;
}

// initialization function to init the queues and the
// synchronization structures between the scheduler and one particular cpu
void init(int cpu)
{
    struct CPUqueuePair *cpuQ = CPUqueues + cpu;

    memset(cpuQ->rdQ.buffer, 0, sizeof(message) * N);
    cpuQ->rdQ.slotsCheios = 0;
    cpuQ->rdQ.slotsVazios = N;
    cpuQ->rdQ.c = 0;
    cpuQ->rdQ.p = 0;

    memset(cpuQ->wrQ.buffer, 0, sizeof(message) * N);
    cpuQ->wrQ.slotsCheios = 0;
    cpuQ->wrQ.slotsVazios = N;
    cpuQ->wrQ.c = 0;
    cpuQ->wrQ.p = 0;

    sem_init(cpuQ->rdS + 0, 0, 1);
    sem_init(cpuQ->rdS + 1, 0, 0);

    sem_init(cpuQ->wrS + 0, 0, 1);
    sem_init(cpuQ->wrS + 1, 0, 0);
}

/* toCPU: we send:
 - the cpu number (for verification by the CPU receiving function)
 - the jID job ID or the EOSIM tag
 - the job duration (expected = real in this simulation). N.A on EOSIM
*/
void toCPU(int cpu, int jID, int jDuration)
{
    struct CPUqueuePair *cpuQ = CPUqueues + cpu;

    message msg;
    msg[0]= cpu;
    msg[1]= jID;
    msg[2]= jDuration;

    // synchronization actions
    sem_wait(cpuQ->wrS + 0);

    // put the message in the queue to a particular cpu
    putInQueue(msg, &cpuQ->wrQ);

    // synchronization actions
    sem_post(cpuQ->wrS + 1);
}

/* fromCPU: receive an EOSIM reply message (from the CPU process)
 parameter:
 - the cpu number (we are receiving from)
 return:
 - the cpu number (we have received) FAILS if they do not match!
 - the jID value must be EOSIM
 - the duration - ammount of work performed by the CPU process
*/
void fromCPU(int cpu, int *cpuID, int *jID, int *jDuration)
{
    struct CPUqueuePair *cpuQ = CPUqueues + cpu;

    message msg;

    // synchronization actions
    sem_wait(cpuQ->rdS + 1);

    // get the message from the queue from a particular cpu
    getFromQueue(msg, &cpuQ->rdQ);

    // synchronization actions
    sem_post(cpuQ->rdS + 0);

    *cpuID= msg[0];
    *jID= msg[1];
    *jDuration= msg[2];

    assert(cpu == *cpuID);
}
