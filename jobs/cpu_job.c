#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int iterations = (argc > 1) ? atoi(argv[1]) : 1000000;
    int job_id = (argc > 2) ? atoi(argv[2]) : 1;
    
    printf("CPU-bound job %d starting with %d iterations\n", job_id, iterations);
    
    // CPU-intensive computation
    long long sum = 0;
    for (int i = 0; i < iterations; i++) {
        sum += i * i;
        if (i % (iterations / 5) == 0 && i > 0) {
            printf("CPU job %d: %d%% complete (sum=%lld)\n", 
                   job_id, (i * 100) / iterations, sum);
        }
    }
    
    printf("CPU job %d finished! Final sum: %lld\n", job_id, sum);
    return 0;
}

