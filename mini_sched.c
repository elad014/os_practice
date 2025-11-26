#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "config.h"
/*
##########################################################
###################### Global Variables ##################
##########################################################
*/

int debug = 0;
char debug_msg[128];

/*
##########################################################
###################### declarations Functions ############
##########################################################
*/

bool is_valid_args(int argc);
void logger(char *message, int level);
void parse_command(char *command, char *args[]);
double calculate_response(struct timespec start, struct timespec submission);
double calculate_turnaround(struct timespec end, struct timespec submission);
void calculate_average(struct job_time jobs_time[], int process_counter, double *avg_response, double *avg_turnaround);
void print_summary(char *argv[], int jobs_pids[], int jobs_statuses[], struct job_time jobs_time[], int process_counter, double avg_response, double avg_turnaround);
int global_mini_sched(int argc, char *argv[], struct job_time jobs_time[], scheduler_type sched_type);
scheduler_type parse_scheduler_method(char *method);

/*
##########################################################
###################### Main Functions ####################
##########################################################
*/

int main(int argc, char *argv[]) {
    int num_jobs;
    int job_start_index = 1;
    scheduler_type sched_type = SCHED_ORIGINAL;
    
    if(!is_valid_args(argc)) {
        logger("No jobs to schedule", ERROR);
        return 1;
    }
    
    // Check for scheduler method: ./mini_sched FIFO job1 job2
    sched_type = parse_scheduler_method(argv[1]);
    if(sched_type != SCHED_ORIGINAL) {
        job_start_index = 2;
        if(argc < 3) {
            logger("Usage: ./mini_sched [FIFO|RR] job1 job2 ...", ERROR);
            return 1;
        }
    }
    
    num_jobs = argc - job_start_index;
    struct job_time jobs_time[num_jobs];
    
    if(debug) {
        if(sched_type == SCHED_FIFO) {
            logger("Scheduler method: FIFO", 3);
        } else if(sched_type == SCHED_ROUND_ROBIN) {
            logger("Scheduler method: Round Robin", 3);
        } else {
            logger("Scheduler method: Concurrent (default)", 3);
        }
        for (int i = 0; i < num_jobs; i++) {
            snprintf(debug_msg, sizeof(debug_msg), "job %d is: %s", i + 1, argv[job_start_index + i]);
            logger(debug_msg, 3);
        }
    }
    
    if(sched_type == SCHED_FIFO) {
        global_mini_sched(argc - job_start_index + 1, argv + job_start_index - 1, jobs_time, SCHED_FIFO);
    } else if(sched_type == SCHED_ROUND_ROBIN) {
        global_mini_sched(argc - job_start_index + 1, argv + job_start_index - 1, jobs_time, SCHED_ROUND_ROBIN);
    } else {
        global_mini_sched(argc, argv, jobs_time, SCHED_ORIGINAL);
    }
    
    return 0;
}

