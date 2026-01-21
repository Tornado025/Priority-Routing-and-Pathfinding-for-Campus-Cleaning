#include "context.h"
#include <stdlib.h>

int graph[MAX_NODES][MAX_NODES];
char room_names[MAX_NODES][20];
Worker *worker_hashmap[HASH_SIZE];
int worker_count = 0;

void init_context() {
    worker_count = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        worker_hashmap[i] = NULL;
    }
}
