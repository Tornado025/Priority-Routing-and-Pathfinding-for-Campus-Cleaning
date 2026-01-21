#include "dijkstra.h"
#include <stdio.h>
#include <limits.h>

#define INF INT_MAX

void print_recursive_path(int parent[], int j) {
    if (parent[j] == -1)
        return;
    print_recursive_path(parent, parent[j]);
    printf(" -> %s", room_names[j]);
}

void dijkstra(int start_node, int end_node) {
    int dist[MAX_NODES], visited[MAX_NODES], parent[MAX_NODES];
    for (int i = 0; i < MAX_NODES; i++) {
        dist[i] = INF;
        visited[i] = 0;
        parent[i] = -1;
    }
    dist[start_node] = 0;
    for (int count = 0; count < MAX_NODES - 1; count++) {
        int min = INF, u = -1;
        for (int v = 0; v < MAX_NODES; v++)
            if (!visited[v] && dist[v] <= min)
                min = dist[v], u = v;
        if (u == -1)
            break;
        visited[u] = 1;
        for (int v = 0; v < MAX_NODES; v++) {
            if (!visited[v] && graph[u][v] && dist[u] != INF && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }
    if (dist[end_node] == INF) {
        printf("   [!] No path found between %d and %d.\n", start_node, end_node);
    } else {
        printf("   [PATH] Distance: %d units | Route: %s", dist[end_node], room_names[start_node]);
        print_recursive_path(parent, end_node);
        printf("\n");
    }
}
