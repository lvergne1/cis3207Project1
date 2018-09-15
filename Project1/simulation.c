#include "event.h"
#include "queue3.h"
#include "simulation.h"
#include "statistics.h"
#include <stdlib.h>
#include <stdio.h>

#define BUSY 1

int initTime;
int finTime;
int arriveMin;
int arriveMax;
int quitProb;
int cpuMin;
int cpuMax;
int disk1Min;
int disk1Max;
int disk2Min;
int disk2Max;

int systemTime;
int cpuStatus;
int disk1Status;
int disk2Status;

Queue* cpuQueue;
Queue* disk1Queue;
Queue* disk2Queue;
Queue* eventQueue;

void initSystemValues(){
    initTime = 0;
    finTime = 100000;
    arriveMin = 50;
    arriveMax = 150;
    quitProb = 2;
    cpuMin = 50;
    cpuMax = 100;
    disk1Min = 250;
    disk1Max = 400;
    disk2Min = 250;
    disk2Max = 400;
    
    systemTime = initTime;
    cpuStatus = !BUSY;
    disk1Status = !BUSY;
    disk2Status = !BUSY;
    
    cpuQueue = newQueue();
    disk1Queue = newQueue();
    disk2Queue = newQueue();
    eventQueue = newQueue();
}

/* lRndm(int min, int max) uses arc4random to return a random value between min and max, including the boundaries. So lRndm(1,2) returns 1 or 2 randomly.****/
int lRndm(int min, int max){
    return (min+arc4random_uniform(max-min+1));
}

void addNextJob(){
    Event* newJob = newEvent();
    newJob->time = systemTime + lRndm(arriveMin, arriveMax);
    pushByLowestTime(eventQueue, newJob);
}

void endSimulation(){
    destroyQueue(cpuQueue);
    destroyQueue(disk1Queue);
    destroyQueue(disk2Queue);
    destroyQueue(eventQueue);
}

void processCPUArrival(Event* jobToBeProcessed){
    if(cpuStatus != BUSY){
        //printf("\nTime: %d :: job %d is being processed", systemTime, jobToBeProcessed->jobNumber);
        cpuStatus = BUSY;
        statUpdateCpuIdle(cpuStatus, systemTime);
        int jobArrivalTime = jobToBeProcessed->time;
        jobToBeProcessed->time = systemTime + lRndm(cpuMin, cpuMax);
        jobToBeProcessed->eventType = CPU_FINISH;
        statUpdateCpuResponseTime(jobArrivalTime, jobToBeProcessed->time);
        pushByLowestTime(eventQueue, jobToBeProcessed);
    } else{
        push(cpuQueue, jobToBeProcessed);
        statUpdateCpuQSize(cpuQueue->size, systemTime);
    }
}

void processCPUFinish(Event* finishedJob){
    cpuStatus = !BUSY;
    statCpuJobFinished();
    statUpdateCpuIdle(cpuStatus, systemTime);
    if(!isEmpty(cpuQueue)){
        processCPUArrival(pop(cpuQueue));
        statUpdateCpuQSize(cpuQueue->size, systemTime);
    }
    if(3 == lRndm(1,5)){
        //printf("\nTime: %d :: job %d is exiting the system", systemTime, finishedJob->jobNumber);
        free(finishedJob);
    }else{
        finishedJob->eventType = DISK_ARRIVAL;
        pushByLowestTime(eventQueue, finishedJob);
    }
}

void processDiskArrival(Event* jobToUseDisk){
    Queue* diskQueue = disk1Queue;
    int* diskStatus = &disk1Status;
    int diskNumber = 1;
    int eventChoice = DISK1_FINISH;
    if ((disk1Queue->size == disk2Queue->size && lRndm(0,1)) || (disk2Queue->size < disk1Queue->size)){
        diskQueue = disk2Queue;
        diskStatus = &disk2Status;
        diskNumber = 2;
        eventChoice = DISK2_FINISH;
    }
    if(*diskStatus != BUSY){
        *diskStatus = BUSY;
        statUpdateDiskIdle(diskNumber, *diskStatus, systemTime);
        jobToUseDisk->time = systemTime + lRndm(disk1Min, disk1Max);
        jobToUseDisk->eventType = eventChoice;
        pushByLowestTime(eventQueue, jobToUseDisk);
    }else{
        push(diskQueue, jobToUseDisk);
        statUpdateDiskQSize(diskNumber, diskQueue->size, systemTime);
    }
}

