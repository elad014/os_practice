#! /bin/bash

cd "$(dirname "$0")/.."
make
echo "========================================"
echo "TEST 1: Basic jobs (sleep-based)"
echo "========================================"
echo "--- Concurrent custom jobs ---"
./mini_sched ./jobs/job1.out ./jobs/job2.out ./jobs/job3.out
echo "--- FIFO custom jobs ---"
./mini_sched FIFO ./jobs/job1.out ./jobs/job2.out ./jobs/job3.out
echo "--- Round Robin custom jobs ---"
./mini_sched RR ./jobs/job1.out ./jobs/job2.out ./jobs/job3.out

echo "========================================"
echo "TEST 2: Built-in jobs (sleep-based)"
echo "========================================"
echo "--- Concurrent built-in jobs ---"
./mini_sched "ls -ltr" "sleep 3" "echo hello"
echo "--- FIFO built-in jobs ---"
./mini_sched FIFO "ls -ltr" "sleep 3" "echo hello"
echo "--- Round Robin built-in jobs ---"
./mini_sched RR "ls -ltr" "sleep 3" "echo hello"