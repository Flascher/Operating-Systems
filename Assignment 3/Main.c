#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

typedef struct {
    int arrivalTime;
    int burstTime;
    int turnaround;
    double normTurnaround;
    int latency;
    int startTime;
    int finishTime;
    int priority;
    int name;
    int amountDone;
    int done;
} Process;

typedef struct {
    double fifoTurnaround;
    double fifoNormTurnaround;
    double fifoLatency;
    double fifoUtilization;
    double sjfTurnaround;
    double sjfNormTurnaround;
    double sjfLatency;
    double sjfUtilization;
    double rrTurnaround;
    double rrNormTurnaround;
    double rrLatency;
    double rrUtilization;
    double mfqTurnaround;
    double mfqNormTurnaround;
    double mfqLatency;
    double mfqUtilization;
} Results;

Results r;

int numProcesses;

Process* generateProcesses(int num);
int generateRandomIntInRange(int min, int max);
int processesAreDone(Process *processes);
void runFifo(Process *processes);
void runSjf(Process *processes);
void runRoundRobin(Process *processes);
void runMfq(Process *processes);
Process getProcessOfPriority(Process *processes, int priority);
int compareArrivalTime(Process *s1, Process *s2);
int compareBurstTime(Process *s1, Process *s2);
void printProcessInfo(Process p);

int main(int argc, char *argv[]) {
    if(argc == 1) {
        numProcesses = 50;
    }
    else if(argv[1] != NULL){
        numProcesses = atoi(argv[1]);
        if(numProcesses > 100) {
            numProcesses = 100;
        }
    }

    Process* processes = generateProcesses(numProcesses);

    // int i;
    // for(i = 0; i < numProcesses; i++) {
    //     Process p = processes[i];
    //     printProcessInfo(p);
    // }

    runFifo(processes);
    printf("===== finished FIFO =====\n\n");
    runSjf(processes);
    printf("===== finished SJF =====\n\n");
    runRoundRobin(processes);
    printf("===== finished Round Robin =====\n\n");
    runMfq(processes);
    printf("===== finished MFQ =====\n\n");
}

Process* generateProcesses(int num) {
    Process *processes = malloc(sizeof(Process) * num);

    int i;
    for(i = 0; i < num; i++) {
        Process p;
        p.arrivalTime = generateRandomIntInRange(0, 10);
        p.burstTime = generateRandomIntInRange(3, 25);
        p.name = i;
        p.done = 0;
        p.startTime = 0;
        p.finishTime = 0;
        p.priority = 1;
        p.turnaround = 0;
        p.normTurnaround = 0;
        p.latency = 0;
        p.amountDone = 0;

        processes[i] = p;
    }

    return processes;
}

int generateRandomIntInRange(int min, int max) {
    int x = rand() % (max + 1 - min) + min;

    return x;
}

int processesAreDone(Process *processes) {
    int finished = TRUE;

    int i;
    for(i = 0; i < numProcesses; i++) {
        if(!processes[i].done) {
            finished = FALSE;
            break;
        }
    }

    // printf("processes are done: %d\n", finished);
    return finished;
}

