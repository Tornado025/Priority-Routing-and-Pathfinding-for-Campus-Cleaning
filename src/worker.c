#include "worker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define INF INT_MAX

int hash_function(int worker_id) {
    return worker_id % HASH_SIZE;
}

void init_worker_hashmap() {
    for (int i = 0; i < HASH_SIZE; i++) {
        worker_hashmap[i] = NULL;
    }
}

Worker *get_worker(int worker_id) {
    int index = hash_function(worker_id);
    Worker *current = worker_hashmap[index];
    while (current != NULL) {
        if (current->worker_id == worker_id) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void get_all_workers(Worker **workers_array, int *count) {
    *count = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        Worker *current = worker_hashmap[i];
        while (current != NULL) {
            workers_array[*count] = current;
            (*count)++;
            current = current->next;
        }
    }
}

void add_worker(int id, char *name, int start_room) {
    if (worker_count >= MAX_WORKERS) {
        printf("   [!] Maximum workers reached.\n");
        return;
    }
    Worker *new_worker = (Worker *)malloc(sizeof(Worker));
    new_worker->worker_id = id;
    new_worker->current_room_id = start_room;
    strcpy(new_worker->name, name);
    new_worker->is_occupied = 1;
    new_worker->is_busy = 0;
    new_worker->jobs_completed = 0;
    new_worker->next = NULL;
    int index = hash_function(id);
    if (worker_hashmap[index] == NULL) {
        worker_hashmap[index] = new_worker;
    } else {
        Worker *current = worker_hashmap[index];
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_worker;
    }
    worker_count++;
    printf("   [+] Worker '%s' (ID %d) added at %s.\n", name, id, room_names[start_room]);
}

int find_best_available_worker(int target_room) {
    int best_worker_id = -1;
    int min_score = INF;
    for (int i = 0; i < HASH_SIZE; i++) {
        Worker *current = worker_hashmap[i];
        while (current != NULL) {
            if (!current->is_occupied || current->is_busy) {
                current = current->next;
                continue;
            }
            int start = current->current_room_id;
            int dist[MAX_NODES], visited[MAX_NODES];
            for (int j = 0; j < MAX_NODES; j++) {
                dist[j] = INF;
                visited[j] = 0;
            }
            dist[start] = 0;
            for (int count = 0; count < MAX_NODES - 1; count++) {
                int min = INF, u = -1;
                for (int v = 0; v < MAX_NODES; v++) {
                    if (!visited[v] && dist[v] <= min) {
                        min = dist[v];
                        u = v;
                    }
                }
                if (u == -1)
                    break;
                visited[u] = 1;
                for (int v = 0; v < MAX_NODES; v++) {
                    if (!visited[v] && graph[u][v] && dist[u] != INF && dist[u] + graph[u][v] < dist[v]) {
                        dist[v] = dist[u] + graph[u][v];
                    }
                }
            }
            int score = dist[target_room] + (current->jobs_completed * 2);
            if (score < min_score) {
                min_score = score;
                best_worker_id = current->worker_id;
            }
            current = current->next;
        }
    }
    return best_worker_id;
}

void mark_worker_busy(int worker_id, int busy) {
    Worker *worker = get_worker(worker_id);
    if (worker != NULL) {
        worker->is_busy = busy;
    }
}
