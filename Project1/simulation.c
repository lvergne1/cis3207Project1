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

FILE* configFile;
FILE* logFile;

void initSystemValues(){
    configFile = fopen("config.txt", "r");
    initTime = readConfigValues();
    finTime = readConfigValues();
    arriveMin = readConfigValues();
    arriveMax = readConfigValues();
    quitProb = readConfigValues();
    cpuMin = readConfigValues();
    cpuMax = readConfigValues();
    disk1Min = readConfigValues();
    disk1Max = readConfigValues();
    disk2Min = readConfigValues();
    disk2Max = readConfigValues();
    fclose(configFile);
    
    logFile = fopen("log.txt", "w");
    fprintf(logFile, "FILE CONTAINING CONFIG VALUES AND EVENTLOG\n");
    fprintf(logFile, "init time %d\n", initTime);
    fprintf(logFile, "finish time %d\n", finTime);
    fprintf(logFile, "arrival minimum %d\n", arriveMin);
    fprintf(logFile, "arrival maximum %d\n", arriveMax);
    fprintf(logFile, "quit probability %d\n", quitProb);
    fprintf(logFile, "cpu minimum %d\n", cpuMin);
    fprintf(logFile, "cpu maximum %d\n", cpuMax);
    fprintf(logFile, "disk 1 minimum %d\n", disk1Min);
    fprintf(logFile, "disk 1 maximum %d\n", disk1Max);
    fprintf(logFile, "disk 2 minimum %d\n", disk2Min);
    fprintf(logFile, "disk 2 maximum %d\n", disk2Max);
   
    systemTime = initTime;
    cpuStatus = !BUSY;
    disk1Status = !BUSY;
    disk2Status = !BUSY;
    
    cpuQueue = newQueue();
    disk1Queue = newQueue();
    disk2Queue = newQueue();
    eventQueue = newQueue();
    
    
}
/*****************************************************************************
 *readConfigValues is a method that reads each value in the config file, and returns
 *that value as an int.  File is opened and subsequently closed outside of the
 *method, and it is the responsibility of the caller to open and close the
 configFile.
 *****************************************************************************/
int readConfigValues(){;
    char buff[255];
    int value;
    fscanf(configFile, "%s", buff);
    fscanf(configFile, "%d", &value);
    return value;
}

/*****************************************************************************
 *lRndm(int min, int max) uses arc4random to return a random value between
 *min and max, including the boundaries. So lRndm(1,2) returns 1 or 2 randomly.
 *lrnd is autoseeded, and so no seed value argument is required.
 *****************************************************************************/
int lRndm(int min, int max){
    return (min+arc4random_uniform(max-min+1));
}
/*****************************************************************************
 *addNextJob() creates and adds the next event to the event queue.  First, a new
 *job is allocated on the heap and a pointer is returned (via the newEvent method.
 *Then the time is set to be a random number between arrieMin and max( using lrndm),
 *and then this event is added to the queue
 *****************************************************************************/
void addNextJob(){
    Event* newJob = newEvent();
    newJob->time = systemTime + lRndm(arriveMin, arriveMax);
    pushByLowestTime(eventQueue, newJob);
}

/*****************************************************************************
 *When endSimulation is called, each of the associated datatypes is destroyed.
 *Since all allocated heap storage must by construction be in this list
 *(except previously removed events such as terminated jobs, and the last event)
 *this method is responsible for all cleanup by calling destroyQueue.
 *****************************************************************************/
void endSimulation(){
    destroyQueue(cpuQueue);
    destroyQueue(disk1Queue);
    destroyQueue(disk2Queue);
    destroyQueue(eventQueue);
    fclose(logFile);
}
/*****************************************************************************
 *When an event lands here, the CPU is either BUSY or not.  If it is not BUSY,
 *the event is processed by generating a random process time (using lRndm) and
 *adding this time to systemTime and pushing this value into the eventQueue
 *by priority. All statistiscs are informed via stat methods.
 *****************************************************************************/
