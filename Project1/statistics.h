//
//  statistics.h
//  
//
//  Created by Leo Vergnetti on 9/12/18.
//

#ifndef statistics_h
#define statistics_h

#include <stdio.h>

void statInit(int finalTime);
void statUpdateCpuQSize(int newSize, int systemTime);
void statUpdateCpuIdle(int cpuState, int systemTime);
void statUpdateDiskIdle(int diskNumber, int diskState, int systemTime);
void statUpdateDiskQSize(int diskNumber, int newSize, int systemTime);
void statCpuJobFinished();
void statDiskJobFinished(int diskNumber);
void statUpdateCpuResponseTime(int startTime, int endTime);
void statPrintCpuStats();
void statPrintDiskStats();
#endif /* statistics_h */
