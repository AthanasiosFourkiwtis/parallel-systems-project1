/* AM: 4940 FOYRKIOTIS ATHANASIOS */

#include <stdio.h>
#include <omp.h>

#define UPTO 10000000

long int count,      /* number of primes */
         lastprime;  /* the last prime found */


void serial_primes(long int n) {
	long int i, num, divisor, quotient, remainder; /* local variables */

	if (n < 2) return;
	count = 1;                         /* 2 is the first prime */
	lastprime = 2;

	for (i = 0; i < (n-1)/2; ++i) {    /* for every odd number */
		num = 2*i + 3;

		divisor = 1;
		do 
		{
			divisor += 2;                  /* starts at 1 and then +=2, so it begins at 3 and steps by 2 each loop: 3, 5, 7, ... */
			quotient  = num / divisor;     /* compute the quotient so the check can continue until we pass the quotient/divisor point */
			remainder = num % divisor;    /* compute the remainder to check whether the number has an exact divisor */
		} while (remainder && divisor <= quotient); /* keep going while no exact divisor is found and sqrt(num) is not yet passed */

		if (remainder || divisor == num) /* if no divisor was found, num is prime */
		{
			count++; /* bump the prime count */
			lastprime = num; /* update the largest prime found so far */
		}
	}
}


void openmp_primes(long int n) {
	long int i, num, divisor, quotient, remainder;
	long int local_count,local_lastprime; /* so the loop never touches the globals directly */
	if (n < 2) return;
	local_count = 1;                         /* 2 is the first prime */
	local_lastprime = 2;
	
	#pragma omp parallel for private(num, divisor, quotient, remainder) \
                         reduction(+:local_count) reduction(max:local_lastprime) \
                         schedule(runtime)
	
	 for (i = 0; i < (n-1)/2; ++i) { /* this is the parallelized part: every iteration produces a different odd number */
        num = 2*i + 3;

        divisor = 1;
        do
        {
            divisor += 2;
            quotient  = num / divisor;
            remainder = num % divisor;
        } while (remainder && divisor <= quotient);

        if (remainder || divisor == num)
        {
            local_count++;
            if (num > local_lastprime)
                local_lastprime = num;
        }
    }

    count = local_count;
    lastprime = local_lastprime;
}

int main()
{
    double time;

    printf("Serial and parallel prime number calculations:\n\n");

    /* Time the sequential algorithm */
    time = omp_get_wtime();
    serial_primes(UPTO);
    time = omp_get_wtime() - time;
    printf("[serial] count = %ld, last = %ld (time = %lf)\n", count, lastprime, time);

    /* Time the OpenMP algorithm */
    time = omp_get_wtime();
    openmp_primes(UPTO);
    time = omp_get_wtime() - time;
    printf("[openmp] count = %ld, last = %ld (time = %lf)\n", count, lastprime, time);

    return 0;
}