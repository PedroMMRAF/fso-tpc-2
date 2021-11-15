
#define CPU_H

void *CPU(void *id);

void fromSCH(int myCPUid, int *cpuID, int *jID, int *jDuration);

void toSCH(int cpu, int jID, int jDuration);