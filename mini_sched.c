#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/time.h>
#include"config.h"
#include<signal.h>
#include<time.h>

void calculate_time(struct timespec *start, struct timespec *end, char *job_name);


int main(int argc, char *argv[]) {
    int job_pid;
    int status;
    int jobs_pids[argc-1];
    char sec_str[32];
    char nsec_str[32];
    struct {
        struct timespec start;
        struct timespec end;
    } timers[argc];

    
    clock_gettime(CLOCK_MONOTONIC, &timers[0].start);
    if(argc < 2) {
        printf("Usage: %s <number of jobs>\n", argv[0]);
        return 1;
    }

    for (int i=1; i<argc; i++) {
        job_pid = fork();
        jobs_pids[i-1] = job_pid;
        if(job_pid == 0) {
            // Child process: record time just before exec
            struct timespec exec_time;
            clock_gettime(CLOCK_MONOTONIC, &exec_time);
            
            // Pass the timestamp to the job as arguments
            sprintf(sec_str, "%ld", exec_time.tv_sec);
            sprintf(nsec_str, "%ld", exec_time.tv_nsec);
            
            char *cmd[] = {argv[i], sec_str, nsec_str, NULL};
            execvp(argv[i], cmd);
            perror("execvp failed");
            exit(1);
        }
        // Parent process: record start time for this job
        clock_gettime(CLOCK_MONOTONIC, &timers[i].start);
        printf("job name: %s pid: %d\n", argv[i], jobs_pids[i-1]);
    }
    
    for (int i=0; i<argc-1; i++) {
        waitpid(jobs_pids[i], &status, 0);
        clock_gettime(CLOCK_MONOTONIC, &timers[i+1].end);
        calculate_time(&timers[i+1].start, &timers[i+1].end, argv[i+1]);
        if (WIFEXITED(status)) {
            printf("job %d finished with status %d\n", jobs_pids[i], WEXITSTATUS(status));
        } else {
            printf("job %d did not exit normally\n", jobs_pids[i]);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &timers[0].end);
    calculate_time(&timers[0].start, &timers[0].end, "total");
    return 0;
}

void calculate_time(struct timespec *start, struct timespec *end, char *job_name) {
    double diff = (end->tv_sec - start->tv_sec) +
                  (end->tv_nsec - start->tv_nsec) / 1e9;
    printf("time spent: %f seconds for job %s\n", diff, job_name);
}

