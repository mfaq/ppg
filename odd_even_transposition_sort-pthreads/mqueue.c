#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "mqueue.h"

mqueue_t *mq_create(int cap)
{
	mqueue_t *mq = malloc(sizeof(mqueue_t));
	if (mq == NULL)
		return NULL;

	memset(mq, 0, sizeof(mqueue_t));
	mq->cap = cap;
	
	if (pthread_cond_init(&mq->cond, NULL) != 0)
		goto cleanup;

	if (pthread_mutex_init(&mq->mu, NULL) != 0)
		goto cleanup;

	mq->data = malloc(cap * sizeof(int));
	if (mq->data == NULL) {
		goto cleanup;
	}

	return mq;

cleanup:
	// TODO: call cond_destroy/mutex_destroy when appropriate
	free(mq);
	return NULL;

}

void mq_destroy(mqueue_t *mq)
{
	free(mq->data);
	pthread_mutex_destroy(&mq->mu);
	pthread_cond_destroy(&mq->cond);
	free(mq);
}

void mq_send(mqueue_t *mq, int msg)
{
	int ret;
	if ((ret = pthread_mutex_lock(&mq->mu)) != 0)
		fprintf(stderr, "send: mutex_lock returned %d\n", ret);
	while (mq->qsize == mq->cap) {
		pthread_cond_wait(&mq->cond, &mq->mu);
	}
	mq->data[mq->tail++] = msg;
	if (mq->tail == mq->cap)
		mq->tail = 0;

	mq->qsize++;
	pthread_mutex_unlock(&mq->mu);
	pthread_cond_broadcast(&mq->cond);
	
}

int mq_recv(mqueue_t *mq)
{
	int ret;
	if ((ret = pthread_mutex_lock(&mq->mu)) != 0)
		fprintf(stderr, "send: mutex_lock returned %d\n", ret);

	while (mq->qsize == 0) {
		pthread_cond_wait(&mq->cond, &mq->mu);
	}
	int msg = mq->data[mq->head++];
	if (mq->head == mq->cap)
		mq->head = 0;

	mq->qsize--;
	pthread_mutex_unlock(&mq->mu);
	pthread_cond_broadcast(&mq->cond);
	
	return msg;
}

