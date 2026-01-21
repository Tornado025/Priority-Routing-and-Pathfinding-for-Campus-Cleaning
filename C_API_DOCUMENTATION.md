# C CORE API DOCUMENTATION & DATA FLOW

## OVERVIEW
The C core provides a minimal FFI interface exposing data structures and operations for the campus cleaning scheduler. All operations manipulate shared global state via context.c.

---

## GLOBAL STATE (context.c/context.h)

### Data Structures (Read/Write via FFI)
```c
int graph[MAX_NODES][MAX_NODES];           // Adjacency matrix for campus graph
char room_names[MAX_NODES][20];            // Room name strings
Worker *worker_hashmap[HASH_SIZE];         // Hash table buckets (linked lists)
int worker_count;                          // Current worker count
```

### Initialization
```c
void init_context()
```
- Called internally by `init_system()`
- Zeros out worker_count
- Initializes hash map buckets to NULL

---

## DATA STRUCTURES

### Worker (types.h)
```c
typedef struct Worker {
    int worker_id;              // Unique ID
    int current_room_id;        // Current location (index into room_names)
    char name[20];              // Worker name
    int is_occupied;            // 1 if active, 0 if inactive
    int is_busy;                // 1 if executing job, 0 if available
    int jobs_completed;         // Completion counter
    struct Worker *next;        // Hash collision chain pointer
} Worker;
```

### Request (types.h)
```c
typedef struct {
    int room_id;                // Target room index
    int priority;               // Base priority (1-10)
    int effective_priority;     // priority + aging boost
    int arrival_time;           // Sequence number for FIFO tiebreaker
    int wait_count;             // Cycles spent waiting
    int is_emergency;           // 1 for emergency, 0 for routine
    char cause[100];            // Description
} Request;
```

### PriorityQueue (priority_queue.h)
```c
typedef struct {
    Request data[MAX_REQUESTS]; // Max heap array
    int size;                   // Current heap size
    int counter;                // Global arrival timestamp counter
} PriorityQueue;
```

---

## API FUNCTIONS

### 1. SYSTEM INITIALIZATION

#### `void init_system()`
- **Source:** campus.c
- **Called by:** GUI on startup
- **Operations:**
  1. Calls `addSampleRooms()` to populate `graph[][]` and `room_names[]`
  2. Calls `addSampleWorkers()` to initialize worker hash map
- **Data modified:** `graph`, `room_names`, `worker_hashmap`, `worker_count`

---

### 2. PRIORITY QUEUE OPERATIONS

#### `void initPQ(PriorityQueue *pq)`
- **Source:** priority_queue.c
- **Called by:** GUI on startup
- **Operations:** Zeros `pq->size` and `pq->counter`
- **Data modified:** PriorityQueue structure

#### `void enqueue(PriorityQueue *pq, int room_id, int priority, int is_emergency, char *cause)`
- **Source:** priority_queue.c
- **Called by:** 
  - `init_routine_jobs()` (6 times for routine jobs)
  - `add_emergency_job()` (emergency insertion)
  - `execute_job()` in GUI (re-queue routine jobs)
- **Operations:**
  1. Creates Request at `pq->data[pq->size]`
  2. Sets fields: room_id, priority, effective_priority=priority, is_emergency, cause
  3. Increments `pq->counter` and assigns as `arrival_time`
  4. Increments `pq->size`
  5. **Heap up operation:** Swaps with parent while priority is higher
- **Data modified:** `pq->data[]`, `pq->size`, `pq->counter`
- **Algorithm:** Max heap insertion with custom comparator

#### `Request dequeue(PriorityQueue *pq)`
- **Source:** priority_queue.c
- **Called by:** `execute_job()` in GUI
- **Operations:**
  1. Returns empty Request if `pq->size == 0`
  2. Extracts root `pq->data[0]`
  3. Moves last element to root
  4. Decrements `pq->size`
  5. **Heap down operation:** Swaps with larger child while not max
- **Returns:** Request with highest priority (or empty with room_id=-1)
- **Data modified:** `pq->data[]`, `pq->size`
- **Algorithm:** Max heap extraction

#### `int is_higher_priority(Request a, Request b)`
- **Source:** priority_queue.c
- **Called by:** `enqueue()`, `dequeue()`, `update_job_priorities()`
- **Priority rules (in order):**
  1. Emergency flag: `a.is_emergency > b.is_emergency`
  2. Effective priority: `a.effective_priority > b.effective_priority`
  3. FIFO tiebreaker: `a.arrival_time < b.arrival_time`