void runFifo(Process *processes) {
    int currentTime = 0;
    int busyTime = 0;
    Process *fifoProcesses = malloc(sizeof(processes));

    int i;
    for(i = 0; i < numProcesses; i++) {
        fifoProcesses[i] = processes[i];
    }

    qsort(fifoProcesses, numProcesses, sizeof(Process), (int (*) (const void *, const void*)) compareArrivalTime);

    // run processes
    for(i = 0; i < numProcesses; i++) {
        Process p = fifoProcesses[i];
        p.startTime = currentTime;
        p.latency = currentTime - p.arrivalTime;
        if(p.latency < 0)
            p.latency = 0;

        while(currentTime <= (p.burstTime + p.startTime)) {
            // process hasn't arrived yet
            if(p.arrivalTime >= currentTime) {
                currentTime++;
            }
            // process is running
            else {
                currentTime++;
                busyTime++;
            }
        }

        // printProcessInfo(p);
        // printf("currentTime: %d\nbusyTime: %d\n", currentTime, busyTime);

        p.done = TRUE;
        p.finishTime = currentTime;
        p.turnaround = p.latency + p.burstTime;
        p.normTurnaround = (double) (p.turnaround / p.burstTime);

        // printProcessInfo(p);

        fifoProcesses[i] = p;
    }

    double avgTurnaroundTime = 0;
    double avgNormTurnaroundTime = 0;
    double avgLatency = 0;
    double utilization = (double) busyTime / (double) currentTime;

    // calculate avg values
    for(i = 0; i < numProcesses; i++) {
        Process p = fifoProcesses[i];

        avgTurnaroundTime += p.turnaround;
        avgNormTurnaroundTime += p.normTurnaround;
        avgLatency += p.latency;
    }

    avgTurnaroundTime /= numProcesses;
    avgNormTurnaroundTime /= numProcesses;
    avgLatency /= numProcesses;

    r.fifoTurnaround = avgTurnaroundTime;
    r.fifoNormTurnaround = avgNormTurnaroundTime;
    r.fifoLatency = avgLatency;
    r.fifoUtilization = utilization;

    printf("----- FIFO stats -----\n");
    printf("Average Turnaround: %f\n", avgTurnaroundTime);
    printf("Average Normalized Turnaround: %f\n", avgNormTurnaroundTime);
    printf("Average Latency: %f\n", avgLatency);
    printf("Utilization: %f\n", utilization);
}

void runSjf(Process *processes) {
    int currentTime = 0;
    int busyTime = 0;
    Process *sjfProcesses = malloc(sizeof(processes));

    int i;
    for(i = 0; i < numProcesses; i++) {
        sjfProcesses[i] = processes[i];
    }

    qsort(sjfProcesses, numProcesses, sizeof(Process), (int (*) (const void *, const void *)) compareBurstTime);

    // run processes
    for(i = 0; i < numProcesses; i++) {
        Process p = sjfProcesses[i];
        p.startTime = currentTime;
        p.latency = currentTime - p.arrivalTime;
        if(p.latency < 0)
            p.latency = 0;

        while(currentTime <= (p.burstTime + p.startTime)) {
            // process hasn't arrived yet
            if(p.arrivalTime >= currentTime) {
                currentTime++;
            }
            // process is running
            else {
                currentTime++;
                busyTime++;
            }
        }

        // printProcessInfo(p);
        // printf("currentTime: %d\nbusyTime: %d\n", currentTime, busyTime);

        p.done = TRUE;
        p.finishTime = currentTime;
        p.turnaround = p.latency + p.burstTime;
        p.normTurnaround = (double) (p.turnaround / p.burstTime);

        // printProcessInfo(p);

        sjfProcesses[i] = p;
    }

    double avgTurnaroundTime = 0;
    double avgNormTurnaroundTime = 0;
    double avgLatency = 0;
    double utilization = (double) busyTime / (double) currentTime;

    // calculate avg values
    for(i = 0; i < numProcesses; i++) {
        Process p = sjfProcesses[i];

        avgTurnaroundTime += p.turnaround;
        avgNormTurnaroundTime += p.normTurnaround;
        avgLatency += p.latency;
    }

    avgTurnaroundTime /= numProcesses;
    avgNormTurnaroundTime /= numProcesses;
    avgLatency /= numProcesses;

    r.sjfTurnaround = avgTurnaroundTime;
    r.sjfNormTurnaround = avgNormTurnaroundTime;
    r.sjfLatency = avgLatency;
    r.sjfUtilization = utilization;

    printf("----- SJF stats -----\n");
    printf("Average Turnaround: %f\n", avgTurnaroundTime);
    printf("Average Normalized Turnaround: %f\n", avgNormTurnaroundTime);
    printf("Average Latency: %f\n", avgLatency);
    printf("Utilization: %f\n", utilization);
}

