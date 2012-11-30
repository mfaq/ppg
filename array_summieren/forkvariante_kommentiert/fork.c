#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <assert.h>

/* um portabel zu bleiben, CLOCK_MONOTONIC_RAW auf Systemen, *
 * die es nicht bieten, durch das von POSIX garantierte      *
 * CLOCK_MONOTONIC ersetzen.                                 */
#ifndef CLOCK_MONOTONIC_RAW
# define CLOCK_MONOTONIC_RAW CLOCK_MONOTONIC
#endif

#define NUMPROCS 2
#define N (1UL*1000*1000)
static int *a;
static long *results;

/* nutzt clock_gettime, um eine Zeit in Mikrosekunden zu erhalten */
static long long
time_get_usec(void)
{
	long long ret;
	struct timespec ts;
	int rc;

	rc = clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	if (rc == -1)
		perror("clock_gettime");

	/* Skalieren */
	ret = (long long) ts.tv_sec * 1000000LL;
	ret += (long long) ts.tv_nsec / 1000LL;

	return ret;
}

/* Initialisierung des Arrays, nichts besonderes zu sehen */
static void
init_a(size_t n)
{
	for (size_t i = 0; i < n; i++)
		a[i] = i+1;
}

/* jeder Prozess summiert die Zahlen mit den Indizes  von i bis n-1 *
 * Ergebnisse werden in den Shared-Memory-Bereich geschrieben */
static void
compute(size_t i, size_t n, int proc)
{
	long result = 0;

	for (; i < n; i++)
		result += a[i];

	results[proc] = result;
}

/* Aufruf: fork [Anzahl Prozesse] [N in Millionen]
 * ohne Argumente aufgerufen: 2 Prozesse, N = 1e6
 *
 * Funktionsweise:
 * Nach der Initialisierung befinden sich im Speicher, auf den a zeigt,
 * aufsteigend die Zahlen von 1 bis N. Für jeden zu startenden Prozess
 * (1. Argument des Programms) wird Speicherplatz für das Teilergebnis 
 * im Shared-Memory-Bereich benötigt. Der Kontrollprozess spaltet nun
 * nach und nach die Prozesse ab und wartet auf deren Beendigung.
 * Nach Beendigung eines (Rechen-)Prozesses kann das Teilergebnis aus
 * dem Shared-Memory-Bereich gelesen werden. Die Summe der Teilergebnisse
 * ergibt das Gesamtergebnis.                                              */
int
main(int argc, char **argv)
{
	/* Anzahl der rechnenden Prozesse */
	int numprocs = argv[1] ? strtol(argv[1], NULL, 10) : NUMPROCS; 
	/* Anzahl der zu summierenden Zahlen */
	size_t nelem = argv[1] && argv[2] ? strtoul(argv[2], NULL, 10)*1000*1000 : N;

	long long start, stop;
	pid_t procs[numprocs];
	int status[numprocs];
	long result = 0;
	
	/* Ausgabe erfolgt nur wenn stdout/fd1 ein Terminal ist */
	isatty(1) && printf("%d procs, N=%zu\n", numprocs, nelem);

	/* Speicher für N ints beschaffen */
	a = malloc(nelem*sizeof(int));
	if (!a) {
		perror("malloc");
		exit(1);
	}

	/* Erstellen des (anonymen) Shared-Memory-Bereichs für den Austausch
	 * der (Teil-)Ergebnisse */
	results = mmap(NULL, numprocs*sizeof(long), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	if (results == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
	
	init_a(nelem);

	/* Zeit läuft ... */
	start = time_get_usec();

	/* für jeden zu erzeugenden Prozess  */
	for (int i = 0; i < numprocs; i++) {
		/* Start- und Endindizes bestimmen */
		size_t j = i * (nelem / numprocs);
		size_t n = (i+1) * (nelem / numprocs);

		switch (procs[i] = fork()) {
			case -1:
				perror("fork");
				exit(1);
				break;

			case 0: /* Kind zur Summenberechnung schicken */
				compute(j,n,i);
				/* Kind wird nicht mehr benötigt */
				exit(0);
				break;

			default:
				/* nothing */
				break;
		}
	}
	
	/* für jeden geforkten Prozess */
	for (int i = 0; i < numprocs; i++) {
		/* auf Beendigung warten */
		waitpid(procs[i], &status[i], 0);
		/* Teilergebnis nun in results[i], Teilergebniss aufaddieren */
		result += results[i];
	}

	/* Gesamtergebnis liegt nun vor, Zeit stoppen */
	stop = time_get_usec();

	/* Ergebnis überprüfen, wenn falsch -> abort() */
	assert(result == ((nelem*(nelem+1LL)) / 2LL));

	if (isatty(1))
		/* ausführlichere Ausgabe von Ergebnis und Laufzeit auf Terminal */
		printf("result: %ld\ntook %lld usec (%f sec)\n", result, stop-start, ((double)stop-start) / 1e6);
	else
		/* strukturierte Ausgabe ansonsten (bei Umleitung in Datei z.B.) */
		printf("%zu %lld\n", nelem, stop-start);

	/* Resourcen freigeben */
	free(a);
	munmap(results, numprocs*sizeof(long));

	return 0;
}

