#ifndef _SIMULATION_H_
#define _SIMULATION_H_
#include <stdlib.h>
#include "event.h"
#include "queue3.h"

void addNextJob();
int lRndm(int min, int max);
void initSystemValues();
void runSimulation();
void endSimulation();
void handleEvent(Event* event);
void processCPUArrival(Event* jobToBeProcessed);
void processCPUFinish(Event* finishedJob);
void processDiskArrival(Event* jobToUseDisk);
void processDiskFinish(Event* jobFinishedWithDisk);
void processDisk1Finish(Event* jobFinishedWithDisk1);
void processDisk2Finish(Event* jobFinishedWithDisk2);
int readConfigValues();
#endif 
