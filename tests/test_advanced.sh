#!/bin/bash

cd "$(dirname "$0")/.."

echo "========================================"
echo "Building all programs..."
echo "========================================"
make clean
make

echo ""
echo "========================================"
echo "TEST 1: Basic jobs (sleep-based)"
echo "========================================"
echo "--- Concurrent ---"
./mini_sched ./jobs/job1.out ./jobs/job2.out ./jobs/job3.out

echo ""
echo "--- FIFO ---"
./mini_sched FIFO ./jobs/job1.out ./jobs/job2.out ./jobs/job3.out

echo ""
echo "--- Round Robin ---"
./mini_sched RR ./jobs/job1.out ./jobs/job2.out ./jobs/job3.out

echo ""
echo "========================================"
echo "TEST 2: CPU-bound jobs"
echo "========================================"
echo "--- Concurrent (3 CPU jobs) ---"
./mini_sched "./jobs/cpu_job.out 1000000 1" "./jobs/cpu_job.out 1000000 2" "./jobs/cpu_job.out 1000000 3"

echo ""
echo "--- FIFO (3 CPU jobs) ---"
./mini_sched FIFO "./jobs/cpu_job.out 1000000 1" "./jobs/cpu_job.out 1000000 2" "./jobs/cpu_job.out 1000000 3"

echo ""
echo "--- Round Robin (3 CPU jobs) ---"
./mini_sched RR "./jobs/cpu_job.out 1000000 1" "./jobs/cpu_job.out 1000000 2" "./jobs/cpu_job.out 1000000 3"

echo ""
echo "========================================"
echo "TEST 3: I/O-bound jobs"
echo "========================================"
echo "--- Concurrent (3 I/O jobs) ---"
./mini_sched "./jobs/io_job.out 100 1" "./jobs/io_job.out 100 2" "./jobs/io_job.out 100 3"

echo ""
echo "--- FIFO (3 I/O jobs) ---"
./mini_sched FIFO "./jobs/io_job.out 100 1" "./jobs/io_job.out 100 2" "./jobs/io_job.out 100 3"

echo ""
echo "--- Round Robin (3 I/O jobs) ---"
./mini_sched RR "./jobs/io_job.out 100 1" "./jobs/io_job.out 100 2" "./jobs/io_job.out 100 3"

echo ""
echo "========================================"
echo "TEST 4: Mixed workload (CPU + I/O)"
echo "========================================"
echo "--- Concurrent (2 CPU, 2 I/O, 2 Mixed) ---"
./mini_sched "./jobs/cpu_job.out 500000 1" "./jobs/io_job.out 50 1" "./jobs/mixed_job.out 3 1" \
                  "./jobs/cpu_job.out 500000 2" "./jobs/io_job.out 50 2" "./jobs/mixed_job.out 3 2"

echo ""
echo "--- FIFO (2 CPU, 2 I/O, 2 Mixed) ---"
./mini_sched FIFO "./jobs/cpu_job.out 500000 1" "./jobs/io_job.out 50 1" "./jobs/mixed_job.out 3 1" \
                          "./jobs/cpu_job.out 500000 2" "./jobs/io_job.out 50 2" "./jobs/mixed_job.out 3 2"

echo ""
echo "--- Round Robin (2 CPU, 2 I/O, 2 Mixed) ---"
./mini_sched RR "./jobs/cpu_job.out 500000 1" "./jobs/io_job.out 50 1" "./jobs/mixed_job.out 3 1" \
                                  "./jobs/cpu_job.out 500000 2" "./jobs/io_job.out 50 2" "./jobs/mixed_job.out 3 2"

echo ""
echo "========================================"
echo "TEST 5: Command strings"
echo "========================================"
echo "--- Concurrent ---"
./mini_sched "ls -ltr" "sleep 2" "echo hello"

echo ""
echo "--- FIFO ---"
./mini_sched FIFO "ls -ltr" "sleep 2" "echo hello"

echo ""
echo "--- Round Robin ---"
./mini_sched RR "ls -ltr" "sleep 2" "echo hello"

echo ""
echo "========================================"
echo "All tests completed!"
echo "========================================"