void runRoundRobin(Process *processes) {
    int currentTime = 0;
    int busyTime = 0;
    int quantum = generateRandomIntInRange(1, 10);
    int currentQuantum = 0;
    Process *rrProcesses = malloc(sizeof(Process) * numProcesses);

    int i;
    for(i = 0; i < numProcesses; i++) {
        rrProcesses[i] = processes[i];
    }

    // run processes
    for(i = 0; !processesAreDone(rrProcesses); i++) {
        Process p = rrProcesses[i % numProcesses];
        if(i <= numProcesses) {
            p.startTime = currentTime;
            p.latency = currentTime - p.arrivalTime;
            if(p.latency < 0)
                p.latency = 0;
        }

        while(!p.done || currentQuantum < quantum) {
            // check to see if current process has finished
            if(p.amountDone >= p.burstTime) {
                p.done = TRUE;
                p.finishTime = currentTime;
                p.turnaround = p.latency + p.burstTime;
                p.normTurnaround = (double) (p.turnaround / p.burstTime);

                rrProcesses[p.name] = p;
            }

            // process hasn't arrived yet
            if(p.arrivalTime >= currentTime) {
                currentTime++;
                break;
            }
            // process is running
            else {
                currentTime++;
                busyTime++;
                p.amountDone++;
            }
            currentQuantum++;
        }

        currentQuantum = 0;

        // printProcessInfo(p);
        // printf("currentTime: %d\nbusyTime: %d\n", currentTime, busyTime);
    }

    double avgTurnaroundTime = 0;
    double avgNormTurnaroundTime = 0;
    double avgLatency = 0;
    double utilization = (double) busyTime / (double) currentTime;

    for(i = 0; i < numProcesses; i++) {
        Process p = rrProcesses[i];

        avgTurnaroundTime += p.turnaround;
        avgNormTurnaroundTime += p.normTurnaround;
        avgLatency += p.latency;
    }

    r.rrTurnaround = avgTurnaroundTime;
    r.rrNormTurnaround = avgNormTurnaroundTime;
    r.rrLatency = avgLatency;
    r.rrUtilization = utilization;

    printf("----- Round Robin stats -----\n");
    printf("Average Turnaround: %f\n", avgTurnaroundTime);
    printf("Average Normalized Turnaround: %f\n", avgNormTurnaroundTime);
    printf("Average Latency: %f\n", avgLatency);
    printf("Utilization: %f\n", utilization);
}

