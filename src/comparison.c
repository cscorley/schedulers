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

#define MAX_QUANTUM 10

// maximums for generation
#define MAX_PRIORITY 20
#define MAX_ARRIVAL 250
#define MAX_SERVICE 250
#define MAX_JOBS 100


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
    int argTestCount;

    // get the passed options from the user
    // options c and n are required to run.
    while ((opt = getopt(argc, argv, "hg:i:")) != -1) {
        switch (opt) {
            case 'h':
                printf("Usage: %s (-i input_file | -g number)\n\n", argv[0]);
                printf("Options:\n\t-i\tInput file for testing");
                printf("\n\t-g\tGenerate the given number of random tests");
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
                argTestCount = atoi(optarg);
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

    if (iFlag){
        run(argFileName);
    }

    if (gFlag){
        generate(argTestCount);
    }

    // might as well?
    printf("\n\n");
    exit(EXIT_SUCCESS);
}

void generate(int testCount){
    int i;
    int j;
    FILE* outfile;
    char * test = "generatedTest";

    unsigned int genJobs;
    unsigned int genArrival;
    unsigned int genService;
    unsigned int genPriority;

    // set random seed
    unsigned int iseed = (unsigned int)time(NULL);
    srand (iseed);

    for (i=0; i < testCount; i++){
        outfile = fopen(test, "w");
        // generate file
        if (outfile != NULL){
            genJobs = rand() % MAX_JOBS;
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
}

void run(char * input){
    int jobCount;
    printf("Running Scheduler Comparison.\n\n");

    printf("Input file: %s", input);

    printf("\nReading file...");
    queue jobList;
    initialize(&jobList);
    readJobs(&jobList, input);
    jobCount = length(&jobList);

    printf("\nScheduling %d jobs\n", jobCount);

    printf("esfrwef");
    fflush(stdout);
    generalScheduler(copy(&jobList),"shortestJobResults.txt", &service_less_func);
    printf("esfrwef");
    fflush(stdout);
    generalScheduler(copy(&jobList),"highestPriorityResults.txt", &priority_more_func);
    printf("esfrwef");
    fflush(stdout);
    roundRobinScheduler(copy(&jobList), "roundRobinResults.txt");
    printf("esfrwef");
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
void roundRobinScheduler(queue * processes, char* output){
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
        int quantum = MAX_QUANTUM + 1;
        bool CPUfree = true;

        //begin simulation
        while(true){
            quantum--;
            clock++;
            inCPU->timeleft--;
            if(inCPU->timeleft == 0){
//                printf("\ntime %d",clock);
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
                quantum = MAX_QUANTUM;
            }
            while(nextJob != NULL) { //&& nextJob->arrival <= clock){
                if(nextJob->arrival <= clock){
//                    printf("\nnext job: %d\ntime %d",nextJob->service,clock);
                    nextJob->timeleft = nextJob->service;
                    push(&waiting, nextJob);
                    nextJob = pop(processes);
                }
                else{
                    break;
                }
            }
            if(CPUfree){
                if(isEmpty(&waiting)){
                    if(isEmpty(processes)){
                        break;
                    }
                }
                else{
                    inCPU = pop(&waiting);
//                    if(inCPU->timeleft <= 0){
//                        inCPU->timeleft = inCPU->service;
//                    }
                    quantum = MAX_QUANTUM;
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
void generalScheduler(queue * processes, char* output, bool(*comp)(process *, process *)){
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
            while(nextJob != NULL){
                if(nextJob->arrival <= clock){
                    nextJob->timeleft = nextJob->service;
                    pushOrdered(&waiting, nextJob, comp);
                    nextJob = pop(processes);
                }
                else{
                    break;
                }
            }
            if(CPUfree){
                if(isEmpty(&waiting)){
                    if(isEmpty(processes)){
                        break;
                    }
                }
                else{
                    inCPU = pop(&waiting);
//                    if(inCPU->timeleft <= 0){
//                        inCPU->timeleft = inCPU->service;
//                    }
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