- **Returns:** 1 if a has higher priority, 0 otherwise

---

### 3. JOB MANAGEMENT

#### `void init_routine_jobs(PriorityQueue *pq)`
- **Source:** job.c
- **Called by:** GUI "Initialize Routine Jobs" button
- **Operations:**
  1. Calls `enqueue()` 6 times for rooms 0-5
  2. Priorities: [3, 5, 5, 7, 7, 5]
  3. All have `is_emergency=0`
- **Data modified:** `pq->data[]`, `pq->size`, `pq->counter`

#### `void add_emergency_job(PriorityQueue *pq, int room_id, int priority, char *cause)`
- **Source:** job.c
- **Called by:** GUI "Add Emergency Job" button
- **Operations:** Calls `enqueue()` with `is_emergency=1`
- **Data modified:** `pq->data[]`, `pq->size`, `pq->counter`

#### `void update_job_priorities(PriorityQueue *pq)`
- **Source:** job.c
- **Called by:** 
  - GUI "Age/Update Priorities" button
  - `execute_job()` before dequeue
- **Operations:**
  1. **For each Request in heap:**
     - Skip if `is_emergency == 1`
     - Increment `wait_count`
     - Calculate `age_boost = wait_count / 3`
     - Set `effective_priority = priority + age_boost`
     - Cap at `EMERGENCY_THRESHOLD - 1` (8)
  2. **Heapify entire heap:** Bottom-up heap reconstruction
- **Data modified:** `pq->data[]` (wait_count, effective_priority fields)
- **Algorithm:** Aging + complete heap rebuild

---

### 4. WORKER MANAGEMENT

#### `void init_worker_hashmap()`
- **Source:** worker.c
- **Called by:** `addSampleWorkers()` in sample_data.c
- **Operations:** Sets all `worker_hashmap[i] = NULL`
- **Data modified:** `worker_hashmap[]`

#### `int hash_function(int worker_id)`
- **Source:** worker.c
- **Called by:** `add_worker()`, `get_worker()`
- **Operations:** Returns `worker_id % HASH_SIZE`
- **Algorithm:** Modulo hashing

#### `void add_worker(int id, char *name, int start_room)`
- **Source:** worker.c
- **Called by:** `addSampleWorkers()` in sample_data.c (5 workers)
- **Operations:**
  1. Allocates Worker struct
  2. Sets fields: worker_id, name, current_room_id, is_occupied=1, is_busy=0, jobs_completed=0
  3. Hashes ID to bucket index
  4. Inserts at end of linked list in bucket
  5. Increments `worker_count`
- **Data modified:** `worker_hashmap[]`, `worker_count`
- **Algorithm:** Hash table with chaining

#### `Worker *get_worker(int worker_id)`
- **Source:** worker.c
- **Called by:** `execute_job()` in GUI
- **Operations:**
  1. Hashes worker_id to bucket
  2. Traverses linked list to find matching worker_id
- **Returns:** Pointer to Worker or NULL
- **Algorithm:** Hash table lookup

#### `void get_all_workers(Worker **workers_array, int *count)`
- **Source:** worker.c
- **Called by:** GUI to display worker table (deprecated in current GUI)
- **Operations:**
  1. Iterates all hash buckets
  2. Traverses linked lists
  3. Fills workers_array with pointers
  4. Sets count
- **Data modified:** workers_array (output parameter)

#### `int find_best_available_worker(int target_room)`
- **Source:** worker.c
- **Called by:** `execute_job()` in GUI
- **Operations:**
  1. **For each worker in hash map:**
     - Skip if `!is_occupied || is_busy`
     - **Run Dijkstra:** Calculate shortest path from `worker.current_room_id` to `target_room`
     - Calculate score: `distance + (jobs_completed * 2)`
  2. Return worker_id with minimum score
- **Returns:** worker_id or -1 if no available workers
- **Data read:** `graph[][]`, `worker_hashmap[]`
- **Algorithm:** Dijkstra + greedy selection with load balancing penalty

---

### 5. PATHFINDING

#### `void dijkstra(int start_node, int end_node)`
- **Source:** dijkstra.c
- **Called by:** CLI (not used by GUI, but available)
- **Operations:**
  1. Initializes `dist[]` array with INF, `visited[]`, `parent[]`
  2. Sets `dist[start_node] = 0`
  3. **For each iteration:**
     - Find unvisited node with minimum distance
     - Mark visited
     - Update distances of neighbors via `graph[u][v]`
  4. Reconstructs path via parent array
  5. Prints to stdout (not captured by GUI)
