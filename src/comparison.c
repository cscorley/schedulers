/*
 *      comparison.c
 *
 * This is the main file for the scheduling algorithm simulations.  It will
 * get the input file location from the user and run the scheduler function upon that
 * set of read data for each algorithm.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include "process.h"
#include "queue.c"

#define TRUE 1
#define FALSE 0
#define MAX_FILE_LEN 64
#define MAX_JOBS 32
#define MAX_QUANTUM 10

int readJobs(process *, char *);
double calculateWait(char *, int);
void roundRobinScheduler(process *, int, char *);
void shortestJobScheduler(process *, int, char *);
void highestPriorityScheduler(process *, int, char *);

/*
 *
 * name: main
 *
 * Receives information from the user of the input and then makes appropriate calls
 *
 */
int main(int argc, char** argv)
{
    printf("Scheduler Comparison.\n\n");

    printf("Name of your input file (%d characters max): ", MAX_FILE_LEN);
    char input[MAX_FILE_LEN];
    scanf("%s", input);

    printf("\nReading file...");
    process jobList[MAX_JOBS];
    int jobCount = readJobs(jobList, input);

    printf("\nScheduling %d jobs:", jobCount);
    fflush(stdout);

    roundRobinScheduler(jobList, jobCount, "roundRobinResults.txt");
    printf("\n\tRound robin complete.");
    fflush(stdout);
    shortestJobScheduler(jobList, jobCount,"shortestJobResults.txt");
    printf("\n\tShortest job complete.");
    fflush(stdout);
    highestPriorityScheduler(jobList, jobCount, "highestPriorityResults.txt");
    printf("\n\tHighest priority complete.");
    fflush(stdout);

    printf("\n\nDone!");

    printf("\n\nThe results from input file \"%s\":", input);
    double roundRobinAverage = calculateWait("roundRobinResults.txt", jobCount);
    double shortestJobAverage = calculateWait("shortestJobResults.txt", jobCount);
    double highestPriorityAverage = calculateWait("highestPriorityResults.txt", jobCount);

    printf("\nRound Robin wait time average: %f", roundRobinAverage);
    printf("\nShortest Job First wait time average: %f", shortestJobAverage);
    printf("\nHighest Priority First wait time average: %f", highestPriorityAverage);

    printf("\n\nThe best scheduler for the data set was ");
    if(roundRobinAverage < shortestJobAverage && roundRobinAverage < highestPriorityAverage){
        printf("Round Robin.");
    }
    else if(shortestJobAverage < roundRobinAverage && shortestJobAverage < highestPriorityAverage){
        printf("Shortest Job First.");
    }
    else if(highestPriorityAverage < shortestJobAverage && highestPriorityAverage < roundRobinAverage){
        printf("Highest Priority First.");
    }
    return 0;
}

/*
 *
 * name: readJobs
 *
 * Function parses the input file given to it and populates the processes array with the values read
 *
 * @param    processes    a process array of processes to be read into
 * @param    input    a character array of the file name to read from
 * @return  number of jobs read
 */
int readJobs(process * processes, char * input){
    FILE* infile;
    int i = 0;
    infile = fopen(input, "r");
    if(infile != NULL){
        for(i=0;i<MAX_JOBS;i++){
            if (fscanf(infile, "%s %d %d %d",
                processes[i].name,
                &processes[i].arrival,
                &processes[i].service,
                &processes[i].priority) < 1){
                break;
            }
            processes[i].timeleft = processes[i].service;
        }
        fclose(infile);
    }
    else{
        printf("Could not open input file!");
        exit(1);
    }
    return i;
}

/*
 *
 * name: calculateWait
 *
 * Calculates the average wait time from the input file given
 * @param    fileName    a character array of the file name
 * @return    the average wait time of the processes
 */

double calculateWait(char * fileName, int jobCount)
{
    int totalWait = 0;
    int tempWait;
    char garbage[1];
    int trash;
    int i;

    FILE * inFile;
    inFile = fopen(fileName, "r");
    if (inFile != NULL){
        for(i = 0; i< jobCount; i++){
            fscanf(inFile, "%s %d %d %d", garbage, &trash, &tempWait, &trash);
            totalWait += tempWait;
        }
        fclose(inFile);
    }

    return ((double)totalWait)/((double)jobCount);
}

/*
 *
 * name: roundRobinScheduler
 *
 * Function simulates the scheduler algorithm on a CPU and prints the results to the file given
 *
 * @param    processes    a process array of processes to be scheduled for work in the CPU
 * @param    output    a character array of the file name to be printed to
 */
