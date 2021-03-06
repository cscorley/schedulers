/*
 *      comparison.c
 *
 * This is the main file for the scheduling algorithm simulations.  It will
 * get the input file location from the user and run the scheduler function upon that
 * set of read data for each algorithm.
 *
 */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "process.h"
#include "queue.h"
#include "comparison.h"

// default values
#define DEF_QUANTUM 10

// maximums for generation
#define MAX_PRIORITY 20
#define MAX_ARRIVAL 250
#define MAX_SERVICE 100
#define MAX_JOBS 100
#define MIN_JOBS 10


bool arrival_less_func(process * a, process * b){
    return (a->arrival < b->arrival);
}

bool service_less_func(process * a, process * b){
    return (a->service < b->service);
}

bool priority_more_func(process * a, process * b){
    return (a->priority > b->priority);
}

/*
 *
 * name: main
 *
 * Receives information from the user of the input and then makes appropriate calls
 *
 */
int main(int argc, char** argv)
{
    bool iFlag = false;
    bool gFlag = false;
    int opt;
    char * argFileName;
    int quantum = DEF_QUANTUM;

    // options c and n are required to run.
    while ((opt = getopt(argc, argv, "hq:g:i:")) != -1) {
        switch (opt) {
            case 'h':
                printf("Usage: %s (-i input_file | -g output_file) [-q quantum]\n\n", argv[0]);
                printf("Options:\n\t-i\tInput file for testing");
                printf("\n\t-g\tGenerate random test to this file and run it");
                printf("\n\t-q\tUse this value as the quantum in the round\
robin scheduler. Default is %d", DEF_QUANTUM);
                printf("\n\n\t-h\tDisplays this help message\n");                
                exit(EXIT_SUCCESS);        
            case 'i':
                iFlag = true;
                argFileName = (char *)malloc(strlen(optarg));
                if(argFileName == NULL){
                    printf("Can't get memory!");
                    exit(EXIT_FAILURE);
                }
                strcpy(argFileName, optarg);
                break;
            case 'g':
                gFlag = true;
                argFileName = (char *)malloc(strlen(optarg));
                if(argFileName == NULL){
                    printf("Can't get memory!");
                    exit(EXIT_FAILURE);
                }
                strcpy(argFileName, optarg);
                break;
            case 'q':
                quantum = atoi(optarg);
                if (quantum < 1){
                    quantum = DEF_QUANTUM;
                }
                break;
            default: /* '?' */
                fprintf(stderr, "Usage: %s [-i input_file] [-g number]\n\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    if (!iFlag && !gFlag){
        printf("Expected at least one option, either -i or -g. See help (%s -h)\n\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (iFlag && gFlag){
        printf("Expected only one option, either -i or -g. See help (%s -h)\n\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    if (iFlag){
        run(argFileName, quantum);
    }
    else if (gFlag){
        generate(argFileName);
        run(argFileName, quantum);
    }

    // might as well?
    printf("\n\n");
    exit(EXIT_SUCCESS);
}

void generate(char * test){
    int j;
    FILE* outfile;

    unsigned int genJobs;
    unsigned int genArrival;
    unsigned int genService;
    unsigned int genPriority;

    // set random seed
    unsigned int iseed = (unsigned int)time(NULL);
    srand (iseed);

    outfile = fopen(test, "w");
    // generate file
    if (outfile != NULL){
        genJobs = rand() % (MAX_JOBS - MIN_JOBS + 1) + MIN_JOBS;
        for (j=0; j < genJobs; j++){
            genArrival = rand () % MAX_ARRIVAL;
            genService = rand () % MAX_SERVICE;
            genPriority = rand () % MAX_PRIORITY;

            fprintf(outfile, "GENJOB_%d %d %d %d\n",
                   j, genArrival, genService, genPriority); 
        }

        fclose(outfile);
    }
    else{
        printf("could not open output file for generation");
        exit(1);
    }
    // run tests on file?
}

void run(char * input, int quantum){
    int jobCount;
    char output[strlen(input) + 36];
    printf("Running Scheduler Comparison.\n\n");

    printf("Input file: %s", input);

    printf("\nReading file...");
    queue jobList;
    initialize(&jobList);
    readJobs(&jobList, input);
    jobCount = length(&jobList);

    printf("\nScheduling %d jobs\n", jobCount);

    strcpy(output, input);
    strcat(output, "_shortestJobResults.txt");
    generalScheduler(copy(&jobList), output, false, &service_less_func);
    double shortestJobAverage = calculateWait(output, jobCount);

    strcpy(output, input);
    strcat(output, "_shortestRemainingResults.txt");
    generalScheduler(copy(&jobList), output, true, &service_less_func);
    double shortestRJobAverage = calculateWait(output, jobCount);

    strcpy(output, input);
    strcat(output, "_highestPriorityResults.txt");
    generalScheduler(copy(&jobList), output, false, &priority_more_func);
    double highestPriorityAverage = calculateWait(output, jobCount);

    strcpy(output, input);
    strcat(output, "_highestPriorityPreemptResults.txt");
    generalScheduler(copy(&jobList), output, true, &priority_more_func);
    double highestPriorityPAverage = calculateWait(output, jobCount);

    strcpy(output, input);
    strcat(output, "_roundRobinResults.txt");
    roundRobinScheduler(copy(&jobList), output, quantum);
    double roundRobinAverage = calculateWait(output, jobCount);
    printf("\n\nDone!");

    printf("\n\nThe results from input file \"%s\":", input);

    printf("\nRound Robin wait time average: %f", roundRobinAverage);
    printf("\nShortest Job First wait time average: %f", shortestJobAverage);
    printf("\nShortest Remaining Time First wait time average: %f", shortestRJobAverage);
    printf("\nHighest Priority First wait time average: %f", highestPriorityAverage);
    printf("\nHighest Priority First Preemptive wait time average: %f", highestPriorityPAverage);

    printf("\n\nThe best scheduler(s) for the data set was ");
    if(roundRobinAverage <= shortestJobAverage 
        && roundRobinAverage <= shortestRJobAverage 
        && roundRobinAverage <= highestPriorityAverage
        && roundRobinAverage <= highestPriorityPAverage){
        printf("\n\t\t\tRound Robin");
    }
    if(shortestJobAverage <= roundRobinAverage 
            && shortestJobAverage <= shortestRJobAverage 
            && shortestJobAverage <= highestPriorityAverage
            && shortestJobAverage <= highestPriorityPAverage){
        printf("\n\t\t\tShortest Job First");
    }
    if(shortestRJobAverage <= roundRobinAverage 
            && shortestRJobAverage <= shortestJobAverage 
            && shortestRJobAverage <= highestPriorityAverage
            && shortestRJobAverage <= highestPriorityPAverage){
        printf("\n\t\t\tShortest Remaining Time First");
    }
    if(highestPriorityAverage <= shortestJobAverage 
            && highestPriorityAverage <= shortestRJobAverage 
            && highestPriorityAverage <= highestPriorityPAverage 
            && highestPriorityAverage <= roundRobinAverage){
        printf("\n\t\t\tHighest Priority First");
    }
    if(highestPriorityPAverage <= shortestJobAverage 
            && highestPriorityPAverage <= shortestRJobAverage 
            && highestPriorityPAverage <= highestPriorityAverage 
            && highestPriorityPAverage <= roundRobinAverage){
        printf("\n\t\t\tHighest Priority First (Premptive)");
    }

}

/*
 *
 * name: readJobs
 *
 * Function parses the input file given to it and populates the processes array with the values read
 *
 * @param    processes    a process array of processes to be read into
 * @param    input    a character array of the file name to read from
 */
void readJobs(queue * processes, char * input){
    FILE* infile;
    process * p = (process *)malloc(sizeof(process));
    if(p == NULL){
        printf("out of memory");
        exit(1);
    }
    infile = fopen(input, "r");
    if(infile != NULL){
        while(fscanf(infile, "%s %d %d %d",
                p->name,
                &p->arrival,
                &p->service,
                &p->priority) == 4){
            p->timeleft = p->service;
            pushOrdered(processes, p, &arrival_less_func);
            p = (process *)malloc(sizeof(process));
            if(p == NULL){
                printf("out of memory");
                exit(1);
            }
        }
        fclose(infile);
    }
    else{
        printf("Could not open input file!\n");
        exit(1);
    }
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
    char garbage[32];
    int trash;

    FILE * inFile;
    inFile = fopen(fileName, "r");
    if (inFile != NULL){
        while(fscanf(inFile, "%s %d %d %d", 
                    garbage, &trash, &tempWait, &trash) == 4){
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
void roundRobinScheduler(queue * processes, char* output, int user_quantum){
    FILE* outfile;
    outfile = fopen(output, "w");
    if(outfile != NULL){
        // make a couple things needed
        process * inCPU = (process *)malloc(sizeof(process));
        process * nextJob;
        queue waiting;

        if (!isEmpty(processes)){
            nextJob = pop(processes);
        }
        else{
            printf("No jobs?");
            exit(1);
        }
            
        // initialize everything before starting
        initialize(&waiting);
        inCPU->timeleft = -1;
        int clock = -1;
        int quantum = user_quantum + 1; // set one higher for first loop
        bool CPUfree = true;

        //begin simulation
        while(true){
            quantum--;
            clock++;
            inCPU->timeleft--;
            if(inCPU->timeleft == 0){
                fprintf(outfile, "%s %d %d %d\n",
                    inCPU->name,
                    inCPU->arrival,
                    clock - inCPU->service - inCPU->arrival, // the wait time
                    clock);
                CPUfree = true;
            }
            if(quantum == 0 && !CPUfree){
                push(&waiting, inCPU);
                inCPU = pop(&waiting);
                quantum = user_quantum;
            }
            while(nextJob != NULL && nextJob->arrival <= clock){
                nextJob->timeleft = nextJob->service;
                push(&waiting, nextJob);
                nextJob = pop(processes);
            }
            if(CPUfree){
                if(isEmpty(&waiting)){
                    if(isEmpty(processes)){
                        break;
                    }
                }
                else{
                    inCPU = pop(&waiting);
                    quantum = user_quantum;
                    CPUfree = false;
                }
            }
        }
        fclose(outfile);
    }
    else{
        printf("Could not open output file!\n");
        exit(1);
    }
}

/*
 *
 * name: generalJobScheduler
 *
 * Function simulates the scheduler algorithm on a CPU and prints the results to the file given
 *
 * @param    processes    a process array of processes to be scheduled for work in the CPU
 * @param    output    a character array of the file name to be printed to
 */
void generalScheduler(queue * processes, char* output, bool preemptive, bool(*comp)(process *, process *)){
    FILE* outfile;
    outfile = fopen(output, "w");
    if(outfile != NULL){
        // make a couple things needed
        process * inCPU = (process *)malloc(sizeof(process));
        process * nextJob;
        queue waiting;

        if (!isEmpty(processes)){
            nextJob = pop(processes);
        }
        else{
            printf("No jobs?");
            exit(1);
        }


        // initialize everything before starting
        initialize(&waiting);
        inCPU->timeleft = -1;
        int clock = -1;
        bool CPUfree = true;
        bool addedProc = false;

        //begin simulation
        while(true){
            clock++;
            inCPU->timeleft--;
            if(inCPU->timeleft == 0){
                fprintf(outfile, "%s %d %d %d\n",
                    inCPU->name,
                    inCPU->arrival,
                    clock - inCPU->service - inCPU->arrival, // the wait time
                    clock);
                CPUfree = true;
            }
            while(nextJob != NULL && nextJob->arrival <= clock){
                    nextJob->timeleft = nextJob->service;
                    pushOrdered(&waiting, nextJob, comp);
                    nextJob = pop(processes);
                    addedProc = true;
            }

            if (preemptive && addedProc && inCPU->timeleft > 0) {
                if (comp(top(&waiting),inCPU) == 1 ){
                    pushOrdered(&waiting, inCPU, comp);
                    CPUfree = true;
                }
            }
            addedProc = false;
            if(CPUfree){
                if(isEmpty(&waiting)){
                    if(isEmpty(processes)){
                        break;
                    }
                }
                else{
                    inCPU = pop(&waiting);
                    CPUfree = false;
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
