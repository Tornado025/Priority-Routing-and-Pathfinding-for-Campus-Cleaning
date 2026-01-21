#include <string.h>
#include "sample_data.h"
#include "worker.h"
#include "context.h"

void addSampleRooms(char room_names[][20], int graph[][30]) {
    strcpy(room_names[0], "Kotak Lab");
    strcpy(room_names[1], "CV Block");
    strcpy(room_names[2], "EC Block");
    strcpy(room_names[3], "EE Block");
    strcpy(room_names[4], "Admin Block");
    strcpy(room_names[5], "CS Block");
    strcpy(room_names[6], "Ground");
    strcpy(room_names[7], "ET Block");

    memset(graph, 0, sizeof(int) * MAX_NODES * MAX_NODES);
    
    // kotak to cv
    graph[0][1] = graph[1][0] = 1;
    // kotak to ground
    graph[0][6] = graph[6][0] = 2;
    // cv to ee
    graph[1][3] = graph[3][1] = 2;
    // ee to ec
    graph[3][2] = graph[2][3] = 1;
    // ground to cs
    graph[6][5] = graph[5][6] = 2;
    // cs to ee
    graph[5][3] = graph[3][5] = 2;
    // ec to et
    graph[2][7] = graph[7][2] = 2;
    // ee to et
    graph[3][7] = graph[7][3] = 4;
    // admin to kotak
    graph[4][0] = graph[0][4] = 6;
}

void addSampleWorkers(int *worker_count) {
    init_worker_hashmap();
    *worker_count = 0;
    
    add_worker(101, "A", 0);
    add_worker(102, "B", 1);
    add_worker(103, "C", 2);
    add_worker(104, "D", 3);
    add_worker(105, "E", 4);
}