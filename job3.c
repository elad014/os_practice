#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include"config.h"  

int main(int argc, char *argv[]) {
    int job_num = 3;
    
    // Measure startup time if timestamps were passed
    if(argc >= 3) {
        struct timespec start_time, current_time;
        start_time.tv_sec = atol(argv[1]);
        start_time.tv_nsec = atol(argv[2]);
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        
        double startup_time = (current_time.tv_sec - start_time.tv_sec) +
                             (current_time.tv_nsec - start_time.tv_nsec) / 1e9;
        printf("Job %d startup time: %f seconds\n", job_num, startup_time);
    }
    
    for(int i = 0; i < job3_time; i++) {
        printf("im job %d time left %d\n", job_num, job3_time - i);
        sleep(1);
    }
    printf("job %d finished\n", job_num);
    return job3_status_pass;
}