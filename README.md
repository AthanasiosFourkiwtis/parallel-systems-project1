# MYE023 – Parallel Systems and Programming — Assignment #1 (OpenMP)

Parallelizing two serial programs with OpenMP, timing them, and comparing
against the serial versions.

**Student:** Athanasios Fourkiotis (ID 4940)
**Academic year:** 2025–26 · **Instructor:** Vassilios Dimakopoulos

## Contents

| File | Description |
|---|---|
| `primes.c` | Prime counting — serial + parallel (OpenMP) version |
| `mergesort.c` | Merge sort — serial + parallel (OpenMP tasks) version |
| `hw1.pdf` | Assignment handout |
| `MYE023_Ergasia1.pdf` | Written report with timings, charts, and discussion |
| `*.exe` | Compiled executables (Windows) |

## Part 1 — Parallelizing prime counting (40%)

`openmp_primes()` parallelizes the prime-finding loop without changing the
algorithm:

- `#pragma omp parallel for` on the loop over odd numbers.
- `private(num, divisor, quotient, remainder)` for the helper variables.
- `reduction(+:count)` for the prime count and `reduction(max:lastprime)`
  for the largest prime.
- `schedule(runtime)`, so alternative work-sharing policies
  (static / dynamic / guided) can be tried via `OMP_SCHEDULE` and the best
  one picked (the per-iteration workload is uneven).

## Part 2 — Merge sort with OpenMP tasks (40%)

`mergesort_parallel()` implements the recursive sort with OpenMP tasks:

- Each half is sorted in its own `#pragma omp task`, with a `taskwait`
  before the final `merge`.
- It is invoked inside `#pragma omp parallel` / `#pragma omp single` from `main()`.
- **Cutoff (`TASK_CUTOFF`):** small subarrays fall back to the serial
  version, since spawning tasks costs more than it saves at that size.

## Part 3 — Taskloop / `final` & `mergeable` clauses (20%)

Trying out the `final` and `mergeable` clauses (OpenMP 5.0) on the merge
sort, behind conditional compilation:

```sh
gcc -O2 -fopenmp -DUSE_FINAL_MERGEABLE mergesort.c -o mergesort_c
```

With `USE_FINAL_MERGEABLE` defined, tasks get
`final(n <= FINAL_CUTOFF) mergeable`, so no new tasks are spawned below a
size threshold. The discussion and results are in the report.

## Building

```sh
gcc -O2 -fopenmp primes.c    -o primes
gcc -O2 -fopenmp mergesort.c -o mergesort
gcc -O2 -fopenmp -DUSE_FINAL_MERGEABLE mergesort.c -o mergesort_c
```

## Running

```sh
./primes                       # N fixed at 10,000,000 (UPTO)
./mergesort 20                 # array of 20,971,520 elements (20 * 1024 * 1024)
```

Testing with 1, 2, 3, and 4 threads:

```sh
OMP_NUM_THREADS=4 OMP_SCHEDULE=dynamic ./primes
```

Every measurement is the average of at least 4 runs on a
4-core processor.
