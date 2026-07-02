# Assignment #1 Analysis — MYE023

## Breaking down the assignment

The assignment asks for the parallelization of **two given serial programs**
using OpenMP. The key constraint throughout: **the algorithm must not change** —
only the work gets distributed properly across threads. It calls for timings
with 1, 2, 3, and 4 threads, a comparison against the plain serial version, and
a full report with charts. Every measurement is the average of ≥4 runs on a
4-core machine.

### Part 1 — Prime counting (40%)
`serial_primes()` is provided; it counts how many primes exist up to N and
which one is the largest. The task is to fill in `openmp_primes()` with the
same algorithm running in parallel, and to experiment with alternative
work-sharing policies (justifying the best choice).

### Part 2 — Merge sort (40%)
A recursive merge sort is provided. The task is to fill in
`mergesort_parallel()` and its invocation from `main()`, so the sort runs in
parallel with **OpenMP tasks**. Experimentation is expected to squeeze out the
maximum speedup — e.g. exploiting the fact that for small arrays the serial
version is faster.

### Part 3 — Taskloop / clauses (20%)
Study the `final` and `mergeable` clauses of OpenMP 5.0, describe them in
writing (1–2 pages), and apply them to the merge sort to see whether they
bring any improvement.

## What was done

### Part 1 — `primes.c`
In `openmp_primes()` the loop over odd numbers is shared out with
`#pragma omp parallel for`. Since every iteration produces an independent
number `num = 2*i + 3`, there are no cross-iteration dependencies.

- The helper variables (`num, divisor, quotient, remainder`) were declared
  `private`, so each thread gets its own copy.
- The prime count is accumulated with `reduction(+:...)` and the largest
  prime with `reduction(max:...)`, avoiding race conditions on the global
  variables.
- Local variables (`local_count`, `local_lastprime`) were used inside the
  parallel loop instead of the globals, with the copy back to the globals
  happening at the end.
- `schedule(runtime)` was chosen so the work-sharing policy can be set
  through the `OMP_SCHEDULE` environment variable (static / dynamic / guided)
  without recompiling. The per-iteration workload is **uneven** (larger
  numbers need more divisions), so a dynamic schedule balances the load
  better — the final choice is justified in the report based on the
  measurements.

### Part 2 — `mergesort.c`
`mergesort_parallel()` was implemented with OpenMP tasks:

- The two halves of the array are each sorted in their own
  `#pragma omp task`, followed by a `#pragma omp taskwait` before the final
  `merge()`, ensuring both have finished.
- The call from `main()` sits inside `#pragma omp parallel` with
  `#pragma omp single`, so one thread creates the tasks and the whole team
  executes them.
- **Cutoff (`TASK_CUTOFF = 8192`):** below this size the serial
  `mergesort_serial()` is called directly. This avoids the task-creation
  overhead on small chunks, where serial execution is faster — exactly the
  optimization the handout hints at.

### Part 3 — `final` & `mergeable` clauses
Applied to the merge sort behind conditional compilation (`#ifdef
USE_FINAL_MERGEABLE`):

- With `USE_FINAL_MERGEABLE` defined, tasks get
  `final(n <= FINAL_CUTOFF) mergeable`.
- `final` stops the creation of new (nested) tasks below the
  `FINAL_CUTOFF = 32768` threshold: the task becomes "final" and the rest of
  the recursion runs serially inside it.
- `mergeable` lets the runtime, when the task is final/included, skip
  creating a new data environment and reuse the parent's instead, cutting
  overhead.
- The comparison of the two builds (with/without the clauses) and whether
  there is any practical gain is presented in the report.

## Deliverables

- `primes.c`, `mergesort.c` — source code
- `MYE023_Ergasia1.pdf` — full report with timings, charts, and discussion
  of the results
