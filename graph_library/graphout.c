#include "graph.h"
#include <stdio.h>
#include "mytypes.h"

void 
printGraph(struct Graph* graph) {
    long v;
    printf("\n\nCurrent vertex number with free space %ld\n", graph->numVertices);
    for (v = 0; v < graph->numVertices; v++) {
        if (graph->map[v] == -1) {
            continue;
        }
        struct node* temp = graph->adjLists[v];
        printf("\n Adjacency list of vertex %ld\n ", graph->map[v]);
        while (temp) {
            printf("%ld (%ld) , ", temp->vertex, temp->length);
            temp = temp->next;
        }
        printf("\n\n");
    }
}