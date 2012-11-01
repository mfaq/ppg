#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <assert.h>

#ifndef CLOCK_MONOTONIC_RAW
# define CLOCK_MONOTONIC_RAW CLOCK_MONOTONIC
#endif

#define NUMPROCS 2
#define N (256UL*1000*1000)
static int *a;
static long *results;

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

static void
compute(size_t i, size_t n, int proc)
{
	long result = 0;

	for (; i < n; i++)
		result += a[i];

	results[proc] = result;
}

int
main(int argc, char **argv)
{
	int numprocs = argv[1] ? strtol(argv[1], NULL, 10) : NUMPROCS;
	size_t nelem = argv[1] && argv[2] ? strtoul(argv[2], NULL, 10)*1000*1000 : N;
	long long start, stop;
	pid_t procs[numprocs];
	int status[numprocs];
	long result = 0;
	
	isatty(1) && printf("%d procs, N=%zu\n", numprocs, nelem);

	a = malloc(nelem*sizeof(int));
	if (!a) {
		perror("malloc");
		exit(1);
	}

	results = mmap(NULL, numprocs*sizeof(long), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	if (results == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
	
	init_a(nelem);

	start = time_get_usec();

	/* computation */
	for (int i = 0; i < numprocs; i++) {
		size_t j = i * (nelem / numprocs);
		size_t n = (i+1) * (nelem / numprocs);

		switch (procs[i] = fork()) {
			case -1:
				perror("fork");
				exit(1);
				break;

			case 0:
				compute(j,n,i);
				exit(0);
				break;

			default:
				/* nothing */
				break;
		}
	}
	
	for (int i = 0; i < numprocs; i++) {
		waitpid(procs[i], &status[i], 0);
		result += results[i];
	}

	stop = time_get_usec();

	assert(result == ((nelem*(nelem+1LL)) / 2LL));

	if (isatty(1))
		printf("result: %ld\ntook %lld usec (%f sec)\n", result, stop-start, ((double)stop-start) / 1e6);
	else
		printf("%zu %lld\n", nelem, stop-start);

	free(a);
	munmap(results, numprocs*sizeof(long));

	return 0;
}

