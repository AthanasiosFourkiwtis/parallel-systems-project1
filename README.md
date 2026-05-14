# MYE023 – Παράλληλα Συστήματα και Προγραμματισμός — Εργασία #1 (OpenMP)

Παραλληλοποίηση δύο σειριακών προγραμμάτων με OpenMP, χρονομέτρηση και
σύγκριση με τη σειριακή έκδοση.

**Φοιτητής:** Φουρκιώτης Αθανάσιος (ΑΜ 4940)
**Ακαδημαϊκό έτος:** 2025–26 · **Διδάσκων:** Βασίλειος Δημακόπουλος

## Περιεχόμενα

| Αρχείο | Περιγραφή |
|---|---|
| `primes.c` | Εύρεση πρώτων αριθμών — σειριακή + παράλληλη (OpenMP) έκδοση |
| `mergesort.c` | Merge sort — σειριακή + παράλληλη (OpenMP tasks) έκδοση |
| `hw1.pdf` | Εκφώνηση της εργασίας |
| `MYE023_Ergasia1.pdf` | Γραπτή αναφορά με χρονομετρήσεις, γραφικές παραστάσεις και συζήτηση |
| `*.exe` | Μεταγλωττισμένα εκτελέσιμα (Windows) |

## Μέρος 1 — Παραλληλοποίηση εύρεσης πρώτων αριθμών (40%)

Η συνάρτηση `openmp_primes()` παραλληλοποιεί τον βρόχο εύρεσης πρώτων χωρίς
αλλαγή του αλγορίθμου:

- `#pragma omp parallel for` στον βρόχο των περιττών αριθμών.
- `private(num, divisor, quotient, remainder)` για τις βοηθητικές μεταβλητές.
- `reduction(+:count)` για το πλήθος των πρώτων και `reduction(max:lastprime)`
  για τον μεγαλύτερο πρώτο.
- `schedule(runtime)` ώστε να δοκιμαστούν εναλλακτικοί τρόποι διαμοιρασμού
  (static / dynamic / guided) μέσω της `OMP_SCHEDULE` και να επιλεγεί ο
  καλύτερος (το φορτίο ανά επανάληψη είναι ανομοιόμορφο).

## Μέρος 2 — Merge sort με OpenMP tasks (40%)

Η `mergesort_parallel()` υλοποιεί την αναδρομική ταξινόμηση με OpenMP tasks:

- Κάθε μισό ταξινομείται σε ξεχωριστό `#pragma omp task`, με `taskwait` πριν
  το τελικό `merge`.
- Καλείται μέσα σε `#pragma omp parallel` / `#pragma omp single` από την `main()`.
- **Cutoff (`TASK_CUTOFF`):** για μικρούς υποπίνακες χρησιμοποιείται η σειριακή
  έκδοση, αφού η δημιουργία tasks κοστίζει περισσότερο από το όφελος.

## Μέρος 3 — Taskloop / clauses `final` & `mergeable` (20%)

Δοκιμή των clauses `final` και `mergeable` (OpenMP 5.0) στο merge sort, με
conditional compilation:

```sh
gcc -O2 -fopenmp -DUSE_FINAL_MERGEABLE mergesort.c -o mergesort_c
```

Όταν οριστεί το `USE_FINAL_MERGEABLE`, τα tasks παίρνουν
`final(n <= FINAL_CUTOFF) mergeable`, ώστε κάτω από ένα όριο να μην
δημιουργούνται νέα tasks. Η σχετική συζήτηση και τα αποτελέσματα βρίσκονται
στην αναφορά.

## Μεταγλώττιση

```sh
gcc -O2 -fopenmp primes.c    -o primes
gcc -O2 -fopenmp mergesort.c -o mergesort
gcc -O2 -fopenmp -DUSE_FINAL_MERGEABLE mergesort.c -o mergesort_c
```

## Εκτέλεση

```sh
./primes                       # N ορισμένο στο 10.000.000 (UPTO)
./mergesort 20                 # πίνακας 20.000.000 στοιχείων (20 * 1024 * 1024)
```

Δοκιμή με 1, 2, 3 και 4 νήματα:

```sh
OMP_NUM_THREADS=4 OMP_SCHEDULE=dynamic ./primes
```

Κάθε μέτρηση λαμβάνεται ως μέσος όρος τουλάχιστον 4 εκτελέσεων, σε
4-πύρηνο επεξεργαστή.
