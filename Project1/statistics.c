
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
int pendingDisk1ResponseTime;
int disk1ResponseTime;

/*Disk2 statistics*/
int disk2IdleTime;
int disk2StartIdle;
int disk2QMaxSize;
float disk2QAvgSize;
int timeOfLastDisk2QChange;
int currentDisk2QSize;
int disk2FinishedJobs;
int pendingDisk2ResponseTime;
int disk2ResponseTime;

FILE *statFile;

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
    pendingDisk1ResponseTime = 0;
    disk1ResponseTime = 0;
    
    disk2IdleTime = 0;
    disk2StartIdle = 0;
    disk2QMaxSize = 0;
    disk2QAvgSize = 0;
    timeOfLastDisk2QChange = 0;
    currentDisk2QSize = 0;
    disk2FinishedJobs = 0;
    pendingDisk2ResponseTime = 0;
    disk2ResponseTime = 0;
    
    totalTime = finalTime;
}

/*******************************************************************************
 *statUpdateDiskIdle first creates a pointer to the diskspecific idle time,
 *and assigns it to disk1 or disk2 specifics depending on which diskNumber called
 *the method.  If the diskState is busy, then it was previously idle, and the
 *idle time is added to the disk time.  If the diskState is not busy, then it is
 *now idle, and so the start idle time is set.
 *******************************************************************************/
void statUpdateDiskIdle(int diskNumber, int diskState, int systemTime){
    int *diskIdleTime = (diskNumber == 1) ? &disk1IdleTime : &disk2IdleTime;
    int *diskStartIdle = (diskNumber == 1) ? &disk1StartIdle : &disk2StartIdle;
    if(diskState == BUSY){
        *diskIdleTime += systemTime - *diskStartIdle;
    }else{
        *diskStartIdle = systemTime;
    }
}

/*******************************************************************************
 *A similar method of pointers is used as above, however in this case the function
 *the average size is computed on the fly. However, it should be noted that this
 *is only possible since finishTime is already set.  The average is based on
 *the size of the queue and the amount of time the queue is spent at this size.
 *As such, the time of the last change is tracked in order to use for the next iteration.
 *******************************************************************************/
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

/*******************************************************************************
 *Method functions almost identically to statUpdateDiskQSize, however, since
 *there is only one processor, pointers are not required to handle or choose the
 *event.
 *******************************************************************************/
void statUpdateCpuQSize(int newSize, int systemTime){
    cpuQAvgSize  += (currentCpuQSize * (systemTime - timeOfLastCpuQChange))/(float)totalTime;
    currentCpuQSize= newSize;
    timeOfLastCpuQChange = systemTime;
    if (newSize > cpuQMaxSize){
        cpuQMaxSize = newSize;
    }
}

/*******************************************************************************
 *Method functions almost identically to statUpdateDiskIdle, however, since
 *there is only one processor, pointers are not required to handle or choose the
 *event.
 *******************************************************************************/
void statUpdateCpuIdle(int cpuState, int systemTime){
    if (cpuState == BUSY){
        cpuIdleTime += (systemTime - cpuStartIdle);
    }
    else{
        cpuStartIdle = systemTime;
    }
}

/*******************************************************************************
 *statUpdateCpuResponseTime recieves the time at which an event entered the cpu
 *for processing, and the projected finish time of that event.  However, although
 *the event time is set, the event does not occur until it is pulled from the
 *event queue and the system time is updated.  As such, this method holds the time
 *difference, but is called by statCpuJobFinisheD()
 *******************************************************************************/
void statUpdateCpuResponseTime(int startTime, int endTime){
    pendingCpuResponseTime = (endTime - startTime);
}
/*******************************************************************************
 *statCpuJobFinished is called when a job finished event is removed from the
 event queue.  The number of finished jobs is incremented, and the pending
 cpu response time is now added to the total cpuResponseTime.
 *******************************************************************************/
void statCpuJobFinished(){
    cpuFinishedJobs += 1;
    cpuResponseTime += pendingCpuResponseTime;
}

