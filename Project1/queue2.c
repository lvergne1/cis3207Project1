
#include "event.h"
#include <stdlib.h>
#include <stdio.h>
#include "queue2.h"

/*newNode(Event* event) returns a pointer to a node in heap with event set*/
Node* newNode(Event* event){
    Node* node = malloc(sizeof(Node));
    node->event = event;
    return node;
}
/*newQueue() allocates memory for empty Queue, inits the queue, inits size to 0, and returns a pointer */
Queue* newQueue(){
    Queue* newQueue = malloc(sizeof(Queue));
    newQueue->size =  0;
    return newQueue;
}


/*
 **push(Queue*, Event*) is a push method if implementing FIFO Queue (cpuQue, disk_i queue).
 **Push creates a new node to hold event,  then inserts the node in last place by iterating the queue.
 **If queue is empty, then node becomes first element in queue.
 **/
void push(Queue* targetQueue, Event* eventToAdd){
    Node* nodeToAdd = newNode(eventToAdd);
    if(isEmpty(targetQueue)){
        targetQueue->firstNode = nodeToAdd;
    }
    else{
        nodeToAdd->previousNode = targetQueue->lastNode;
        targetQueue->lastNode->nextNode = nodeToAdd;
    }
    targetQueue->lastNode = nodeToAdd;
    targetQueue->size += 1;
}
/**********************************************************************/



/*Is empty returns yes if target Queue is empty*/
int isEmpty(Queue* targetQueue){
    return((targetQueue->size == 0));
}

/*printQueue iterates the queue, printing each element in order*/
void printQueue(Queue* targetQueue){
    if(!isEmpty(targetQueue)){
        Node *currentNode;
        currentNode = targetQueue->firstNode;
        while (currentNode != targetQueue->lastNode){
            printf("\nEvent %d of type %d  is in the queue with time %d\n", currentNode->event->jobNumber, currentNode->event->eventType, currentNode->event->time);
            currentNode = currentNode->nextNode;
        }
        printf("\nEvent %d of type %d  is in the queue with time %d\n", currentNode->event->jobNumber, currentNode->event->eventType, currentNode->event->time);
    }
}

/********************************************************************
 *method to add elements if using priority queue.  In theory
 *all elements could be added this way.
 **********************************************************************/
void pushByLowestTime(Queue* targetQueue, Event* eventToAdd){
    //if list is empty or mytime >= lastTime we can use normal push (add to end/ make new)
    if(isEmpty(targetQueue) || eventToAdd->time >= targetQueue->lastNode->event->time){
        push(targetQueue, eventToAdd);
    } else{ //else my time is first or in the middle
        /*find the right place (could be first)*/
        Node* nodeToAdd= newNode(eventToAdd);
        Node* nodeAfterNewNode = targetQueue->firstNode;

        while(eventToAdd->time >= nodeAfterNewNode->event->time){

            nodeAfterNewNode = nodeAfterNewNode->nextNode;
        }
        /* if the we aren't currently at the first node, we can access the previous node, and therefore update*/
        if(nodeAfterNewNode != targetQueue->firstNode){

            nodeAfterNewNode->previousNode->nextNode = nodeToAdd;
            nodeToAdd->previousNode = nodeAfterNewNode->previousNode;
        
        }
        else{ // we are at the first node
            targetQueue->firstNode = nodeToAdd;
        }
        
        nodeAfterNewNode->previousNode = nodeToAdd;
        nodeToAdd->nextNode = nodeAfterNewNode;
        targetQueue->size += 1;
    }
}

/*pop(targetQueue) removes first element from queue, and returns it to user.
 **removeNode is called on the first node, and memory is deallocated for node **(but not event)**
 **returns NULL if targetQueue is empty.*/
Event* pop(Queue* targetQueue){
    Event* event = NULL;
    Node* nodeToBeRemoved;
    if (!isEmpty(targetQueue)){
        nodeToBeRemoved = targetQueue->firstNode;
        event = nodeToBeRemoved->event;
        if (targetQueue->size > 1){
            targetQueue->firstNode = nodeToBeRemoved->nextNode;
        }
        free(nodeToBeRemoved);
        targetQueue->size -= 1;
    }
    return event;
}


/*destroyQueue(Queue*) removes all nodes and events in queue.
 * functions by popping and freeing memory for elements of the list until the list is empty.*/
void destroyQueue(Queue* targetQueue){
    while(!isEmpty(targetQueue)){
        free(pop(targetQueue));
    }
    free(targetQueue);
}

