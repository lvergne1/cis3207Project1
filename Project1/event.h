#ifndef _EVENT_H_
#define _EVENT_H_

#define SYSTEM_ARRIVAL 1
#define CPU_ARRIVAL 2
#define CPU_FINISH 3
#define DISK_ARRIVAL 4
#define DISK1_FINISH 5
#define DISK2_FINISH 6
#define SIMULATION_END 7

typedef struct Event{
  int jobNumber;
  int eventType;
  int time;
}Event;

Event* newEvent();

#endif
