#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include"config.h"  

int main() {
    int job_num = 3;
    
    for(int i = 0; i < job3_time; i++) {
        printf("im job %d time left %d\n", job_num, job3_time - i);
        sleep(1);
    }
    return job3_status_pass;
}