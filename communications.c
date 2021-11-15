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

//TODO initialization function to init the queues and the
// synchronization structures between the scheduler and one particular cpu
void init(int cpu)
{
}

/* toCPU: we send:
 - the cpu number (for verification by the CPU receiving function)
 - the jID job ID or the EOSIM tag
 - the job duration (expected = real in this simulation). N.A on EOSIM
*/
void toCPU(int cpu, int jID, int jDuration)
{
    /* message msg;
  msg[0]= cpu; msg[1]= jID; msg[2]= jDuration; */

    //TODO synchronization actions

    //TODO put the message in the queue to a particular cpu

    //TODO synchronization actions
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
    // message msg;

    //TODO synchronization actions

    //TODO get the message from the queue from a particular cpu

    //TODO synchronization actions

    /* *cpuID= msg[0]; *jID= msg[1]; *jDuration= msg[2];
  assert(cpu == *cpuID); */
}
