#ifndef _QUEUE_H_
#define _QUEUE_H_
#include "event.h"
#include "queue.h"

typedef struct node{
  struct node* nextNode;
  struct node* previousNode;
  Event* event; 
}Node;

typedef struct queue{
  Node* firstElement;  
  int size;
}Queue;

void destroyQueue(Queue* targetQueue);
Event* pop(Queue* targetQueue);
void removeNode(Queue* targetQueue, Node* nodeToBeRemoved);
Queue* newQueue();
void pushByLowestTime(Queue* targetQueue, Event* event);
void push(Queue* targetQueue, Event* eventToAdd);
int isEmpty(Queue* targetQueue);
void printQueue(Queue*);
#endif
