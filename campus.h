#ifndef CAMPUS_H
#define CAMPUS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_NODES 30
#define INF INT_MAX
#define MAX_REQUESTS 50
#define EMERGENCY_THRESHOLD 9
#define MAX_WORKERS 10
#define HASH_SIZE 10

typedef struct Worker {
    int worker_id;
    int current_room_id;
    char name[20];
    int is_occupied;
    int is_busy;
    int jobs_completed;
    struct Worker *next;
} Worker;

typedef struct{
    int room_id;
    int priority;
    int effective_priority;
    int arrival_time;
    int wait_count;
    int is_emergency;
    char cause[100];
}Request;

typedef struct {
    Request data[MAX_REQUESTS];
    int size;
    int counter;      
} PriorityQueue;

extern int graph[MAX_NODES][MAX_NODES];
extern char room_names[MAX_NODES][20];
extern Worker* worker_hashmap[HASH_SIZE];
extern int worker_count;

void init_system();

void initPQ(PriorityQueue *pq);
void enqueue(PriorityQueue *pq, int room_id, int priority, int is_emergency, char* cause);
Request dequeue(PriorityQueue *pq);

void init_worker_hashmap();
int hash_function(int worker_id);
void add_worker(int id, char* name, int start_room);
Worker* get_worker(int worker_id);
int find_best_available_worker(int target_room);
void mark_worker_busy(int worker_id, int busy);
void get_all_workers(Worker** workers_array, int* count);

void init_routine_jobs(PriorityQueue *pq);
void add_emergency_job(PriorityQueue *pq, int room_id, int priority, char* cause);
void update_job_priorities(PriorityQueue *pq);

void dijkstra(int start_node, int end_node);

#endif
