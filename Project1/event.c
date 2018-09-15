
#include "event.h"
#include <stdlib.h>

int jobFactory = 0;

Event* newEvent(){
  Event *newEvent = malloc(sizeof(Event));
  newEvent->jobNumber = jobFactory;
  newEvent->eventType = SYSTEM_ARRIVAL;
  jobFactory += 1;
  return newEvent;
}
