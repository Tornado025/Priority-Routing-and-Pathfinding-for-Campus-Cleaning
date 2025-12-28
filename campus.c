#include "campus.h"

int graph[MAX_NODES][MAX_NODES];
char room_names[MAX_NODES][20];
Worker* worker_hashmap[HASH_SIZE];
int worker_count = 0;

void init_system() {
    strcpy(room_names[0], "Reception");
    strcpy(room_names[1], "Lobby");
    strcpy(room_names[2], "Admin Office");
    strcpy(room_names[3], "Cafeteria");
    strcpy(room_names[4], "Chem Lab");
    strcpy(room_names[5], "Classroom 101");

    memset(graph, 0, sizeof(graph));
    
    graph[0][1] = graph[1][0] = 2;
    graph[1][2] = graph[2][1] = 3;
    graph[1][3] = graph[3][1] = 5;
    graph[2][4] = graph[4][2] = 2;
    graph[3][5] = graph[5][3] = 4;
    graph[4][5] = graph[5][4] = 6;
    
    init_worker_hashmap();
    worker_count = 0;
    
    add_worker(101, "Ravi", 0);
    add_worker(102, "Priya", 1);
    add_worker(103, "Kumar", 3);
}

void initPQ(PriorityQueue *pq) {
    pq->size = 0;
    pq->counter = 0; 
}

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

void enqueue(PriorityQueue *pq, int room_id, int priority, int is_emergency, char* cause) {
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
    if (pq->size <= 0) return empty;
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

        if (largest == i) break;
        swap(&pq->data[i], &pq->data[largest]);
        i = largest;
    }
    return root;
}

void init_worker_hashmap() {
    for (int i = 0; i < HASH_SIZE; i++) {
        worker_hashmap[i] = NULL;
    }
}

int hash_function(int worker_id) {
    return worker_id % HASH_SIZE;
}

Worker* get_worker(int worker_id) {
    int index = hash_function(worker_id);
    Worker* current = worker_hashmap[index];
    
    while (current != NULL) {
        if (current->worker_id == worker_id) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void get_all_workers(Worker** workers_array, int* count) {
    *count = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        Worker* current = worker_hashmap[i];
        while (current != NULL) {
            workers_array[*count] = current;
            (*count)++;
            current = current->next;
        }
    }
}

void add_worker(int id, char* name, int start_room) {
    if (worker_count >= MAX_WORKERS) {
        printf("   [!] Maximum workers reached.\n");
        return;
    }
    
    Worker* new_worker = (Worker*)malloc(sizeof(Worker));
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
        Worker* current = worker_hashmap[index];
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
        Worker* current = worker_hashmap[i];
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
            
            if (u == -1) break;
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
    Worker* worker = get_worker(worker_id);
    if (worker != NULL) {
        worker->is_busy = busy;
    }
}

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

void add_emergency_job(PriorityQueue *pq, int room_id, int priority, char* cause) {
    enqueue(pq, room_id, priority, 1, cause);
}

void print_recursive_path(int parent[], int j) {
    if (parent[j] == -1) return;
    print_recursive_path(parent, parent[j]);
    printf(" -> %s", room_names[j]);
}

void dijkstra(int start_node, int end_node) {
    int dist[MAX_NODES], visited[MAX_NODES], parent[MAX_NODES];
    
    for (int i = 0; i < MAX_NODES; i++) {
        dist[i] = INF; visited[i] = 0; parent[i] = -1;
    }
    dist[start_node] = 0;

    for (int count = 0; count < MAX_NODES - 1; count++) {
        int min = INF, u = -1;
        for (int v = 0; v < MAX_NODES; v++)
            if (!visited[v] && dist[v] <= min) min = dist[v], u = v;

        if (u == -1) break;
        visited[u] = 1;

        for (int v = 0; v < MAX_NODES; v++) {
            if (!visited[v] && graph[u][v] && dist[u] != INF && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }

    if (dist[end_node] == INF) {
        printf("   [!] No path found between %s and %s.\n", 
               room_names[start_node], room_names[end_node]);
    } else {
        printf("   [PATH] Distance: %d units | Route: %s", dist[end_node], room_names[start_node]);
        print_recursive_path(parent, end_node);
        printf("\n");
    }
}

void update_job_priorities(PriorityQueue *pq) {
    for (int i = 0; i < pq->size; i++) {
        Request *req = &pq->data[i];
        
        if (req->is_emergency) continue;
        
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
            
            if (largest == parent) break;
            
            Request temp = pq->data[parent];
            pq->data[parent] = pq->data[largest];
            pq->data[largest] = temp;
            parent = largest;
        }
    }
}
