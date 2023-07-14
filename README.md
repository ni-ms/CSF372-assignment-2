# Assignment 2: Matrix Multiplication with Parallelism

## Problem Statement

For this assignment, we were tasked with multiplying arbitrarily large matrices using parallelism provided by the Linux process and thread libraries.

### Part a) Matrix Multiplication Program (P1)

To solve this part, we implemented a C program called `P1.c`, which took three filenames (`in1.txt`, `in2.txt`, and `out.txt`) as command line arguments. The `in1.txt` and `in2.txt` files contained two matrices of arbitrary size, satisfying the criteria for matrix multiplication. The sizes of the matrices were passed through the command line.

- We implemented `P1` to spawn `n` threads, where each thread read rows and columns from `in1.txt` and `in2.txt`.
- We ensured that different threads read different parts of the files.
- We recorded the time it took to read the entire file into memory with different numbers of threads (1, 2, ..., n) with nanosecond granularity.
- We plotted the time against the number of threads for different input sizes and analyzed the findings.

### Part b) Matrix Computation Program (P2)

To solve this part, we implemented a C program called `P2.c`, which used IPC mechanisms to receive the rows and columns read by `P1`. `P2` spawned multiple threads to compute the cells in the product matrix and stored the result in the `out.txt` file.

- We varied the number of threads from 1 to an arbitrarily large number.
- We recorded the time it took to compute the product with different numbers of threads with nanosecond granularity.
- We plotted the time against the number of threads for different input sizes and analyzed the findings.

### Part c) Scheduler Program (S)

For this part, we implemented a scheduler program called `S.c`. `S` spawned two child processes (`P1` and `P2`) using the `exec` system call. We simulated a uniprocessor scheduler and used the following scheduling algorithms:

- Round Robin with a time quantum of 1 ms for `P1` and 2 ms for `P2`.
- Completely Fair Scheduler where the `vruntime` was updated every 1 ms.

`S` suspended `P1` and let `P2` execute, and vice versa.

- We plotted the total turnaround time vs. workload size and waiting time vs. workload size for the different scheduling algorithms.
- We analyzed the findings and compared the two algorithms for the same workload size.
- We calculated the switching overhead in the different cases.

## How to Run

To compile and run the programs, go to the respective folder and run the code