/*******************************************************************************
 *Method functions almost identically to statUpdateCpuResponseTime, however, since
 * there are two disks, pointers are required to handle the specific disk based
 on the disk number argument.
 *******************************************************************************/
void statUpdateDiskResponseTime(int diskNumber, int startTime, int endTime){
    int* pendingDiskResponseTime = (diskNumber == 1) ? &pendingDisk1ResponseTime : &pendingDisk2ResponseTime;
    *pendingDiskResponseTime = (endTime - startTime);
}

/*******************************************************************************
 *statDisk1JobFinished is called when a job finished event is removed from the
 *event queue.  This method is similar to the statCpuJobFinished event.
 *The number of finished jobs is incremented, and the pending
 *disk1 response time is now added to the total diskResponseTime.
 *******************************************************************************/
void statDiskJobFinished(int diskNumber){
    int* diskFinishedJobs = (diskNumber == 1) ? &disk1FinishedJobs : &disk2FinishedJobs;
    int* diskResponseTime = (diskNumber == 1) ? &disk1ResponseTime : &disk2ResponseTime;
    int* pendingDiskResponseTime = (diskNumber == 1) ? &pendingDisk1ResponseTime : &pendingDisk2ResponseTime;
    *diskFinishedJobs += 1;
    *diskResponseTime += *pendingDiskResponseTime;
}
/*******************************************************************************
 *statPrintCpuStats writes the final statistics to the statistics.txt file.
 *******************************************************************************/
void statPrintCpuStats(){
    statFile = fopen("statistics.txt", "w");
    fprintf(statFile, "\n**********STATISTICS*********\n");
    fprintf(statFile, "CPU Statistics:\n");
    fprintf(statFile, "The CPU was busy for %%%f of total simulation time\n", (((totalTime - cpuIdleTime)/(float)totalTime)*100));
    fprintf(statFile, "The maximum CPU queue length was %d\n", cpuQMaxSize);
    fprintf(statFile, "The average CPU queue length was %f\n", cpuQAvgSize);
    fprintf(statFile, "The CPU processed %d jobs to completion\n", cpuFinishedJobs);
    fprintf(statFile, "The average CPU response time was %f\n", (float)cpuResponseTime/(float)cpuFinishedJobs);
    fprintf(statFile, "Total CPU throughput was %f Jobs/TimeUnit\n", (cpuFinishedJobs/(float)totalTime));
}

void statPrintDiskStats(){
    fprintf(statFile, "\nDisk 1 Statistics:\n");
    fprintf(statFile, "Disk 1 was idle for %%%f of total simulation time\n", (((totalTime-disk1IdleTime)/(float)totalTime)*100));
    fprintf(statFile, "The maximum Disk 1 queue length was %d\n", disk1QMaxSize);
    fprintf(statFile, "The average Disk 1 queue length was %f\n", disk1QAvgSize);
    fprintf(statFile, "Disk 1 processed %d jobs to completion\n", disk1FinishedJobs);
    fprintf(statFile, "The average Disk 1 response time was %f\n", (float)disk1ResponseTime/(float)disk1FinishedJobs);
    fprintf(statFile, "Total Disk 1 throughput was %f Jobs/TimeUnit\n", (disk1FinishedJobs/(float)totalTime));
    fprintf(statFile, "\nDisk 2 Statistics:\n");
    fprintf(statFile, "Disk 2 was idle for %%%f of total simulation time\n", (((totalTime - disk2IdleTime)/(float)totalTime)*100));
    fprintf(statFile, "The maximum Disk 2 queue length was %d\n", disk2QMaxSize);
    fprintf(statFile, "The average Disk 2 queue length was %f\n", disk2QAvgSize);
    fprintf(statFile, "Disk 2 processed %d jobs to completion\n", disk2FinishedJobs);
    fprintf(statFile, "The average Disk 2 response time was %f\n", (float)disk2ResponseTime/(float)disk2FinishedJobs);
     fprintf(statFile, "Total Disk 2 throughput was %f Jobs/TimeUnit\n", (disk2FinishedJobs/(float)totalTime));
    fclose(statFile);
}