void roundRobinScheduler(process * processes, int jobCount, char* output){
    FILE* outfile;
    outfile = fopen(output, "w");
    if(outfile != NULL){
        // make a couple things needed
        process * inCPU = (process *)malloc(sizeof(process));
        queue waiting;

        // initialize everything before starting
        initialize(&waiting);
        inCPU->timeleft = -1;
        int currentJob = 0;
        int clock = -1;
        int quantum = MAX_QUANTUM + 1;
        int CPUfree = TRUE;

        //begin simulation
        while(TRUE){
            quantum--;
            clock++;
            inCPU->timeleft--;
            if(inCPU->timeleft == 0){
                fprintf(outfile, "%s %d %d %d\n",
                    inCPU->name,
                    inCPU->arrival,
                    clock - inCPU->service - inCPU->arrival, // the wait time
                    clock);
                CPUfree = TRUE;
            }
            if(quantum == 0 && !CPUfree){
                push(&waiting, inCPU);
                inCPU = pop(&waiting);
                quantum = MAX_QUANTUM;
            }
            while(processes[currentJob].arrival == clock){
                if(currentJob < jobCount){
                    push( &waiting, &processes[currentJob]);
                    currentJob++;
                }
            }
            if(CPUfree){
                if(isEmpty(&waiting)){
                    if(currentJob == jobCount){
                        break;
                    }
                }
                else{
                    inCPU = pop(&waiting);
                    if(inCPU->timeleft <= 0){
                        inCPU->timeleft = inCPU->service;
                    }
                    quantum = MAX_QUANTUM;
                    CPUfree = FALSE;
                }
            }
        }
        fclose(outfile);
    }
    else{
        printf("Could not open output file!");
        exit(1);
    }
}

/*
 *
 * name: shortestJobScheduler
 *
 * Function simulates the scheduler algorithm on a CPU and prints the results to the file given
 *
 * @param    processes    a process array of processes to be scheduled for work in the CPU
 * @param    output    a character array of the file name to be printed to
 */
void shortestJobScheduler(process * processes, int jobCount, char* output){
    FILE* outfile;
    outfile = fopen(output, "w");
    if(outfile != NULL){
        // make a couple things needed
        process * inCPU = (process *)malloc(sizeof(process));
        queue waiting;

        // initialize everything before starting
        initialize(&waiting);
        inCPU->timeleft = -1;
        int clock = -1;
        int currentJob = 0;
        int CPUfree = TRUE;

        //begin simulation
        while(TRUE){
            clock++;
            inCPU->timeleft--;
            if(inCPU->timeleft == 0){
                fprintf(outfile, "%s %d %d %d\n",
                    inCPU->name,
                    inCPU->arrival,
                    clock - inCPU->service - inCPU->arrival, // the wait time
                    clock);
                CPUfree = TRUE;
            }
            while(processes[currentJob].arrival == clock){
                if(currentJob < jobCount){
                    pushBySerivce( &waiting, &processes[currentJob]);
                    currentJob++;
                }
            }
            if(CPUfree){
                if(isEmpty(&waiting)){
                    if(currentJob == jobCount){ 
                        break;
                    }
                }
                else{
                    inCPU = pop(&waiting);
                    if(inCPU->timeleft <= 0){
                        inCPU->timeleft = inCPU->service;
                    }
                    CPUfree = FALSE;
                }
            }
        }
        fclose(outfile);
    }
    else{
        printf("Could not open output file!");
        exit(1);
    }
}

/*
 *
 * name: highestPriorityScheduler
 *
 * Function simulates the scheduler algorithm on a CPU and prints the results to the file given
 *
 * @param    processes    a process array of processes to be scheduled for work in the CPU
 * @param    output    a character array of the file name to be printed to
 */
void highestPriorityScheduler(process * processes, int jobCount, char* output){
    FILE* outfile;
    outfile = fopen(output, "w");
    if(outfile != NULL){
        // make a couple things needed
        process * inCPU = (process *)malloc(sizeof(process));
        queue waiting;

        // initialize everything before starting
        initialize(&waiting);
        inCPU->timeleft = -1;
        int clock = -1;
        int currentJob = 0;
        int CPUfree = TRUE;

        // begin simulation
        while(TRUE){
            clock++;
            inCPU->timeleft--;
            if(inCPU->timeleft == 0){
                fprintf(outfile, "%s %d %d %d\n",
                    inCPU->name,
                    inCPU->arrival,
                    clock - inCPU->service - inCPU->arrival, // the wait time
                    clock );
                CPUfree = TRUE;
            }
            while(processes[currentJob].arrival == clock){
                if(currentJob < jobCount){
                    pushByPriority(&waiting, &processes[currentJob]);
                    currentJob++;
                }
            }
            if(CPUfree){
                if(isEmpty(&waiting)){
                    if(currentJob == jobCount){
                        break;
                    }
                }
                else{
                    inCPU = pop(&waiting);
                    if(inCPU->timeleft <= 0){
                        inCPU->timeleft = inCPU->service;
                    }
                    CPUfree = FALSE;
                }
            }
        }
        fclose(outfile);
    }
    else{
        printf("Could not open output file!");
        exit(1);
    }
}

