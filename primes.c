/* AM: 4940 FOYRKIOTIS ATHANASIOS */

#include <stdio.h>
#include <omp.h>

#define UPTO 10000000

long int count,      /* number of primes */
         lastprime;  /* the last prime found */


void serial_primes(long int n) {
	long int i, num, divisor, quotient, remainder; /* topikes metavlites */

	if (n < 2) return;
	count = 1;                         /* to 2 einai o prwtos prwtos arithmos */
	lastprime = 2;

	for (i = 0; i < (n-1)/2; ++i) {    /* gia kathe peritto arithmo */
		num = 2*i + 3;

		divisor = 1;
		do 
		{
			divisor += 2;                  /* arxikopoieitai sto 1 kai meta +=2 gia na ksekinisei me 3 kai kathe loop na anevenei ana 2 px 3 ,5,7,.. */
			quotient  = num / divisor;     /* ypologizw to phliko gia na synexisw ton elegxo mexri na perasw to simeio quotient/divisor */
			remainder = num % divisor;    /* ypologizei ypolipo diareshs gia na elegxw an o arithmos exei akrivh diaireth*/
		} while (remainder && divisor <= quotient); /* synexizw oso den exei vrethei akrivis diaireths kai oso den exw perasei to simeio tou sqrt(num) */

		if (remainder || divisor == num) /* an den vrethike diaireths, o num einai prwtos */
		{
			count++; /* ayksanei to plhthos twn prwtwn*/
			lastprime = num; /* enhmerwnei to megalitero prwto poy vrethike mexri twra*/
		}
	}
}


void openmp_primes(long int n) {
	long int i, num, divisor, quotient, remainder;
	long int topiko_count,topiko_lastprime; /*gia na min doulevw kateutheian me tis global*/
	if (n < 2) return;
	topiko_count = 1;                         /* 2 is the first prime */
	topiko_lastprime = 2;
	
	#pragma omp parallel for private(num, divisor, quotient, remainder) \
                         reduction(+:topiko_count) reduction(max:topiko_lastprime) \
                         schedule(runtime)
	
	 for (i = 0; i < (n-1)/2; ++i) { /* ayto to kommati parallhlopoiw gt se kathe epanalispi ftiaxnw diaforetiko perrito arithmo*/
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
            topiko_count++;
            if (num > topiko_lastprime)
                topiko_lastprime = num;
        }
    }

    count = topiko_count;
    lastprime = topiko_lastprime;
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