void runMfq(Process *processes) {
    int currentTime = 0;
    int busyTime = 0;
    int prio1Quantum = 2;
    int prio2Quantum = 4;
    int prio3Quantum = 8;
    int prio4Quantum = 16;
    int randomResetTime = generateRandomIntInRange(100, 500);
    int currentQuantum = 0;
    Process *mfqProcesses = malloc(sizeof(Process) * numProcesses);

    int i;
    for(i = 0; i < numProcesses; i++) {
        mfqProcesses[i] = processes[i];
    }

    // run processes
    for(i = 0; !processesAreDone(mfqProcesses); i++) {
        Process p = mfqProcesses[i % numProcesses];
        if(i <= numProcesses) {
            p.startTime = currentTime;
            p.latency = currentTime - p.arrivalTime;
            if(p.latency < 0)
                p.latency = 0;
        }

        // eventually reset all processes to priority 1 to avoid starvation
        if(currentTime >= randomResetTime) {
            int j;
            for(j = 0; j < numProcesses; j++) {
                mfqProcesses[i].priority = 1;
            }
        }

        if(p.priority == 1) {
            while(!p.done || currentQuantum < prio1Quantum) {
                // check to see if current process has finished
                if(p.amountDone >= p.burstTime) {
                    p.done = TRUE;
                    p.finishTime = currentTime;
                    p.turnaround = p.latency + p.burstTime;
                    p.normTurnaround = (double) (p.turnaround / p.burstTime);

                    mfqProcesses[p.name] = p;
                }

                // process hasn't arrived yet
                if(p.arrivalTime >= currentTime) {
                    currentTime++;
                    break; // go to next process since it hasn't arrived
                }
                // process is running
                else {
                    currentTime++;
                    busyTime++;
                    p.amountDone++;
                }
                currentQuantum++;
            }

            // if process hasn't finished in prio 1, bump to next prio
            if(!p.done) {
                p.priority = 2;
            }

            currentQuantum = 0;
        }
        else if(p.priority == 2) {
            while(!p.done || currentQuantum < prio2Quantum) {
                // check to see if current process has finished
                if(p.amountDone >= p.burstTime) {
                    p.done = TRUE;
                    p.finishTime = currentTime;
                    p.turnaround = p.latency + p.burstTime;
                    p.normTurnaround = (double) (p.turnaround / p.burstTime);

                    mfqProcesses[p.name] = p;
                }

                // process hasn't arrived yet
                if(p.arrivalTime >= currentTime) {
                    break; // go to next process since it hasn't arrived
                }
                // process is running
                else {
                    currentTime++;
                    busyTime++;
                    p.amountDone++;
                }
                currentQuantum++;
            }

            // if process hasn't finished in prio 1, bump to next prio
            if(!p.done) {
                p.priority = 3;
            }

            currentQuantum = 0;
        }
        else if(p.priority == 3) {
            while(!p.done || currentQuantum < prio3Quantum) {
                // check to see if current process has finished
                if(p.amountDone >= p.burstTime) {
                    p.done = TRUE;
                    p.finishTime = currentTime;
                    p.turnaround = p.latency + p.burstTime;
                    p.normTurnaround = (double) (p.turnaround / p.burstTime);

                    mfqProcesses[p.name] = p;
                }

                // process hasn't arrived yet
                if(p.arrivalTime >= currentTime) {
                    currentTime++;
                    break; // go to next process since it hasn't arrived
                }
                // process is running
                else {
                    currentTime++;
                    busyTime++;
                    p.amountDone++;
                }
                currentQuantum++;
            }

            // if process hasn't finished in prio 1, bump to next prio
            if(!p.done) {
                p.priority = 4;
            }

            currentQuantum = 0;
        }
        else {
            while(!p.done || currentQuantum < prio4Quantum) {
                // check to see if current process has finished
                if(p.amountDone >= p.burstTime) {
                    p.done = TRUE;
                    p.finishTime = currentTime;
                    p.turnaround = p.latency + p.burstTime;
                    p.normTurnaround = (double) (p.turnaround / p.burstTime);

                    mfqProcesses[p.name] = p;
                }

                // process hasn't arrived yet
                if(p.arrivalTime >= currentTime) {
                    currentTime++;
                    break; // go to next process since it hasn't arrived
                }
                // process is running
                else {
                    currentTime++;
                    busyTime++;
                    p.amountDone++;
                }
                currentQuantum++;
            }

            currentQuantum = 0;
        }
    }

    double avgTurnaroundTime = 0;
    double avgNormTurnaroundTime = 0;
    double avgLatency = 0;
    double utilization = (double) busyTime / (double) currentTime;

    for(i = 0; i < numProcesses; i++) {
        Process p = mfqProcesses[i];

        avgTurnaroundTime += p.turnaround;
        avgNormTurnaroundTime += p.normTurnaround;
        avgLatency += p.latency;
    }

    r.mfqTurnaround = avgTurnaroundTime;
    r.mfqNormTurnaround = avgNormTurnaroundTime;
    r.mfqLatency = avgLatency;
    r.mfqUtilization = utilization;

    printf("----- MFQ stats -----\n");
    printf("Average Turnaround: %f\n", avgTurnaroundTime);
    printf("Average Normalized Turnaround: %f\n", avgNormTurnaroundTime);
    printf("Average Latency: %f\n", avgLatency);
    printf("Utilization: %f\n", utilization);
}

Process getProcessOfPriority(Process *processes, int priority) {

}

int compareArrivalTime(Process *s1, Process *s2) {
    if(s1->arrivalTime < s2->arrivalTime)
        return -1;
    else if(s1->arrivalTime > s2->arrivalTime)
        return 1;
    else return 0;
}

int compareBurstTime(Process *s1, Process *s2) {
    if(s1->burstTime < s2->burstTime)
        return -1;
    else if(s1->burstTime > s2->burstTime)
        return 1;
    else return 0;
}

// for debugging purposes
void printProcessInfo(Process p) {
    printf("arrivalTime: %d\n", p.arrivalTime);
    printf("burstTime: %d\n", p.burstTime);
    printf("turnaround: %d\n", p.turnaround);
    printf("normTurnaround: %f\n", p.normTurnaround);
    printf("amountDone: %d\n", p.amountDone);
    printf("latency: %d\n", p.latency);
    printf("finishTime: %d\n", p.finishTime);
    printf("name: %d\n", p.name);
    printf("done: %d\n\n", p.done);
}