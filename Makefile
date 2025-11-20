CC = gcc
CFLAGS = -Wall -g -I.

# Directories
JOBS_DIR = jobs
TESTS_DIR = tests

# Executables
TARGETS = mini_sched job1 job2 job3 cpu_job io_job mixed_job

all: $(TARGETS)

mini_sched: mini_sched.c config.h
	$(CC) $(CFLAGS) -o mini_sched mini_sched.c

job1: $(JOBS_DIR)/job1.c config.h
	$(CC) $(CFLAGS) -o $(JOBS_DIR)/job1.out $(JOBS_DIR)/job1.c

job2: $(JOBS_DIR)/job2.c config.h
	$(CC) $(CFLAGS) -o $(JOBS_DIR)/job2.out $(JOBS_DIR)/job2.c

job3: $(JOBS_DIR)/job3.c config.h
	$(CC) $(CFLAGS) -o $(JOBS_DIR)/job3.out $(JOBS_DIR)/job3.c

cpu_job: $(JOBS_DIR)/cpu_job.c
	$(CC) $(CFLAGS) -o $(JOBS_DIR)/cpu_job.out $(JOBS_DIR)/cpu_job.c

io_job: $(JOBS_DIR)/io_job.c
	$(CC) $(CFLAGS) -o $(JOBS_DIR)/io_job.out $(JOBS_DIR)/io_job.c

mixed_job: $(JOBS_DIR)/mixed_job.c
	$(CC) $(CFLAGS) -o $(JOBS_DIR)/mixed_job.out $(JOBS_DIR)/mixed_job.c

clean:
	rm -f mini_sched $(JOBS_DIR)/*.out /tmp/io_job_*.txt /tmp/mixed_job_*.txt
