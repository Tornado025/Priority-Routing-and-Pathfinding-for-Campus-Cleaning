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
    strcpy(room_names[8], "Chem Block");
    strcpy(room_names[9], "Canteen");
    strcpy(room_names[10], "BT Quadrangle");
    strcpy(room_names[11], "Mech Block");
    strcpy(room_names[12], "IEM Block");

    memset(graph, 0, sizeof(int) * MAX_NODES * MAX_NODES);
    
    // kotak to cv
    graph[0][1] = graph[1][0] = 1;
    // kotak to ground
    graph[0][6] = graph[6][0] = 2;
    // cv to ee
    graph[1][3] = graph[3][1] = 2;
    // cv to ec
    graph[1][2] = graph[2][1] = 2;
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
    
    // et to chem (chem is right next to et)
    graph[7][8] = graph[8][7] = 2;
    // chem to canteen (chem leads to canteen in opposite direction south)
    graph[8][9] = graph[9][8] = 4;
    // canteen to BT Quadrangle (east of canteen is BT Quadrangle)
    graph[9][10] = graph[10][9] = 4;
    // BT Quadrangle, mech, and iem are connected like a triangle
    graph[10][11] = graph[11][10] = 3;  // BT Quadrangle to mech
    graph[11][12] = graph[12][11] = 3;  // mech to iem
    graph[12][10] = graph[10][12] = 3;  // iem to BT Quadrangle
    // mech is connected to the admin block
    graph[11][4] = graph[4][11] = 1;
}

void addSampleWorkers(int *worker_count) {
    init_worker_hashmap();
    *worker_count = 0;
    
    add_worker(101, "A", 0);   // Kotak Lab
    add_worker(102, "B", 1);   // CV Block
    add_worker(103, "C", 2);   // EC Block
    add_worker(104, "D", 3);   // EE Block
    add_worker(105, "E", 4);   // Admin Block
    add_worker(106, "F", 8);   // Chem Block
    add_worker(107, "G", 10);  // BT Quadrangle
    add_worker(108, "H", 11);  // Mech Block
}