- **Data read:** `graph[][]`, `room_names[]`
- **Output:** stdout (printf)
- **Algorithm:** Classic Dijkstra on adjacency matrix

---

### 6. SAMPLE DATA

#### `void addSampleRooms(char room_names[][20], int graph[][30])`
- **Source:** sample_data.c
- **Called by:** `init_system()`
- **Operations:**
  1. Copies 8 room names into `room_names[]`
  2. Zeros `graph[][]`
  3. Sets edge weights for campus topology
- **Data modified:** `room_names[]`, `graph[][]`

#### `void addSampleWorkers(int *worker_count)`
- **Source:** sample_data.c
- **Called by:** `init_system()`
- **Operations:**
  1. Calls `init_worker_hashmap()`
  2. Calls `add_worker()` 5 times for workers A-E
- **Data modified:** `worker_hashmap[]`, `worker_count`

---

## GUI TO C INTERACTION FLOW

### Startup Sequence
```
GUI.__init__()
  └─> lib.init_system()
        ├─> addSampleRooms() -> Populates graph[][], room_names[]
        └─> addSampleWorkers()
              ├─> init_worker_hashmap() -> Zeros worker_hashmap[]
              └─> add_worker() x5 -> Creates workers in hash map
  └─> lib.initPQ(&pq) -> Zeros pq.size, pq.counter
```

### Initialize Routine Jobs Flow
```
GUI.init_routine()
  └─> lib.init_routine_jobs(&pq)
        └─> enqueue() x6
              └─> For each: Insert into pq.data[], heap-up
```

### Add Emergency Job Flow
```
GUI.add_emergency()
  └─> lib.add_emergency_job(&pq, room, priority, cause)
        └─> enqueue() with is_emergency=1
              └─> Insert into pq.data[], heap-up (will bubble to top)
```

### Execute Job Flow
```
GUI.execute_job()
  ├─> lib.update_job_priorities(&pq)
  │     └─> For each non-emergency Request:
  │           └─> Increment wait_count, recalc effective_priority, heapify
  │
  ├─> lib.dequeue(&pq)
  │     └─> Extract pq.data[0], move last to root, heap-down
  │
  ├─> lib.find_best_available_worker(req.room_id)
  │     └─> For each available worker:
  │           ├─> Run Dijkstra on graph[][]
  │           └─> Score = distance + (jobs_completed * 2)
  │
  ├─> lib.get_worker(worker_id)
  │     └─> Hash lookup in worker_hashmap[]
  │
  └─> worker.current_room_id = req.room_id (GUI modifies C struct)
      worker.jobs_completed++
      
      If routine job:
        └─> lib.enqueue(&pq, ...) -> Re-add to queue
```

### Age/Update Priorities Flow
```
GUI.update_priorities()
  └─> lib.update_job_priorities(&pq)
        └─> Same as above
```

---

## DATA STRUCTURE VISUALIZATIONS

### Priority Queue (Max Heap)
```
pq.data[0] = highest priority Request (root)
pq.data[1], pq.data[2] = children of root
pq.data[2*i+1], pq.data[2*i+2] = children of node i
```
**GUI displays:** Text dump of all elements with indices showing heap structure

### Worker Hash Map (Chaining)
```
worker_hashmap[0] -> Worker(101) -> NULL
worker_hashmap[1] -> NULL
worker_hashmap[2] -> Worker(102) -> Worker(112) -> NULL
...
```
**GUI displays:** Table view (deprecated in current version)

### Campus Graph (Adjacency Matrix)
```
graph[i][j] = weight of edge from room i to room j (0 if no edge)
```
**GUI displays:** Visual 2D graph with nodes, edges, and room numbers/names

---

## IMPORTANT NOTES

1. **Priority Queue is NOT stdout:** The PriorityQueue widget reads `pq.data[]` array directly via FFI, not console output

2. **Minimal C Logic:** All C functions are pure operations on data structures. No business logic beyond:
   - Heap operations (enqueue/dequeue)
   - Hash map operations (add/lookup)
   - Dijkstra pathfinding
   - Aging formula: `effective_priority = priority + (wait_count / 3)`

3. **GUI Ownership:** The GUI owns the PriorityQueue instance and passes it to C by reference. C never allocates the queue.

4. **Shared Memory:** `graph[]`, `room_names[]`, `worker_hashmap[]` are global variables in the C library, accessed via `ctypes.in_dll()`

5. **Worker Modification:** The GUI directly modifies Worker struct fields after getting pointer from `get_worker()`

6. **No Cleanup:** No explicit memory management needed for globals. Workers are never freed (assumption: fixed worker set).
