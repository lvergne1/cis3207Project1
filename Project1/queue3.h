//
//  queue2.h
//
//
//  Created by Leo Vergnetti on 9/11/18.
//

#ifndef queue3_h
#define queue3_h

#include "queue3.h"
#include "event.h"
#include <stdlib.h>
#include <stdio.h>


typedef struct node{
    struct node* nextNode;
    Event* event;
}Node;

typedef struct queue{
    Node* firstNode;
    Node* lastNode;
    int size;
}Queue;

Node* newNode(Event* event);
void destroyQueue(Queue* targetQueue);
Event* pop(Queue* targetQueue);
Queue* newQueue();
void pushByLowestTime(Queue* targetQueue, Event* event);
void push(Queue* targetQueue, Event* eventToAdd);
int isEmpty(Queue* targetQueue);
void printQueue(Queue*);

#endif /* queue3_h */

