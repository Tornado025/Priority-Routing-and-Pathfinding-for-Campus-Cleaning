#ifndef JOB_H
#define JOB_H

#include "priority_queue.h"

#define EMERGENCY_THRESHOLD 9

void init_routine_jobs(PriorityQueue *pq);
void add_emergency_job(PriorityQueue *pq, int room_id, int priority, char *cause);
void update_job_priorities(PriorityQueue *pq);

#endif
