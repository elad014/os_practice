CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g

# Executables
TARGETS = job1 job2 job3 mini_sched test

all: $(TARGETS)

job1: job1.c config.h
	$(CC) $(CFLAGS) -o job1 job1.c

job2: job2.c config.h
	$(CC) $(CFLAGS) -o job2 job2.c

job3: job3.c config.h
	$(CC) $(CFLAGS) -o job3 job3.c

mini_sched: mini_sched.c config.h
	$(CC) $(CFLAGS) -o mini_sched mini_sched.c

test: test.c
	$(CC) $(CFLAGS) -o test test.c

clean:
	rm -f $(TARGETS)
