#ifndef MQUEUE_H
#define MQUEUE_H

#include <pthread.h>

typedef struct {
	int head;
	int tail;
	int qsize;
	int cap;
	pthread_mutex_t mu;
	pthread_cond_t cond;
	int *data;
} mqueue_t;

mqueue_t *mq_create(int cap);
void mq_destroy(mqueue_t *mq);
void mq_send(mqueue_t *mq, int msg);
int mq_recv(mqueue_t *mq);

#endif

