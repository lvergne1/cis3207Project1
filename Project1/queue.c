#include "queue.h"
#include "event.h"
#include <stdlib.h>
#include <stdio.h>
/*This is the main file holding the methods and structs for a queue. Queue implements a double-linked list of Event pointers.
 **(NOTE: a single linked list might make for cleaner code).
 ** Priority queue is implemented based on event time using pushBySize instead of push.
 ** Using pop() frees memory allocated by node, but not by event, since user must still use event.
 ** destroyQueue() however frees memory allocated by nodes and events, since destroy is called when simulation is finished
 ***/


/*newQueue() allocates memory for empty Queue, inits the queue, inits size to 0, and returns a pointer */
Queue* newQueue(){
    Queue* newQueue = calloc(1, sizeof(Queue));
    newQueue->size =  0;
    return newQueue;
}

/*
 **push(Queue*, Event*) is a push method if implementing FIFO Queue (cpuQue, disk_i queue).
 **Push creates a new node to hold event,  then inserts the node in last place by iterating the queue.
 **If queue is empty, then node becomes first element in queue.
 **/
void push(Queue* targetQueue, Event* eventToAdd){
    Node* newNode = calloc(1, sizeof(Node));
    newNode->event = eventToAdd;
    if(isEmpty(targetQueue)){
        targetQueue->firstElement = newNode;
    }
    else{
        Node* currentNode = targetQueue->firstElement;
        while(currentNode->nextNode!= NULL){ // while there is next node in the queue
            currentNode = currentNode->nextNode; //go to that node
        }
        
        newNode->previousNode = currentNode; //point the last node and current node to eachother.
        currentNode->nextNode = newNode; // continuing above
    }
    targetQueue->size += 1;
}

/*isEmpty checks size of queue, returns true if size == 0, else false*/
int isEmpty(Queue* targetQueue){
    if(targetQueue->size == 0){
        return 1;
    }
    else {
        return 0;
    }
}

/*print(Queue*) used for debugging purposes. Iterates each node in the queue in order,
 ** printing the jobNumber and eventtime as it does
 **/
void printQueue(Queue* targetQueue){
    Node *currentNode = targetQueue->firstElement;
    while (currentNode != NULL){
        printf("\nEvent %d of type %d  is in the queue with time %d\n", currentNode->event->jobNumber, currentNode->event->eventType, currentNode->event->time);
        currentNode = currentNode->nextNode;
    }
}

/*removeNode called by pop() method. takes a Node and iterates the list until the node is found.  
 ** once found, the node is removed from the list, and memory for node is deallocated
 **NOTE:: memory is not deallocated for event. */
void removeNode(Queue* targetQueue, Node* nodeToBeRemoved){
    if(nodeToBeRemoved !=  targetQueue->firstElement){
        if(nodeToBeRemoved->nextNode != NULL){
            nodeToBeRemoved->previousNode->nextNode = nodeToBeRemoved->nextNode;
            nodeToBeRemoved->nextNode->previousNode = nodeToBeRemoved->previousNode;
        }else {
            nodeToBeRemoved->previousNode->nextNode = NULL;
        }
    }else if(nodeToBeRemoved->nextNode != NULL){
        targetQueue->firstElement = nodeToBeRemoved->nextNode;
    }else{
        targetQueue->firstElement = NULL;
    }
    targetQueue->size -= 1;
    free(nodeToBeRemoved);
}

/*pop(targetQueue) removes first element from queue, and returns it to user. 
 **removeNode is called on the first node, and memory is deallocated for node **(but not event)**
 **returns NULL if targetQueue is empty.*/
Event* pop(Queue* targetQueue){
    Event* event = NULL;
    if (!isEmpty(targetQueue)){
        event = targetQueue->firstElement->event;
        removeNode(targetQueue, targetQueue->firstElement);
    }
    return event;
}

/*destroyQueue(Queue*) removes all nodes and events in queue. 
 * functions by popping and freeing memory for elements of the list until the list is empty.*/
void destroyQueue(Queue* targetQueue){
    while(targetQueue->firstElement != NULL){
        free(pop(targetQueue));
    }
    free(targetQueue);
}

/*pushBySize is the main method used to add events in a priority queue.  When pushBySize
 ** is called on a queue, a node is created, and the list is iterated until eventTime < then
 ** element already in queue.  If two elements have the same event time, then the first element
 ** to arrive will be first in the queue.*/
void pushByLowestTime(Queue* targetQueue, Event* event){
    if(isEmpty(targetQueue)){
        push(targetQueue, event);
    }else{
        /*Allocate memory for the new node to be added to the list*/
        targetQueue->size += 1;
        Node* newNode = calloc(1, sizeof(Node));
        newNode->event = event;
        Node* currentNode = targetQueue->firstElement;
        
        do{
            /*If the timestamp of the event is lower than the timestamp of the node*/
            if(event->time < currentNode->event->time){
                /*If lower than some node that is not the first element, then we point the previous element to the new node.*/
                if(currentNode != targetQueue->firstElement){
                    newNode->previousNode = currentNode->previousNode;
                    currentNode->previousNode->nextNode = newNode;
                }else {
                    /*otherwise we the new node is the new first element in the list*/
                    targetQueue->firstElement = newNode;
                }
                /*In either case, the new node and the current node are made to point to eachother*/
                newNode->nextNode = currentNode;
                currentNode->previousNode = newNode;
                /*we have found the proper location, and may stop iterating the queue*/
                break;
            }
            /*Check to make sure the next node may be accessed, and if so, adjust to the next node*/
            if(currentNode->nextNode!= NULL){
                currentNode = currentNode->nextNode;
            }
        }while(currentNode->nextNode!=NULL);
        /*We have now exited the queue.  If new node's pointers are null, new node has not found a place in the list, and so it must have largest or secondlargest timestamp*/
        if(newNode->nextNode == NULL && newNode->previousNode== NULL){
            
            /*If it is the second largest, add it before*/
            if(event->time < currentNode->event->time){
                newNode->nextNode = currentNode;
                newNode->previousNode = currentNode->previousNode;
                currentNode->previousNode->nextNode = newNode;
                currentNode->previousNode = newNode;
            } else{ /*else it is the largest*/
                currentNode->nextNode = newNode;
                newNode->previousNode = currentNode;
            }
        }
    }
}
