/*
 *      queue.h
 *
 * This file contains the two structs used in the queue and a couple of functions to be implemented for
 * the queue.
 *
 */

#ifndef queue_h
#define queue_h

#include "process.h"

struct node{
	process * job;
	struct node * next;
};

typedef struct{
	int count;
	struct node * head;
	struct node * tail;
} queue;

void initialize(queue*);
int isEmpty(queue*);
int length(queue *);
void push(queue*, process*);
void pushByPriority(queue*, process*);
void pushByService(queue*, process*);
void pushByArrival(queue*, process*);
process * pop(queue*);
queue * copy(queue *);

#endif
