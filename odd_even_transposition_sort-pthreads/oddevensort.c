#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "mqueue.h"


struct thread_data {
	int n;
	int rank;
	int *array;
	mqueue_t **mq;
};

void *oddeven(void *tdata)
{
	struct thread_data *td = (struct thread_data *) tdata;
	int n = td->n;
	int rank = td->rank;
	mqueue_t **procs = td->mq;
	int val = td->array[rank];

	for (int i = 0; i < n; i++) {
		if ((i & 1) == (rank & 1)) { /* proc has active role */
			if (rank < (n-1)) { /* not on the outermost right */
				int r = mq_recv(procs[rank]);
				if (r < val) {
					mq_send(procs[rank+1], val);
					val = r;
				} else {
					mq_send(procs[rank+1], r);
				}
			}
		} else { /* passive role */
			if (rank > 0) {  /* not the left edge */
				mq_send(procs[rank-1], val);
				val = mq_recv(procs[rank]);
			}
		}
	}
	td->array[rank] = val;

	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	int n;

	if (argc < 2)
		n = 8;
	else
		n = atoi(argv[1]);

	int *numbers = malloc(n * sizeof(int));
	srand(time(NULL));

	for (int i = 0; i < n; i++) {
		numbers[i] = rand() % 100;
		printf("%d ", numbers[i]);
	}
	putchar('\n');

	pthread_t threads[n];
	mqueue_t *mq[n];
	struct thread_data *td = malloc(n * sizeof(struct thread_data));

	for (int i = 0; i < n; i++) {
		mq[i] = mq_create(10);
		td[i].n = n;
		td[i].rank = i;
		td[i].array = numbers;
		td[i].mq = mq;
	}

	for (int i = 0; i < n; i++) {
		int ret = pthread_create(&threads[i], NULL, oddeven, &td[i]);
		if (ret) {
			printf("ERROR: pthread_create() returned %d\n", ret);
			exit(1);
		}
	}

	void *status;

	for (int i = 0; i < n; i++) {
		int ret = pthread_join(threads[i], &status);
		if (ret) {
			printf("ERROR: pthread_join() returned %d for thread %d\n", ret, i);
			exit(1);
		}
	}

	for (int i = 0; i < n; i++) {
		printf("%d ", numbers[i]);
	}
	putchar('\n');

	free(td);
	free(numbers);

	for (int i = 0; i < n; i++)
		mq_destroy(mq[i]);

	pthread_exit(NULL);
	return 0;
}

