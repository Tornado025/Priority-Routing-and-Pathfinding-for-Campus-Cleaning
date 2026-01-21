#include "priority_queue.h"
#include <stdio.h>
#include <string.h>

void swap(Request *a, Request *b) {
    Request temp = *a;
    *a = *b;
    *b = temp;
}

int is_higher_priority(Request a, Request b) {
    if (a.is_emergency != b.is_emergency)
        return a.is_emergency > b.is_emergency;
    if (a.effective_priority != b.effective_priority)
        return a.effective_priority > b.effective_priority;
    return a.arrival_time < b.arrival_time;
}

void initPQ(PriorityQueue *pq) {
    pq->size = 0;
    pq->counter = 0;
}

void enqueue(PriorityQueue *pq, int room_id, int priority, int is_emergency, char *cause) {
    if (pq->size >= MAX_REQUESTS) {
        printf("   [!] Queue Full\n");
        return;
    }
    int i = pq->size;
    pq->data[i].room_id = room_id;
    pq->data[i].priority = priority;
    pq->data[i].effective_priority = priority;
    pq->data[i].is_emergency = is_emergency;
    pq->data[i].arrival_time = ++pq->counter;
    pq->data[i].wait_count = 0;
    strcpy(pq->data[i].cause, cause);
    pq->size++;
    while (i != 0 && is_higher_priority(pq->data[i], pq->data[(i - 1) / 2])) {
        swap(&pq->data[i], &pq->data[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
    if (is_emergency)
        printf("   [!!!] EMERGENCY Added: %s (Priority %d)\n", room_names[room_id], priority);
    else
        printf("   [+] Routine Job Added: %s (Priority %d)\n", room_names[room_id], priority);
}

Request dequeue(PriorityQueue *pq) {
    Request empty = {-1, -1, -1, -1, 0, 0, ""};
    if (pq->size <= 0)
        return empty;
    if (pq->size == 1) {
        pq->size--;
        return pq->data[0];
    }
    Request root = pq->data[0];
    pq->data[0] = pq->data[pq->size - 1];
    pq->size--;
    int i = 0;
    while (1) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        if (left < pq->size && is_higher_priority(pq->data[left], pq->data[largest]))
            largest = left;
        if (right < pq->size && is_higher_priority(pq->data[right], pq->data[largest]))
            largest = right;
        if (largest == i)
            break;
        swap(&pq->data[i], &pq->data[largest]);
        i = largest;
    }
    return root;
}