void processCPUArrival(Event* jobToBeProcessed){
    if(cpuStatus != BUSY){
        fprintf(logFile, "\nTime: %d :: job %d is being processed", systemTime, jobToBeProcessed->jobNumber);
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

/*****************************************************************************
 *When a CPU_FINISH event is read from the event queue, a job has finished
 *processing.  As such, the status of the CPU is changed to not busy.  Statistics
 *associated with the cpu are updated, and if the cpuQueue is not empty,
 *the next job is popped from the list, and a processing time is set.  The
 *processed job is either sent to disk for further io, or is set to exit the
 *system, upon which memory associated with teh event is free.
 *****************************************************************************/
void processCPUFinish(Event* finishedJob){
    cpuStatus = !BUSY;
    statCpuJobFinished();
    statUpdateCpuIdle(cpuStatus, systemTime);
    if(!isEmpty(cpuQueue)){
        processCPUArrival(pop(cpuQueue));
        statUpdateCpuQSize(cpuQueue->size, systemTime);
    }
    if(lRndm(1,100) <= quitProb){
        fprintf(logFile, "\nTime: %d :: job %d is exiting the system", systemTime, finishedJob->jobNumber);
        free(finishedJob);
    }else{
        finishedJob->eventType = DISK_ARRIVAL;
        pushByLowestTime(eventQueue, finishedJob);
    }
}

/*****************************************************************************
 *This method processes the arrival of a job to the disk.
 *First, pointers are established to point to disk 1 or disk 2 (using pointers) depending
 *on which event is retrieved (identical code for each).
 *First, the disk is set to busy and statistics are updated. Then either a new time and event
 *is set for the disk to finish, or the event is added to the queue for the event.  If added
 *to the queue, statistics are updated again.
 *****************************************************************************/
void processDiskArrival(Event* jobToUseDisk){
    Queue* diskQueue = disk1Queue;
    int* diskStatus = &disk1Status;
    int diskNumber = 1;
    int eventChoice = DISK1_FINISH;
    int jobArrivalTime;
    if ((disk1Queue->size == disk2Queue->size && lRndm(0,1)) || (disk2Queue->size < disk1Queue->size)){
        diskQueue = disk2Queue;
        diskStatus = &disk2Status;
        diskNumber = 2;
        eventChoice = DISK2_FINISH;
    }
    if(*diskStatus != BUSY){
        *diskStatus = BUSY;
        statUpdateDiskIdle(diskNumber, *diskStatus, systemTime);
        jobArrivalTime = jobToUseDisk->time;
        jobToUseDisk->time = systemTime + lRndm(disk1Min, disk1Max);
        jobToUseDisk->eventType = eventChoice;
        statUpdateDiskResponseTime(diskNumber, jobArrivalTime, jobToUseDisk->time);
        pushByLowestTime(eventQueue, jobToUseDisk);
    }else{
        push(diskQueue, jobToUseDisk);
        statUpdateDiskQSize(diskNumber, diskQueue->size, systemTime);
    }
}

/*****************************************************************************
 *If sent here, event has already recieved disk-specific processing.  The job
 *is sent to the cpu via the event queue
 *****************************************************************************/
void processDiskFinish(Event* jobFinishedWithDisk){
    jobFinishedWithDisk->eventType = CPU_ARRIVAL;
    pushByLowestTime(eventQueue, jobFinishedWithDisk);
}

/*****************************************************************************
 *Disk1 has finshed read/write.  First the status of the disk is set to not
 *BUSY, and statistics are then updated.  If something is in the disk1 queue,
 *disk1 is set to process this event.  In either case, job is sent to
 *processDiskFinish for more final phases.
 *****************************************************************************/
void processDisk1Finish(Event* jobFinishedWithDisk1){
    disk1Status = !BUSY;
    statDiskJobFinished(1);
    statUpdateDiskIdle(1, disk1Status, systemTime);
    if(!isEmpty(disk1Queue)){
        processDiskArrival(pop(disk1Queue));
        statUpdateDiskQSize(1, disk1Queue->size, systemTime);
    }
    processDiskFinish(jobFinishedWithDisk1);
}

/*****************************************************************************
 *Disk 2 has finshed read/write.  First the status of the disk is set to not
 *BUSY, and statistics are then updated.  If something is in the disk 2 queue,
 *disk 2 is set to process this event.  In either case, job is sent to
 *processDiskFinish for more final phases.
 *****************************************************************************/
void processDisk2Finish(Event* jobFinishedWithDisk2){
    disk2Status = !BUSY;
    statDiskJobFinished(2);
    statUpdateDiskIdle(2, disk2Status, systemTime);
    if(!isEmpty(disk2Queue)){
        processDiskArrival(pop(disk2Queue));
        statUpdateDiskQSize(2, disk2Queue->size, systemTime);
    }
    processDiskFinish(jobFinishedWithDisk2);
}

/*****************************************************************************
 *A new job has been added to the system. As such, the next event is added to the
 *system, and fallthrough is used (in the switch method handleEvent) to immediately
 *processed.
 *****************************************************************************/
void processJobArrival(Event* newJob){
    //create event for the next job to enter the system.
    addNextJob();
    //send new job to processor
    newJob->eventType = CPU_ARRIVAL;
}

/*****************************************************************************
 *The handleEvent method switches on the eventType of the event, and determines
 *what to do based on the event.  If the event is a system arrival, then the
 *next event is added to the system, and the fall through proceeds to send the
 *job to the processor.  In addition, the log is updated accordingly to coincide
 *with the called event. If the event is the simulation finished event, then
 *the memory associated with the event is freed.
 *****************************************************************************/
void handleEvent(Event* currentEvent){
    switch (currentEvent->eventType){
        /*When a new job enters the system, the next job is created and added to the eventqueue
         *Then this job is sent to the cpu*/
        case SYSTEM_ARRIVAL:
            processJobArrival(currentEvent);
            //use fallthru
        case CPU_ARRIVAL:
            fprintf(logFile, "\nTime: %d ::job %d arrived at CPU :: queue length: %d", systemTime, currentEvent->jobNumber, cpuQueue->size);
            processCPUArrival(currentEvent);
            break;
        case CPU_FINISH:
            fprintf(logFile, "\nTime: %d ::job %d finished at CPU", systemTime, currentEvent->jobNumber);
            processCPUFinish(currentEvent);
            break;
        case DISK_ARRIVAL:
            fprintf(logFile, "\nTime: %d ::job %d arrived at Disk", systemTime, currentEvent->jobNumber);
            processDiskArrival(currentEvent);
            break;
        case DISK1_FINISH :
            fprintf(logFile, "\nTime: %d :: job %d finished at Disk 1", systemTime, currentEvent->jobNumber);
            processDisk1Finish(currentEvent);
            break;
        case DISK2_FINISH :
            fprintf(logFile, "\nTime: %d :: job %d finished with Disk 2", systemTime, currentEvent->jobNumber);
            processDisk2Finish(currentEvent);
            break;
        case SIMULATION_END :
            fprintf(logFile, "\nTime %d :: Finish event read from queue, simulation terminating\n", systemTime);
            free(currentEvent);
            break;
    }
}

/*****************************************************************************
 *runSimulation() is the main loop in which the simulation runs.  When called
 *first the system values are initialized, both for the simulation and for the
 *statistics engine.  Then the last and first jobs are created and added to the
 *event queue. Next, the while loop begins, which pops the next event from the
 *queue.  The system time is adjusted, and the handleEvent method is called with
 *the event.  The loop terminates when the finish event is read, and the systemTime
 *is updated to the finish time.  Then endSimulation() is called which handles
 *the deallocation of memory, and the statistics are written to the log file.
 *****************************************************************************/
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
