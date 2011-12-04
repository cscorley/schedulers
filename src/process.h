/*
 *      process.h
 *
 * This file contains the struct that processes will be made from.
 *
 */

#ifndef process_h
#define process_h

typedef struct{
	char name[2];
	int arrival;
	int service;
	int priority;
	int timeleft;
} process;

#endif
