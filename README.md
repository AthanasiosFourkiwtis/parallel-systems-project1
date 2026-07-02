# MYE023 – Parallel Systems and Programming — Assignment #1 (OpenMP)

Parallelizing two serial programs with OpenMP, timing them and comparing
with the serial versions.

**Student:** Athanasios Fourkiotis (ID 4940)
**Academic year:** 2025–26 · **Instructor:** Vassilios Dimakopoulos

## What's in here

| File | What it is |
|---|---|
| `primes.c` | Finding primes — serial + parallel (OpenMP) version |
| `mergesort.c` | Merge sort — serial + parallel (OpenMP tasks) version |
| `hw1.pdf` | The assignment description |
| `MYE023_Ergasia1.pdf` | My report with timings, charts and discussion |
| `*.exe` | Compiled executables (Windows) |

## Part 1 — Parallelizing the prime counting (40%)

`openmp_primes()` parallelizes the prime-finding loop without changing the
algorithm:

- `#pragma omp parallel for` on the loop over the odd numbers.
- `private(num, divisor, quotient, remainder)` for the helper variables.
- `reduction(+:count)` for the prime count and `reduction(max:lastprime)`
  for the biggest prime.
- `schedule(runtime)`, so I can try the different scheduling policies
  (static / dynamic / guided) through `OMP_SCHEDULE` and pick the best
  one (the work per iteration is uneven).

## Part 2 — Merge sort with OpenMP tasks (40%)

`mergesort_parallel()` does the recursive sort with OpenMP tasks:

- Each half gets sorted in its own `#pragma omp task`, with a `taskwait`
  before the final `merge`.
- It's called inside `#pragma omp parallel` / `#pragma omp single` from `main()`.
- **Cutoff (`TASK_CUTOFF`):** for small subarrays I fall back to the serial
  version, since making tasks costs more than it saves there.

## Part 3 — Taskloop / `final` & `mergeable` clauses (20%)

Trying the `final` and `mergeable` clauses (OpenMP 5.0) on the merge sort,
with conditional compilation:

```sh
gcc -O2 -fopenmp -DUSE_FINAL_MERGEABLE mergesort.c -o mergesort_c
```

With `USE_FINAL_MERGEABLE` defined, the tasks get
`final(n <= FINAL_CUTOFF) mergeable`, so below a size limit no new tasks
get created. The discussion and the results are in the report.

## Build

```sh
gcc -O2 -fopenmp primes.c    -o primes
gcc -O2 -fopenmp mergesort.c -o mergesort
gcc -O2 -fopenmp -DUSE_FINAL_MERGEABLE mergesort.c -o mergesort_c
```

## Run

```sh
./primes                       # N is fixed at 10,000,000 (UPTO)
./mergesort 20                 # array of 20,971,520 elements (20 * 1024 * 1024)
```

Testing with 1, 2, 3 and 4 threads:

```sh
OMP_NUM_THREADS=4 OMP_SCHEDULE=dynamic ./primes
```

Every measurement is the average of at least 4 runs on a
4-core processor.
