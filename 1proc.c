#define _XOPEN_SOURCE 700

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

#ifndef CLOCK_MONOTONIC_RAW
# define CLOCK_MONOTONIC_RAW CLOCK_MONOTONIC
#endif

#define N (256UL*1000*1000)
static int *a;

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

static long
compute(size_t i, size_t n)
{
	long result = 0;

	for (; i < n; i++)
		result += a[i];

	return result;
}

int
main(int argc, char **argv)
{
	long long start, stop;
	long result;
	size_t nelem = argv[1] && !argv[2] ? strtoul(argv[1], NULL, 10)*1000*1000 : N;
	nelem = argv[1] && argv[2] ? strtoul(argv[2], NULL, 10)*1000*1000 : nelem;

	isatty(1) && printf("N=%zu\n", nelem);

	a = malloc(nelem*sizeof(int));
	if (!a) {
		perror("malloc");
		exit(1);
	}
	
	init_a(nelem);

	start = time_get_usec();

	result = compute(0, nelem);

	stop = time_get_usec();

	assert(result == ((nelem*(nelem+1LL)) / 2LL));

	if (isatty(1))
		printf("result: %ld\ntook %lld usec (%f sec)\n", result, stop-start, ((double)stop-start) / 1e6);
	else
		printf("%zu %lld\n", nelem, stop-start);

	free(a);

	return 0;
}

