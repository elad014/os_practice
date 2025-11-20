#ifndef CONFIG_H
#define CONFIG_H

#include <time.h>
#include <stdbool.h>

// Job timing configuration
const int global_job_time = 3;
int job1_time = global_job_time;
int job2_time = 5;
int job3_time = global_job_time;

// Job status configuration
const int global_status_pass = 0;
int job1_status_pass = global_status_pass;
int job2_status_pass = global_status_pass;
int job3_status_pass = global_status_pass;

const int global_status_fail = 1;
int job1_status_fail = global_status_fail;
int job2_status_fail = global_status_fail;
int job3_status_fail = global_status_fail;

// Scheduler data structures and types
struct job_time {
    struct timespec start;
    struct timespec end;
    double response_seconds;
    double turnaround_seconds;
};

typedef enum {
    SCHED_ORIGINAL = 0,
    SCHED_FIFO = 1,
    SCHED_ROUND_ROBIN = 2
} scheduler_type;

// Scheduler method names (command-line arguments)
#define SCHED_METHOD_FIFO "FIFO"
#define SCHED_METHOD_ROUND_ROBIN "RR"

typedef enum {
    INFO = 0,
    WARNING = 1,
    ERROR = 2
} log_level;

// Global variables (extern declarations)
extern int debug;
extern char debug_msg[128];

#endif