int global_mini_sched(int argc, char *argv[], struct job_time jobs_time[], scheduler_type sched_type) {
    char *args[20];
    int job_pid;
    int status;
    int process_counter = argc - 1;
    int jobs_pids[process_counter];
    int jobs_statuses[process_counter];
    int pipes[process_counter][2];   
    const int READ_PIPE = 0;
    const int WRITE_PIPE = 1;
    struct timespec submission_time[process_counter];
    struct timespec job_start_time;
    double avg_response;
    double avg_turnaround;
    bool jobs_completed[process_counter];
    bool jobs_started[process_counter];
    const int TIME_QUANTUM_MS = 100;  // 100ms time quantum

    // Initialize arrays
    for (int i = 0; i < process_counter; i++) {
        jobs_completed[i] = false;
        jobs_started[i] = false;
        if (sched_type == SCHED_ORIGINAL) {                 //init pipe for each job to comunecate with the jobs
            if (pipe(pipes[i]) == -1) {
                perror("pipe failed");
                exit(1);
            }
        }
    }

    // Part 1: Job Submission and Forking
    for (int i = 1; i <= process_counter; i++) {
        
        clock_gettime(CLOCK_REALTIME, &submission_time[i - 1]);                             //record time befor process get pid in real time
        
        job_pid = fork();                                                                   // fork a new process
        jobs_pids[i - 1] = job_pid;
        
        if(job_pid == 0) {
            //in this case we dont know which process start and when so we need to record it in a pipe 
            // if fifo or round robin we know the start time so we can record it in the 
            // job_time array that each process start right after fork
            if(sched_type == SCHED_ORIGINAL) { 
                fflush(stdout);                                                             // flash stdout buffer
                close(pipes[i - 1][READ_PIPE]);                                             //close read end of pipe
                
                clock_gettime(CLOCK_REALTIME, &job_start_time);                             // record job start time in real time
                
                write(pipes[i - 1][WRITE_PIPE], &job_start_time, sizeof(struct timespec)); // write the start time 
                close(pipes[i - 1][WRITE_PIPE]);
            }
            parse_command(argv[i], args);
            
            execvp(args[0], args); 
            perror("execvp failed");
            exit(1);
        } else {
            // Parent process post-fork handling
            if(sched_type == SCHED_ORIGINAL) {
                close(pipes[i - 1][WRITE_PIPE]);
            }
            else if(sched_type == SCHED_ROUND_ROBIN) {
                // Stop the child immediately after fork
                usleep(50);  // Small delay to ensure child has started
                kill(job_pid, SIGSTOP);
            }
        }
    }

    // Part 2: Scheduler-Specific Completion and Timing Handling
    if(sched_type == SCHED_ORIGINAL) {
        // Read start times from all children first
        for (int i = 0; i < process_counter; i++) {
            read(pipes[i][READ_PIPE], &jobs_time[i].start, sizeof(struct timespec));
            close(pipes[i][READ_PIPE]);
        }
        for (int i = 0; i < process_counter; i++) { // waiting loop for all jobs
            int completed_pid = waitpid(-1, &status, 0); // Wait for ANY child
            
            for (int j = 0; j < process_counter; j++) { //record each job end time by pid
                if (jobs_pids[j] == completed_pid) {
                    clock_gettime(CLOCK_REALTIME, &jobs_time[j].end); // record job end time
                    
                    jobs_statuses[j] = status;
                    jobs_time[j].response_seconds = calculate_response(jobs_time[j].start, submission_time[j]);   // calculate response time
                    jobs_time[j].turnaround_seconds = calculate_turnaround(jobs_time[j].end, submission_time[j]); // calculate turnaround time
               }
            }
        }
    }
    else if(sched_type == SCHED_FIFO) {
        // FIFO: Wait for each job sequentially in order
        for (int i = 0; i < process_counter; i++) {
            // Record start time: first job starts immediately, others start when previous finishes
            if (i == 0) {
                clock_gettime(CLOCK_REALTIME, &jobs_time[i].start);
            } else {
                jobs_time[i].start = jobs_time[i-1].end;  // Start when previous job finished
            }
            
            waitpid(jobs_pids[i], &status, 0);
            
            clock_gettime(CLOCK_REALTIME, &jobs_time[i].end);
            
            jobs_statuses[i] = status;
            jobs_time[i].response_seconds = calculate_response(jobs_time[i].start, submission_time[i]);
            jobs_time[i].turnaround_seconds = calculate_turnaround(jobs_time[i].end, submission_time[i]);
        }
    }
    else if(sched_type == SCHED_ROUND_ROBIN) {
        // Round robin scheduling loop
        int completed_count = 0;
        int current_job = 0;
        
        while(completed_count < process_counter) {
            if(!jobs_completed[current_job]) {
                // Record start time on first run
                if(!jobs_started[current_job]) {
                    clock_gettime(CLOCK_REALTIME, &jobs_time[current_job].start);
                    jobs_started[current_job] = true;
                }
                
                // Continue the job
                kill(jobs_pids[current_job], SIGCONT);
                
                // Let it run for the time quantum
                usleep(TIME_QUANTUM_MS * 1000);
                
                // Check if job has completed
                int result = waitpid(jobs_pids[current_job], &status, WNOHANG);
                
                if(result > 0) {
                    // Job completed
                    clock_gettime(CLOCK_REALTIME, &jobs_time[current_job].end);
                    jobs_statuses[current_job] = status;
                    jobs_time[current_job].response_seconds = calculate_response(jobs_time[current_job].start, submission_time[current_job]);
                    jobs_time[current_job].turnaround_seconds = calculate_turnaround(jobs_time[current_job].end, submission_time[current_job]);
                    jobs_completed[current_job] = true;
                    completed_count++;
                } else {
                    // Job still running, stop it
                    kill(jobs_pids[current_job], SIGSTOP);
                }
            }
            
            // Move to next job in round-robin fashion
            current_job = (current_job + 1) % process_counter;
        }
    }

    if (debug) {
        logger("****************Debug Information****************\n", 3);
        for (int i = 0; i < process_counter; i++) {
            snprintf(debug_msg, sizeof(debug_msg), "Job %d:", i + 1);
            logger(debug_msg, 3);
            printf("  Submission time: %ld.%09ld\n", submission_time[i].tv_sec, submission_time[i].tv_nsec);
            printf("  Start time: %ld.%09ld\n", jobs_time[i].start.tv_sec, jobs_time[i].start.tv_nsec);
            printf("  End time: %ld.%09ld\n", jobs_time[i].end.tv_sec, jobs_time[i].end.tv_nsec);
            printf("  Response time: %.6f seconds\n", jobs_time[i].response_seconds);
            printf("  Turnaround time: %.6f seconds\n", jobs_time[i].turnaround_seconds);

            printf("\n");
        }
    }
    calculate_average(jobs_time, process_counter, &avg_response, &avg_turnaround);
    print_summary(argv, jobs_pids, jobs_statuses, jobs_time, process_counter, avg_response, avg_turnaround);
    return 0;

}

