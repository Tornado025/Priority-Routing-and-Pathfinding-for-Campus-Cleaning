#ifndef TYPES_H
#define TYPES_H

typedef struct Worker {
    int worker_id;
    int current_room_id;
    char name[20];
    int is_occupied;
    int is_busy;
    int jobs_completed;
    int skill_type;
    struct Worker *next;
} Worker;

typedef struct {
    int room_id;
    int priority;
    int effective_priority;
    int arrival_time;
    int wait_count;
    int is_emergency;
    int skill_required;
    char cause[100];
} Request;

#endif
