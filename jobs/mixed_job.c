#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int cycles = (argc > 1) ? atoi(argv[1]) : 3;
    int job_id = (argc > 2) ? atoi(argv[2]) : 1;
    
    printf("Mixed job %d starting with %d CPU/IO cycles\n", job_id, cycles);
    
    for (int cycle = 0; cycle < cycles; cycle++) {
        // CPU burst
        printf("Mixed job %d: CPU burst %d/%d\n", job_id, cycle + 1, cycles);
        long long sum = 0;
        for (int i = 0; i < 500000; i++) {
            sum += i * i;
        }
        
        // I/O burst
        printf("Mixed job %d: I/O burst %d/%d\n", job_id, cycle + 1, cycles);
        char filename[64];
        snprintf(filename, sizeof(filename), "/tmp/mixed_job_%d.txt", job_id);
        FILE *fp = fopen(filename, "a");
        if (fp) {
            fprintf(fp, "Job %d - Cycle %d - Sum: %lld\n", job_id, cycle, sum);
            fclose(fp);
        }
        
        sleep(1); // Simulate waiting for I/O
    }
    
    // Cleanup
    char filename[64];
    snprintf(filename, sizeof(filename), "/tmp/mixed_job_%d.txt", job_id);
    remove(filename);
    
    printf("Mixed job %d finished!\n", job_id);
    return 0;
}