/*
##########################################################
###################### Helper Functions ##################
##########################################################
*/

bool is_valid_args(int argc) {
    if(argc < 2) {
        return false;
    }
    return true;
}

void logger(char *message, int level) {
    if(level == 0) {
        printf("[INFO]: %s\n", message);
    } else if(level == 1) {
        printf("[WARNING]: %s\n", message);
    } else if(level == 2) {
        printf("[ERROR]: %s\n", message);
    } else {
        printf("[DEBUG]: %s\n", message);
    }
}

void parse_command(char *command, char *args[]) {
    /* 
    This function parses the command and stores the arguments in the args array.
    args[0] is the actualy command and the rest are the arguments.
    */
    int arg_count = 0;
    char *token = strtok(command, " ");
    
    while (token != NULL && arg_count < 19) {
        args[arg_count] = token;
        token = strtok(NULL, " ");
        arg_count++;
    }
    args[arg_count] = NULL;
}

double calculate_response(struct timespec start, struct timespec submission) {
    return (start.tv_sec - submission.tv_sec) + 
           (start.tv_nsec - submission.tv_nsec) / 1e9;
}

double calculate_turnaround(struct timespec end, struct timespec submission) {
    return (end.tv_sec - submission.tv_sec) + 
           (end.tv_nsec - submission.tv_nsec) / 1e9;
}

void calculate_average(struct job_time jobs_time[], int process_counter, double *avg_response, double *avg_turnaround) {
    double total_response = 0.0;
    double total_turnaround = 0.0;
    
    for (int i = 0; i < process_counter; i++) {
        total_response += jobs_time[i].response_seconds;
        total_turnaround += jobs_time[i].turnaround_seconds;
    }
    
    *avg_response = total_response / process_counter;
    *avg_turnaround = total_turnaround / process_counter;
}

void print_summary(char *argv[], int jobs_pids[], int jobs_statuses[], struct job_time jobs_time[], int process_counter, double avg_response, double avg_turnaround) {
    printf("\n****************Summary: processes executed****************\n");
    printf("%-20s\t%-8s\t%-8s\t%-15s\t%-15s\n", 
           "Job", "PID", "Status", "Response(s)", "Turnaround(s)");
    printf("-----------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < process_counter; i++) {
        printf("%-20s\t%-8d\t%-8d\t%-15.6f\t%-15.6f\n", 
               argv[i + 1], 
               jobs_pids[i], 
               jobs_statuses[i],
               jobs_time[i].response_seconds,
               jobs_time[i].turnaround_seconds);
    }
    printf("-----------------------------------------------------------------------------------\n");
    
    printf("%-20s\t%-8s\t%-8s\t%-15.6f\t%-15.6f\n", 
           "Average", "", "", avg_response, avg_turnaround);
    printf("-----------------------------------------------------------------------------------\n");
}

scheduler_type parse_scheduler_method(char *method) {
    if(method != NULL) {
        if(strcmp(method, SCHED_METHOD_FIFO) == 0) {
            return SCHED_FIFO;
        } 
        else if(strcmp(method, SCHED_METHOD_ROUND_ROBIN) == 0) {
            return SCHED_ROUND_ROBIN;
        }
    }
    return SCHED_ORIGINAL;
}

