/* AM: 4940 FOYRKIOTIS ATHANASIOS */

/* MYE023 - A simple recursive mergesort */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>


/* Allocate, and shuffle an array of all integers up to n */
int *random_array(int n) /* builds an array of values and shuffles them randomly */
{
	int i, j, *arr, t;

	if ((arr = (int *) malloc(n*sizeof(int))) == NULL)
		return NULL;
	for (i = 0; i < n; i++)
		arr[i] = i;
	for (i = 0; i < n-1; i++)
	{
		j = i + rand() / (RAND_MAX / (n - i) + 1);
		t = arr[j];
		arr[j] = arr[i];
		arr[i] = t;;
	}
	return arr;
}


/* Call this to make sure the array is sorted correctly */
int check_sorted(int *arr, int n)  /* checks that the array came out correctly sorted */
{
	int i;
	for (i = 0; i < n; i++)
		if (arr[i] != i)
			return 0;
	return 1;
}


/* Merge the two halves of arr using tmp as temporary storage */
void merge(int *arr, int n, int *tmp) /* the final merge step: takes 2 already-sorted chunks and joins them into a bigger one */
{
	int i = 0, j = n/2, k = 0;

	while (i < n/2 && j < n)
		tmp[k++] = (arr[i] <= arr[j]) ? arr[i++] : arr[j++];
	while (i < n/2)
		tmp[k++] = arr[i++];
	while (j < n)
		tmp[k++] = arr[j++];

	memcpy(arr, tmp, n*sizeof(int));
}


/* Simple insertion sort which is quite faster on small arrays */
void insertion_sort(int *arr, int n) /* used on small arrays, where it is faster */
{
	int i, j, x;

	for (i = 1; i < n; i++)
	{
		for (x = arr[i], j = i-1; j >= 0 && arr[j] > x; j--)
			arr[j + 1] = arr[j];
		arr[j + 1] = x;
	}
}


/* Sequential, recursive implementation */
void mergesort_serial(int *arr, int n, int *tmp)
{
	if (n < 2) return; /* an array of 0 or 1 elements is already sorted */

	/* If n is small, then merge sort is very slow... */
	if (n > 32) /* threshold for using merge sort */
	{
		/* Sort the first half */
		mergesort_serial(arr, n/2, tmp); /* the first call sorts the first half */

		/* Sort the second half */
		mergesort_serial(arr+n/2, n-n/2, tmp+n/2); /* the second call sorts the second half */
		/* so neither depends on the other */
	
		/* merge sorted halves into a totally sorted array */
		merge(arr, n, tmp); 
	}
	else
		insertion_sort(arr, n);
}
#define TASK_CUTOFF 8192   /* part B */
#define FINAL_CUTOFF 32768 /* part C */
/* Task-based OpenMP implementation */
void mergesort_parallel(int *arr, int n, int *tmp)
{
	 if (n < 2)
        return;

    if (n <= TASK_CUTOFF) {
        mergesort_serial(arr, n, tmp);
        return;
    }
	/* Baseline task version for part B */
    /* Compile with -DUSE_FINAL_MERGEABLE for part C */
#ifdef USE_FINAL_MERGEABLE
	#pragma omp task shared(arr, tmp) firstprivate(n) final(n <= FINAL_CUTOFF) mergeable
#else
    #pragma omp task shared(arr, tmp) firstprivate(n)
#endif
    mergesort_parallel(arr, n/2, tmp);

#ifdef USE_FINAL_MERGEABLE
    #pragma omp task shared(arr, tmp) firstprivate(n) final(n <= FINAL_CUTOFF) mergeable
#else
    #pragma omp task shared(arr, tmp) firstprivate(n)
#endif
    mergesort_parallel(arr + n/2, n - n/2, tmp + n/2);

    #pragma omp taskwait
    merge(arr, n, tmp);

}


int main(int argc, char *argv[])
{
	int n, *array, *tmp;
	double time;

	if (argc < 2)
	{
		EXIT:
		printf("Pass the size of the array (in millions of elements).\n");
		exit(1);
	}

	n = atoi(argv[1]);
	if (n < 1)
		goto EXIT;
	n *= 1024*1024;

	if ((array = random_array(n)) == NULL)
		exit(1);
	if ((tmp = (int *) malloc(n*sizeof(int))) == NULL)
		exit(1);

  /* Time the sequential algorithm */
	time = omp_get_wtime();
	mergesort_serial(array, n, tmp);
    time = omp_get_wtime() - time;

	if (!check_sorted(array, n))
	{
		SORTERROR:
		fprintf(stderr, "Error in sorting\n");
		exit (1);
	}
	printf("sequential time = %lf sec\n", time);

	/* Prepare a new array for the parallel version */
	free(array);
	if ((array = random_array(n)) == NULL)
		exit(1);

	/* Repeat the above for the parallel (OpenMP) version */
	time = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp single
        mergesort_parallel(array, n, tmp);
    }

    time = omp_get_wtime() - time;

    if (!check_sorted(array, n))
        goto SORTERROR;

    printf("parallel time = %lf sec\n", time);

    free(array);
    free(tmp);

    return 0;
}
