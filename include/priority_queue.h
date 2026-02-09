#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "types.h"
#include "context.h"

#define MAX_REQUESTS 50

typedef struct {
    Request data[MAX_REQUESTS];
    int size;
    int counter;
} PriorityQueue;

void initPQ(PriorityQueue *pq);
void enqueue(PriorityQueue *pq, int room_id, int priority, int is_emergency, int skill_required, char *cause);
Request dequeue(PriorityQueue *pq);
void swap(Request *a, Request *b);
int is_higher_priority(Request a, Request b);

#endif
