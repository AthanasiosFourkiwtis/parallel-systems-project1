# Assignment #1 Analysis — MYE023

## What the assignment asks

The assignment asks to parallelize **two given serial programs** with
OpenMP. The main rule everywhere: **the algorithm doesn't change**, you only
split the work properly across the threads. It asks for timings with 1, 2, 3
and 4 threads, comparison with the plain serial version, and a full report
with charts. Every measurement is the average of at least 4 runs on a
4-core machine.

### Part 1 — Finding primes (40%)
We're given `serial_primes()`, which counts how many primes exist up to N and
which is the biggest one. We have to fill in `openmp_primes()` with the same
algorithm running in parallel, and try different ways of splitting the work
(and justify the best choice).

### Part 2 — Merge sort (40%)
We're given a recursive merge sort. We have to fill in `mergesort_parallel()`
and its call from `main()`, so the sorting happens in parallel with
**OpenMP tasks**. It asks for some experimenting to get the biggest possible
speedup — e.g. using the fact that for small arrays the serial version is faster.

### Part 3 — Taskloop / clauses (20%)
Study the `final` and `mergeable` clauses of OpenMP 5.0, describe them in
writing (1–2 pages) and apply them to the merge sort to see if there's any
improvement.

## What I did

### Part 1 — `primes.c`
In `openmp_primes()` the loop over the odd numbers gets split with
`#pragma omp parallel for`. Since every iteration makes its own independent
number `num = 2*i + 3`, there are no dependencies between iterations.

- The helper variables (`num, divisor, quotient, remainder`) are declared
  `private`, so each thread gets its own copy.
- The prime count is gathered with `reduction(+:...)` and the biggest prime
  with `reduction(max:...)`, which avoids race conditions on the global
  variables.
- I used local variables (`local_count`, `local_lastprime`) instead of the
  globals inside the parallel loop, and copy them to the globals at the end.
- I picked `schedule(runtime)` so the scheduling policy can be set from the
  `OMP_SCHEDULE` environment variable (static / dynamic / guided) without
  recompiling. The work per iteration is **uneven** (big numbers need more
  divisions), so a dynamic schedule balances the load better — the final
  choice is justified in the report based on the measurements.

### Part 2 — `mergesort.c`
I implemented `mergesort_parallel()` with OpenMP tasks:

- The two halves of the array each get sorted in their own
  `#pragma omp task`, followed by `#pragma omp taskwait` before the final
  `merge()`, so both are done first.
- The call from `main()` is inside `#pragma omp parallel` with
  `#pragma omp single`, so one thread creates the tasks and the team
  executes them.
- **Cutoff (`TASK_CUTOFF = 8192`):** below this size I call the serial
  `mergesort_serial()` directly. That skips the task-creation overhead on
  small chunks, where serial is faster — which is exactly the optimization
  the assignment hints at.

### Part 3 — `final` & `mergeable` clauses
Applied to the merge sort with conditional compilation (`#ifdef
USE_FINAL_MERGEABLE`):

- With `USE_FINAL_MERGEABLE` defined, the tasks get
  `final(n <= FINAL_CUTOFF) mergeable`.
- `final` stops the creation of new (nested) tasks below the
  `FINAL_CUTOFF = 32768` limit: the task becomes "final" and the rest of the
  recursion runs serially inside it.
- `mergeable` lets the runtime, when the task is final/included, skip making
  a new data environment and reuse the parent's, which cuts the overhead.
- The comparison of the two versions (with/without the clauses) and whether
  there's any real improvement is in the report.

## Deliverables

- `primes.c`, `mergesort.c` — source code
- `MYE023_Ergasia1.pdf` — full report with timings, charts and discussion
  of the results
