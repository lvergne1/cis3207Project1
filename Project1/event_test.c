#include <stdio.h>
#include <stdlib.h>
#include "event.h"
#include "queue.h"

int main(){

  Event *event1 = newEvent();
  printf("\n allocated memory for first event");
  Event *event2 = newEvent();
  printf("\n two events successfully created and allocated");
  Queue* cpuQueue = newQueue();
  push(cpuQueue, event1);
  printf("Size of cpuQueue: %d\n", cpuQueue->size);
    push(cpuQueue, event2);
  printf("\nSize of cpuQueue: %d\n", cpuQueue->size);
  print(cpuQueue);
  Event *event1copy = pop(cpuQueue);
  printf("\nJob %d removed from the queue\n", event1copy->jobNumber);
  print(cpuQueue);
  Event *event2copy = pop(cpuQueue);
  printf("\nJob %d removed from the queue\n", event2copy->jobNumber);
  
  int i = 0;
  Event* events[10];
  while(i < 10){
    events[i] = newEvent();
    push(cpuQueue, events[i]);
    i += 1;
  }
  print(cpuQueue);
  
  

  destroyQueue(cpuQueue);
  free(event1copy);
  free(event2);
  return 0;
}
