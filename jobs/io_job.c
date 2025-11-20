#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int iterations = (argc > 1) ? atoi(argv[1]) : 100;
    int job_id = (argc > 2) ? atoi(argv[2]) : 1;
    char filename[64];
    
    snprintf(filename, sizeof(filename), "/tmp/io_job_%d.txt", job_id);
    
    printf("I/O-bound job %d starting with %d write/read cycles\n", job_id, iterations);
    
    FILE *fp;
    for (int i = 0; i < iterations; i++) {
        // Write operation
        fp = fopen(filename, "a");
        if (fp) {
            fprintf(fp, "Job %d - Iteration %d: Writing data to file\n", job_id, i);
            fclose(fp);
        }
        
        // Read operation
        fp = fopen(filename, "r");
        if (fp) {
            char buffer[256];
            while (fgets(buffer, sizeof(buffer), fp) != NULL) {
                // Just reading, not printing to avoid clutter
            }
            fclose(fp);
        }
        
        if (i % (iterations / 5) == 0 && i > 0) {
            printf("I/O job %d: %d%% complete\n", job_id, (i * 100) / iterations);
        }
        
        usleep(10000); // Small delay to simulate realistic I/O timing
    }
    
    // Cleanup
    remove(filename);
    printf("I/O job %d finished!\n", job_id);
    return 0;
}

