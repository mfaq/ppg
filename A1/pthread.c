#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <assert.h>

#ifndef CLOCK_MONOTONIC_RAW
# define CLOCK_MONOTONIC_RAW CLOCK_MONOTONIC
#endif

#define NUMPROCS 2
#define N (256UL*1000*1000)
static int *a;

struct thread_data {
	size_t i;
	size_t n;
};

static long long
time_get_usec(void)
{
	long long ret;
	struct timespec ts;
	int rc;

	rc = clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	if (rc == -1)
		perror("clock_gettime");

	ret = (long long) ts.tv_sec * 1000000LL;
	ret += (long long) ts.tv_nsec / 1000LL;

	return ret;
}

static void
init_a(size_t n)
{
	for (size_t i = 0; i < n; i++)
		a[i] = i+1;
}

static void *
compute(void *tdata)
{
	long result = 0;
	struct thread_data *td = (struct thread_data *) tdata;

	for (size_t i = td->i; i < td->n; i++)
		result += a[i];

	return (void *) result;
}

int
main(int argc, char **argv)
{
	int numprocs = argv[1] ? strtol(argv[1], NULL, 10) : NUMPROCS;
	size_t nelem = argv[1] && argv[2] ? strtoul(argv[2], NULL, 10)*1000*1000 : N;
	long long start, stop;
	pthread_t procs[numprocs];
	intptr_t status[numprocs];
	struct thread_data td[numprocs];
	long result = 0;
	
	isatty(1) && printf("%d procs, N=%zu\n", numprocs, nelem);
	

	a = malloc(nelem*sizeof(int));
	if (!a) {
		perror("malloc");
		exit(1);
	}

	init_a(nelem);

	start = time_get_usec();

	/* computation */
	for (int i = 0; i < numprocs; i++) {
		size_t j = i * (nelem / numprocs);
		size_t n = (i+1) * (nelem / numprocs);
		td[i] = (struct thread_data) { .i=j, .n=n };

		int rc = pthread_create(&procs[i], NULL, compute, &td[i]);
		if (rc) {
			perror("pthread_create");
			exit(1);
		}
	}

	for (int i = 0; i < numprocs; i++) {
		int rc = pthread_join(procs[i], (void **) &status[i]);
		if (rc) {
			perror("pthread_join");
			exit(1);
		}
		result += status[i];
	}
	
	stop = time_get_usec();

	assert(result == ((nelem*(nelem+1L)) / 2L));

	if (isatty(1))
		printf("result: %ld\ntook %lld usec (%f sec)\n", result, stop-start, ((double)stop-start) / 1e6);
	else
		printf("%zu %lld\n", nelem, stop-start);

	free(a);

	return 0;
}

