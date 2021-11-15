
CFLAGS = -Wall -g #-Werror

PROGS = JobSch

all: $(PROGS)

JobSch: JobSch.o scheduler.o communications.o CPU.o
	gcc -o JobSch  JobSch.o scheduler.o communications.o CPU.o -lpthread

JobSch.o:  JobSch.c communications.h scheduler.h globals.h
	gcc $(CFLAGS) -c JobSch.c

scheduler.o:  scheduler.c communications.h scheduler.h globals.h
	gcc $(CFLAGS) -c scheduler.c

communications.o:  communications.c communications.h globals.h
	gcc $(CFLAGS) -c communications.c

CPU.o: CPU.c globals.h
	gcc $(CFLAGS) -c CPU.c 

clean:
	rm -f *.o $(PROGS)
