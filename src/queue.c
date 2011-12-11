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
 * name: length
 *
 * Checks to see if the given queue has any nodes in it.
 *
 * @param	q	the queue to be checked
 * @return	length of queue q
 */
int length(queue * q) {
	return q->count;
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

    //printf("job push: %s %d %d %d\n", j->name, j->arrival, j->service, j->priority);
    //fflush(stdout);
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
 * name: pushByOrdered
 *
 * Pushes the given process job onto the given queue in an order according to
 * the comp_func
 *
 * @param	q	the queue to be pushed onto
 * @param	j	the process job to be inserted into the node of the queue
 * @param   comp   function which decides how the list will be sorted
 */
void pushOrdered(queue * q, process * j, bool(* comp)(process *, process *)) {
	struct node * newNode;
	newNode = malloc(sizeof(struct node));
	if(newNode == NULL){
		printf("out of memory");
		exit(1);
	}
    //printf("job push: %s %d %d %d\n", j->name, j->arrival, j->service, j->priority);
    //fflush(stdout);

	newNode->job = j;

	if(!isEmpty(q)){
		// some older code which finds where to insert
		struct node * before;
		struct node * after;
		before = q->head;
		after = q->head;
		while(after != NULL && !comp(j, after->job)){
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
 * name: pop
 *
 * Pops the head of the queue, removes the head and returns the head's job.
 *
 * @param	q	the queue to be popped from
 * @return	the process job that was popped
 */
process * pop(queue * q){
    if(isEmpty(q)){
        return NULL;
    }

	process * returning;
    returning = q->head->job;
    struct node * deleting = q->head;
    q->head = q->head->next;
    q->count--;
    free(deleting);
    //printf("job pop: %s %d %d %d\n", returning->name, returning->arrival, returning->service, returning->priority);
    //fflush(stdout);

    return returning;
}

process * top(queue * q){
    return q->head->job;
}

queue * copy(queue * q){
    queue * nq = (queue *)malloc(sizeof(queue));
    if(nq == NULL){
        printf("out of memory");
        exit(1);
    }
    initialize(nq);

    struct node * nextNode;
    if(!isEmpty(q)){
        nextNode = q->head;
        while(length(nq) < length(q) && nextNode != NULL){
            push(nq, nextNode->job);
            nextNode = nextNode->next;
        }
    }

    return nq;
}

