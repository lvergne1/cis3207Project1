//
//  queue2.h
//  
//
//  Created by Leo Vergnetti on 9/11/18.
//

#ifndef queue2_h
#define queue2_h

#include <stdio.h>

typedef struct node{
    struct node* nextNode;
    struct node* previousNode;
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

#endif /* queue2_h */
