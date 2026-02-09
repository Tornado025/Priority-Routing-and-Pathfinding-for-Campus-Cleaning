#include "job.h"
#include <stdio.h>

void init_routine_jobs(PriorityQueue *pq) {
    printf("\n  [SETUP] Initializing routine cleaning schedule...\n");
    printf("  [INFO] Priority-based round robin with workload balancing.\n");
    enqueue(pq, 0, 3, 0, 1, "Routine Kotak Lab cleaning");
    enqueue(pq, 1, 5, 0, 1, "Routine CV Block cleaning");
    enqueue(pq, 2, 5, 0, 1, "Routine EC Block cleaning");
    enqueue(pq, 3, 5, 0, 1, "Routine EE Block cleaning");
    enqueue(pq, 4, 4, 0, 9, "Routine Admin Block cleaning");
    enqueue(pq, 5, 5, 0, 1, "Routine CS Block cleaning");
    enqueue(pq, 6, 3, 0, 1, "Routine Ground maintenance");
    enqueue(pq, 7, 5, 0, 1, "Routine ET Block cleaning");
    enqueue(pq, 8, 5, 0, 3, "Routine Chem Block cleaning");
    enqueue(pq, 9, 6, 0, 5, "Routine Canteen sanitation");
    enqueue(pq, 10, 5, 0, 1, "Routine BT Quadrangle cleaning");
    enqueue(pq, 11, 5, 0, 1, "Routine Mech Block cleaning");
    enqueue(pq, 12, 5, 0, 1, "Routine IEM Block cleaning");
    printf("  [OK] Routine jobs initialized. These will cycle automatically.\n");
}

void add_emergency_job(PriorityQueue *pq, int room_id, int priority, int skill_required, char *cause) {
    enqueue(pq, room_id, priority, 1, skill_required, cause);
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
