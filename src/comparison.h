/*
 *      comparison.h
 *
 * This is the main file for the scheduling algorithm simulations.  It will
 * get the input file location from the user and run the scheduler function upon that
 * set of read data for each algorithm.
 *
 */

#ifndef comparison_h
#define comparison_h

#include "process.h"
#include "queue.h"


int readJobs(process *, char *);
double calculateWait(char *, int);
void roundRobinScheduler(process *, int, char *);
void shortestJobScheduler(process *, int, char *);
void highestPriorityScheduler(process *, int, char *);

int main(int, char**);

#endif
