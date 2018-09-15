
#include "statistics.h"
#include <stdio.h>
#define BUSY 1

/*CPU statistics*/
int cpuIdleTime;
int totalTime;
int cpuStartIdle;
int cpuQMaxSize;
int cpuFinishedJobs;
float cpuQAvgSize;
int timeOfLastCpuQChange;
int currentCpuQSize;
int pendingCpuResponseTime;
long cpuResponseTime;

/*Disk1 statistics*/
int disk1IdleTime;
int disk1StartIdle;
int disk1QMaxSize;
float disk1QAvgSize;
int timeOfLastDisk1QChange;
int currentDisk1QSize;
int disk1FinishedJobs;

/*Disk2 statistics*/
int disk2IdleTime;
int disk2StartIdle;
int disk2QMaxSize;
float disk2QAvgSize;
int timeOfLastDisk2QChange;
int currentDisk2QSize;
int disk2FinishedJobs;

void statInit(int finalTime){
    cpuIdleTime = 0;
    cpuStartIdle = 0;
    cpuQMaxSize= 0;
    cpuFinishedJobs = 0;
    cpuQAvgSize = 0;
    timeOfLastCpuQChange = 0;
    currentCpuQSize = 0;
    pendingCpuResponseTime = 0;
    cpuResponseTime = 0;
    
    disk1IdleTime = 0;
    disk1StartIdle = 0;
    disk1QMaxSize = 0;
    disk1QAvgSize = 0;
    timeOfLastDisk1QChange = 0;
    currentDisk1QSize = 0;
    disk1FinishedJobs = 0;
    
    
    disk2IdleTime = 0;
    disk2StartIdle = 0;
    disk2QMaxSize = 0;
    disk2QAvgSize = 0;
    timeOfLastDisk2QChange = 0;
    currentDisk2QSize = 0;
    disk2FinishedJobs = 0;
    
    totalTime = finalTime;
}

void statUpdateDiskIdle(int diskNumber, int diskState, int systemTime){
    int *diskIdleTime = (diskNumber == 1) ? &disk1IdleTime : &disk2IdleTime;
    int *diskStartIdle = (diskNumber == 1) ? &disk1StartIdle : &disk2StartIdle;
    if(diskState == BUSY){
        *diskIdleTime += systemTime - *diskStartIdle;
    }else{
        *diskStartIdle = systemTime;
    }
}

void statUpdateDiskQSize(int diskNumber, int newSize, int systemTime){
    int *diskQMaxSize = (diskNumber == 1) ? &disk1QMaxSize : &disk2QMaxSize;
    int *timeOfLastDiskQChange = (diskNumber == 1) ? &timeOfLastDisk1QChange : &timeOfLastDisk2QChange;
    float *diskQAvgSize = (diskNumber == 1) ? &disk1QAvgSize : &disk2QAvgSize;
    int *currentDiskQSize = (diskNumber == 1) ? &currentDisk1QSize : &currentDisk2QSize;
    
    *diskQAvgSize += (*currentDiskQSize * (systemTime - *timeOfLastDiskQChange))/(float)totalTime;
    *currentDiskQSize = newSize;
    *timeOfLastDiskQChange = systemTime;
    *diskQMaxSize = (newSize > *diskQMaxSize) ? newSize : *diskQMaxSize;
}

void statUpdateCpuQSize(int newSize, int systemTime){
    cpuQAvgSize  += (currentCpuQSize * (systemTime - timeOfLastCpuQChange))/(float)totalTime;
    currentCpuQSize= newSize;
    timeOfLastCpuQChange = systemTime;
    if (newSize > cpuQMaxSize){
        cpuQMaxSize = newSize;
    }
}

void statUpdateCpuIdle(int cpuState, int systemTime){
    if (cpuState == BUSY){
        cpuIdleTime += (systemTime - cpuStartIdle);
    }
    else{
        cpuStartIdle = systemTime;
    }
}

void statUpdateCpuResponseTime(int startTime, int endTime){
    pendingCpuResponseTime = (endTime - startTime);
}

void statCpuJobFinished(){
    cpuFinishedJobs += 1;
    cpuResponseTime += pendingCpuResponseTime;
}

void statDiskJobFinished(int diskNumber){
    int* diskFinishedJobs = (diskNumber == 1) ? &disk1FinishedJobs : &disk2FinishedJobs;
    *diskFinishedJobs += 1;
}

void statPrintCpuStats(){
    printf("\n**********STATISTICS*********\n");
    printf("CPU Statistics:\n");
    printf("The CPU was busy for %%%f of total simulation time\n", (((totalTime - cpuIdleTime)/(float)totalTime)*100));
    printf("The maximum CPU queue length was %d\n", cpuQMaxSize);
    printf("The average CPU queue length was %f\n", cpuQAvgSize);
    printf("The CPU processed %d jobs to completion\n", cpuFinishedJobs);
    printf("The average CPU response time was %f\n", (float)cpuResponseTime/(float)cpuFinishedJobs);
    printf("Total throughput was %f Jobs/TimeUnit\n", (cpuFinishedJobs/(float)totalTime));
}

void statPrintDiskStats(){
    printf("\nDisk 1 Statistics:\n");
    printf("Disk 1 was idle for %%%f of total simulation time\n", (((totalTime-disk1IdleTime)/(float)totalTime)*100));
    printf("The maximum Disk 1 queue length was %d\n", disk1QMaxSize);
    printf("The average Disk 1 queue length was %f\n", disk1QAvgSize);
    printf("Disk 1 processed %d jobs to completion\n", disk1FinishedJobs);

    printf("\nDisk 2 Statistics:\n");
    printf("Disk 2 was idle for %%%f of total simulation time\n", (((totalTime - disk2IdleTime)/(float)totalTime)*100));
    printf("The maximum Disk 2 queue length was %d\n", disk2QMaxSize);
    printf("The average Disk 1 queue length was %f\n", disk2QAvgSize);
    printf("Disk 2 processed %d jobs to completion\n", disk2FinishedJobs);
}
