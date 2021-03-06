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


void readJobs(queue *, char *);
double calculateWait(char *, int);
void roundRobinScheduler(queue *, char *, int);
void generalScheduler(queue *, char *, bool, bool(*)(process*,process*));

int main(int, char**);

void run(char*, int);
void generate(char*);

#endif
