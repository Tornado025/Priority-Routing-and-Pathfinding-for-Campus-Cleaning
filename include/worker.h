#ifndef WORKER_H
#define WORKER_H

#include "types.h"
#include "context.h"

int hash_function(int worker_id);
void init_worker_hashmap();
Worker *get_worker(int worker_id);
void get_all_workers(Worker **workers_array, int *count);
void add_worker(int id, char *name, int start_room, int skill_type);
int find_best_available_worker(int target_room, int skill_required);
void mark_worker_busy(int worker_id, int busy);

#endif
