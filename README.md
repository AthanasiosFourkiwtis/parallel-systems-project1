# Parallel Systems, Assignment 1 (OpenMP)

Parallelizing two serial programs with OpenMP, timing them and comparing with the serial versions. Course of the CSE department, University of Ioannina, academic year 2025-26, instructor Vassilios Dimakopoulos. Athanasios Fourkiotis, student ID 4940.

## What the assignment asks

We're given two working serial programs, one that counts the primes up to N and a recursive merge sort, and the job is to parallelize them with OpenMP without changing the algorithm. The prime loop goes with a parallel for, trying different schedules and justifying the best one, the merge sort goes with tasks, and the last part studies the final and mergeable clauses of OpenMP 5.0 to see whether they help. Everything gets timed with 1 to 4 threads and compared against the serial versions.

## How I solved it

For the primes, each loop iteration checks its own independent odd number, so the loop parallelizes directly. The only real issues are the shared counters, which I handled with reductions (a sum reduction for the count and a max reduction for the largest prime, with the helper variables declared private), and the uneven work per iteration, since bigger numbers need more divisions. That's why the schedule is left as schedule(runtime), so I could try static, dynamic and guided through the OMP_SCHEDULE environment variable without recompiling and pick the best from the measurements.

For the merge sort, the two halves of the array don't depend on each other, so each one becomes its own task and a taskwait guards the final merge. The call from main sits inside a parallel region with a single directive, so one thread creates the tasks and the whole team executes them. The important trick is the cutoff: below 8192 elements creating tasks costs more than it saves, so the code falls back to the serial sort there, which is exactly the optimization the assignment hints at.

For the third part, the tasks optionally get final(n <= 32768) mergeable behind an ifdef. The final clause makes a task below the threshold stop spawning nested tasks and run the rest of the recursion serially inside it, and mergeable lets the runtime reuse the parent's data environment instead of making a new one. Whether this actually helps in practice is discussed in the report. The full step-by-step reasoning is in [ANALYSIS.md](ANALYSIS.md).

## What's in here

`primes.c` has the serial and the parallel prime counting, `mergesort.c` has the serial and the task-based merge sort, `hw1.pdf` is the assignment description and `MYE023_Ergasia1.pdf` is my report with the timings, the charts and the discussion. The exe files are compiled Windows executables.

## Build and run

```sh
gcc -O2 -fopenmp primes.c    -o primes
gcc -O2 -fopenmp mergesort.c -o mergesort
gcc -O2 -fopenmp -DUSE_FINAL_MERGEABLE mergesort.c -o mergesort_c
```

Running them is straightforward. The primes program has N fixed at 10 million and the merge sort takes the array size in millions of elements, so 20 means an array of 20971520 elements:

```sh
./primes
./mergesort 20
```

To test with different thread counts and schedules:

```sh
OMP_NUM_THREADS=4 OMP_SCHEDULE=dynamic ./primes
```

Every measurement in the report is the average of at least 4 runs on a 4-core processor.
