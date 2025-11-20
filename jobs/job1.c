#include<stdio.h>
#include<unistd.h>
#include<time.h>
#include<stdlib.h>
#include"config.h"

int main(int argc, char *argv[]) {    
    int job_num = 1;
    for(int i = 0; i < job1_time; i++) {
        printf("im job %d time left %d\n", job_num, job1_time - i);
        sleep(1);
    }
    return job1_status_pass;
}