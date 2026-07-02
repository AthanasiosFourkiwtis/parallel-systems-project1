# Assignment 1 Analysis

## Breaking down the assignment

The assignment asks for the parallelization of two given serial programs with OpenMP. The main rule everywhere is that the algorithm doesn't change, you only split the work properly across the threads. It asks for timings with 1, 2, 3 and 4 threads, comparison with the plain serial version, and a full report with charts. Every measurement is the average of at least 4 runs on a 4-core machine.

Part 1 (40%) gives us serial_primes(), which counts how many primes exist up to N and which one is the biggest. We have to fill in openmp_primes() with the same algorithm running in parallel and try different ways of splitting the work, justifying the best choice.

Part 2 (40%) gives us a recursive merge sort. We have to fill in mergesort_parallel() and its call from main(), so the sorting happens in parallel with OpenMP tasks. Some experimenting is expected to get the biggest possible speedup, for example using the fact that for small arrays the serial version is faster.

Part 3 (20%) asks to study the final and mergeable clauses of OpenMP 5.0, describe them in writing in a page or two, and apply them to the merge sort to see if there's any improvement.

## What I did

In openmp_primes() the loop over the odd numbers gets split with a parallel for. Since every iteration makes its own independent number num = 2i + 3, there are no dependencies between iterations. The helper variables num, divisor, quotient and remainder are declared private so each thread gets its own copy. The prime count is gathered with a sum reduction and the biggest prime with a max reduction, which avoids race conditions on the global variables. Inside the parallel loop I work with local variables, local_count and local_lastprime, and copy them to the globals at the end. The schedule is schedule(runtime), so the policy can be set from the OMP_SCHEDULE environment variable without recompiling. The work per iteration is uneven, since big numbers need more divisions, so a dynamic schedule balances the load better. The final choice is justified in the report based on the measurements.

For the merge sort, the two halves of the array each get sorted in their own task, followed by a taskwait before the final merge() so both are done first. The call from main() is inside a parallel region with a single directive, so one thread creates the tasks and the team executes them. The cutoff TASK_CUTOFF = 8192 means that below this size the serial mergesort_serial() gets called directly. That skips the task creation overhead on small chunks, where serial is faster anyway, and it's exactly the optimization the assignment hints at.

For the clauses, they're applied to the merge sort behind conditional compilation with ifdef USE_FINAL_MERGEABLE. When it's defined, the tasks get final(n <= FINAL_CUTOFF) mergeable with the cutoff at 32768. The final clause stops the creation of new nested tasks below the limit, the task becomes final and the rest of the recursion runs serially inside it. The mergeable clause lets the runtime, when the task is final or included, skip making a new data environment and reuse the parent's, which cuts overhead. The comparison of the two builds and whether there's any real gain is in the report.

## Deliverables

The source code is primes.c and mergesort.c, and MYE023_Ergasia1.pdf is the full report with the timings, the charts and the discussion of the results.
