# Mini Scheduler

A simple process scheduler implementing three scheduling algorithms: Concurrent (default), FIFO, and Round-Robin.

## Features

- **Concurrent**: All jobs start simultaneously (default OS scheduling)
- **FIFO**: Jobs execute sequentially in submission order
- **Round-Robin**: Time-sliced execution with 100ms quantum

Uses only POSIX system calls (fork, execvp, waitpid, signals) - no threads or external libraries.

## Project Structure

```
OS_HW/
├── mini_sched.c          # Main scheduler implementation
├── config.h              # Configuration and data structures  
├── Makefile              # Build configuration
├── README.md             # This file
├── jobs/                 # Job programs and test workloads
│   ├── job1.c, job2.c, job3.c    # Basic test jobs
│   ├── cpu_job.c                  # CPU-intensive workload
│   ├── io_job.c                   # I/O-intensive workload
│   └── mixed_job.c                # Mixed CPU/IO workload
└── tests/                # Test scripts
    ├── test_hearbeat.sh          # Quick functionality tests
    └── test_advanced.sh          # Comprehensive tests
```

## Quick Start

### Compile
```bash
make
```

### Run

**Default (Concurrent)**:
```bash
./mini_sched ./jobs/job1.out ./jobs/job2.out ./jobs/job3.out
```

**FIFO**:
```bash
./mini_sched FIFO ./jobs/job1.out ./jobs/job2.out ./jobs/job3.out
```

**Round-Robin**:
```bash
./mini_sched RR ./jobs/job1.out ./jobs/job2.out ./jobs/job3.out
```

### Command Strings
Jobs can be executables or shell commands:
```bash
./mini_sched.out "ls -l" "sleep 2" "echo hello"
./mini_sched.out FIFO ./job1.out ./job2.out
```

## Test Programs

### Basic Jobs (sleep-based)
```bash
./mini_sched ./jobs/job1.out ./jobs/job2.out ./jobs/job3.out
```

### CPU-Bound Jobs
```bash
./mini_sched RR "./jobs/cpu_job.out 1000000 1" "./jobs/cpu_job.out 1000000 2"
```

### I/O-Bound Jobs
```bash
./mini_sched FIFO "./jobs/io_job.out 100 1" "./jobs/io_job.out 100 2"
```

### Mixed Workload
```bash
./mini_sched "./jobs/cpu_job.out 500000 1" "./jobs/io_job.out 50 1" "./jobs/mixed_job.out 3 1"
```

## Run Test Suites

### Basic Tests
```bash
./tests/test_hearbeat.sh
```

### Advanced Tests (CPU/IO-bound workloads)
```bash
./tests/test_advanced.sh
```

## Output

The scheduler provides:
- **Response time**: Time from submission to first execution
- **Turnaround time**: Time from submission to completion
- **Average metrics**: Mean response and turnaround times
- **Debug info**: Detailed timing information (when debug=1)

### Example Output
```
****************Summary: processes executed****************
Job                 	PID     	Status  	Response(s)    	Turnaround(s)  
-----------------------------------------------------------------------------------
./job1.out              12345   	0       	0.000184       	3.002012       
./job2.out              12346   	0       	0.000172       	5.002467       
-----------------------------------------------------------------------------------
Average             	        	        	0.000178       	4.002240       
-----------------------------------------------------------------------------------
```


