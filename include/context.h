#ifndef CONTEXT_H
#define CONTEXT_H

#define MAX_NODES 30
#define MAX_WORKERS 10
#define HASH_SIZE 10

typedef struct Worker Worker;

extern int graph[MAX_NODES][MAX_NODES];
extern char room_names[MAX_NODES][20];
extern Worker *worker_hashmap[HASH_SIZE];
extern int worker_count;

void init_context();

#endif