void processDiskFinish(Event* jobFinishedWithDisk){
    jobFinishedWithDisk->eventType = CPU_ARRIVAL;
    pushByLowestTime(eventQueue, jobFinishedWithDisk);
}

void processDisk1Finish(Event* jobFinishedWithDisk1){
    disk1Status = !BUSY;
    statUpdateDiskIdle(1, disk1Status, systemTime);
    if(!isEmpty(disk1Queue)){
        processDiskArrival(pop(disk1Queue));
        statUpdateDiskQSize(1, disk1Queue->size, systemTime);
        statDiskJobFinished(1);
    }
    processDiskFinish(jobFinishedWithDisk1);
}

void processDisk2Finish(Event* jobFinishedWithDisk2){
    disk2Status = !BUSY;
    statUpdateDiskIdle(2, disk2Status, systemTime);
    if(!isEmpty(disk2Queue)){
        processDiskArrival(pop(disk2Queue));
        statUpdateDiskQSize(2, disk2Queue->size, systemTime);
        statDiskJobFinished(2);
    }
    processDiskFinish(jobFinishedWithDisk2);
}

void processJobArrival(Event* newJob){
    //create event for the next job to enter the system.
    addNextJob();
    //send new job to processor
    newJob->eventType = CPU_ARRIVAL;
}

void handleEvent(Event* currentEvent){
    switch (currentEvent->eventType){
        /*When a new job enters the system, the next job is created and added to the eventqueue
         *Then this job is sent to the cpu*/
        case SYSTEM_ARRIVAL:
            processJobArrival(currentEvent);
            //use fallthru
        case CPU_ARRIVAL:
            //printf("\nTime: %d ::job %d arrived at CPU :: queue length: %d", systemTime, currentEvent->jobNumber, cpuQueue->size);
            processCPUArrival(currentEvent);
            break;
        case CPU_FINISH:
           // printf("\nTime: %d ::job %d finished at CPU", systemTime, currentEvent->jobNumber);
            processCPUFinish(currentEvent);
            break;
        case DISK_ARRIVAL:
            //printf("\nTime: %d ::job %d arrived at Disk", systemTime, currentEvent->jobNumber);
            processDiskArrival(currentEvent);
            break;
        case DISK1_FINISH :
            //printf("\nTime: %d :: job %d finished at Disk 1", systemTime, currentEvent->jobNumber);
            processDisk1Finish(currentEvent);
            break;
        case DISK2_FINISH :
            //printf("\nTime: %d :: job %d finished with Disk 2", systemTime, currentEvent->jobNumber);
            processDisk2Finish(currentEvent);
            break;
        case SIMULATION_END :
            //printf("\nTime %d :: Finish event read from queue, simulation terminating\n", systemTime);
            free(currentEvent);
            break;
    }
}
void runSimulation(){
    
    initSystemValues();
    statInit(finTime);
    
    /*Create last event and add it to the eventQueue at time finTime*/
    Event* lastJob = newEvent();
    lastJob->time = finTime;
    lastJob->eventType = SIMULATION_END;
    pushByLowestTime(eventQueue, lastJob);
    
    /*Create first event and add it to the eventqueue at time 0*/
    Event* job1 = newEvent();
    job1->time = 0;
    pushByLowestTime(eventQueue, job1);
    while(!isEmpty(eventQueue) && systemTime < finTime){
        Event* nextEvent = pop(eventQueue);
        systemTime = nextEvent->time;
        handleEvent(nextEvent);
    }
    endSimulation();
    statPrintCpuStats();
    statPrintDiskStats();
}
