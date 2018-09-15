#include <stdio.h>
#include <stdlib.h>
#include "event.h"
#include "queue3.h"

int main(int argc, char** argv){
    
    Event* events[10];
    printf("preparing to create and initialize new QUEUE\n");
    Queue* eventQueue = newQueue();
    printf("eventQueue successfully created\n");
    printQueue(eventQueue);
    printf("creating new event\n");
    int i = 0;
    while(i < 10){
        events[i] = newEvent();
        events[i]->time = 10-i;
        pushByLowestTime(eventQueue, events[i]);
        i++;
    }
   
    
    printf("eventsuccesfully created now adding to list");
    printf("\nlist has %d elements. Is empty? %d\n Contents of list:\n", eventQueue->size, isEmpty(eventQueue));
    printQueue(eventQueue);
    
    printf("preparing to pop events\n");
    Event* event1;
    i = 0;
    while(i < 10){
        event1 = pop(eventQueue);
        printf("\nlist has %d elements.  Is empty? %d\n Contents of list:\n", eventQueue->size, isEmpty(eventQueue));
        printQueue(eventQueue);
        i++;
        free(event1);
    }
    
    event1 = newEvent();
    Event* event2 = newEvent();
    pushByLowestTime(eventQueue, event1);
    pushByLowestTime(eventQueue, event2);
     printf("\nlist has %d elements.  Is empty? %d\n Contents of list:\n", eventQueue->size, isEmpty(eventQueue));
    printQueue(eventQueue);
    destroyQueue(eventQueue);
    
    return 0;
}
