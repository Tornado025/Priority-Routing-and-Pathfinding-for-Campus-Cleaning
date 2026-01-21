#include "job.h"
#include <stdio.h>

void init_routine_jobs(PriorityQueue *pq) {
    printf("\n  [SETUP] Initializing routine cleaning schedule...\n");
    printf("  [INFO] Priority-based round robin with workload balancing.\n");
    enqueue(pq, 0, 3, 0, "Routine reception cleaning");
    enqueue(pq, 1, 5, 0, "Routine lobby maintenance");
    enqueue(pq, 2, 5, 0, "Routine office cleaning");
    enqueue(pq, 3, 7, 0, "Routine cafeteria sanitation");
    enqueue(pq, 4, 7, 0, "Routine lab cleaning");
    enqueue(pq, 5, 5, 0, "Routine classroom cleaning");
    printf("  [OK] Routine jobs initialized. These will cycle automatically.\n");
}

void add_emergency_job(PriorityQueue *pq, int room_id, int priority, char *cause) {
    enqueue(pq, room_id, priority, 1, cause);
}

void update_job_priorities(PriorityQueue *pq) {
    for (int i = 0; i < pq->size; i++) {
        Request *req = &pq->data[i];
        if (req->is_emergency)
            continue;
        req->wait_count++;
        int age_boost = req->wait_count / 3;
        req->effective_priority = req->priority + age_boost;
        if (req->effective_priority >= EMERGENCY_THRESHOLD) {
            req->effective_priority = EMERGENCY_THRESHOLD - 1;
        }
    }
    for (int i = pq->size / 2 - 1; i >= 0; i--) {
        int parent = i;
        while (1) {
            int largest = parent;
            int left = 2 * parent + 1;
            int right = 2 * parent + 2;
            if (left < pq->size && is_higher_priority(pq->data[left], pq->data[largest]))
                largest = left;
            if (right < pq->size && is_higher_priority(pq->data[right], pq->data[largest]))
                largest = right;
            if (largest == parent)
                break;
            Request temp = pq->data[parent];
            pq->data[parent] = pq->data[largest];
            pq->data[largest] = temp;
            parent = largest;
        }
    }
}
