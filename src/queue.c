/*
 *      queue.c
 *
 * This is the queue implementation which sorts nodes by depending on the push function used.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "process.h"
#include "queue.h"

/*
 *
 * name: isEmpty
 *
 * Checks to see if the given queue has any nodes in it.
 *
 * @param	q	the queue to be checked
 * @return	0 if false, 1 if true
 */
int isEmpty(queue * q) {
	return (q->count == 0);
}

/*
 *
 * name: initialize
 *
 * Simply insures that the queue's count is set to 0 before beginning.
 *
 * @param	q	the queue to be initialized
 */
void initialize(queue * q){
	q->count = 0;
}

/*
 *
 * name: pushByService
 *
 * Pushes the given process job onto the given queue in an order so that the jobs with the lowest
 * service time are first
 *
 * @param	q	the queue to be pushed onto
 * @param	j	the process job to be inserted into the node of the queue
 */
void pushBySerivce(queue * q, process * j) {
	struct node * newNode;
	newNode = malloc(sizeof(struct node));
	if(newNode == NULL){
		printf("out of memory");
		exit(1);
	}

	newNode->job = j;

	if(!isEmpty(q)){
		// some older code which finds where to insert
		struct node * before;
		struct node * after;
		before = q->head;
		after = q->head;
		while(after != NULL && j->service >= after->job->service){
			before = after;
			after = after->next;
		}

		newNode->next = after;

		if(after != q->head){
			before->next = newNode;
		}
		else{
			q->head = newNode;
		}

		q->count++;
	}
	else{
		q->head = newNode;
		q->head->next = NULL;
		q->tail = q->head;
		q->count = 1;
	}
}


/*
 *
 * name: pushByPriority
 *
 * Pushes the given process job onto the given queue in an order so that the jobs with the highest
 * priorty are first
 *
 * @param	q	the queue to be pushed onto
 * @param	j	the process job to be inserted into the node of the queue
 */
void pushByPriority(queue * q, process * j) {
	struct node * newNode;
	newNode = malloc(sizeof(struct node));
	if(newNode == NULL){
		printf("out of memory");
		exit(1);
	}

	newNode->job = j;

	if(!isEmpty(q)){
		// some older code which finds where to insert
		struct node * before;
		struct node * after;
		before = q->head;
		after = q->head;
		while(after != NULL && j->priority <= after->job->priority){
			before = after;
			after = after->next;
		}

		newNode->next = after;

		if(after != q->head){
			before->next = newNode;
		}
		else{
			q->head = newNode;
		}

		q->count++;
	}
	else{
		q->head = newNode;
		q->head->job = j;
		q->head->next = NULL;
		q->tail = q->head;
		q->count = 1;
	}
}

/*
 *
 * name: push
 *
 * Pushes the given process job onto the given queue in an order of arrival (FIFO)
 *
 * @param	q	the queue to be pushed onto
 * @param	j	the process job to be inserted into the node of the queue
 */
void push(queue * q, process * j) {
	struct node * newNode;
	newNode = malloc(sizeof(struct node));
	if(newNode == NULL){
		printf("out of memory");
		exit(1);
	}

	newNode->job = j;
	newNode->next = NULL;

	if(!isEmpty(q)){
		q->tail->next = newNode;
		q->tail = newNode;
		q->count++;
	}
	else{
		q->head = newNode;
		q->tail = q->head;
		q->count = 1;
	}
}

/*
 *
 * name: pop
 *
 * Pops the head of the queue, removes the head and returns the head's job.
 *
 * @param	q	the queue to be popped from
 * @return	the process job that was popped
 */
process * pop(queue * q){
	process * returning;
	if(!isEmpty(q)){
		returning = q->head->job;
		struct node * deleting = q->head;
		q->head = q->head->next;
		q->count--;
		free(deleting);
	}
	return returning;
}
