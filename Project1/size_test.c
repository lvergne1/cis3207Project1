#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "event.h"

int main(){
  Queue* queue = newQueue();
  printf("Queue is Empty: %d", isEmpty(queue));
  Event* event = newEvent();
  push(queue, event);
  printf("QueueisEmpty: %d", isEmpty(queue));
  destroyQueue(queue);
  return 0;
}
