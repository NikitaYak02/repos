#include "graph.h"
#include "graphout.h"
#include "mytypes.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "graphout.h"

int 
main(void) {
    struct Graph* graph = createGraph();
    long x[3], par_num, t;
    long long res;
    char str[100], buf[15], *pointer, *p1;
    const char *word[] = {"AddVertex", "AddEdge", "DeleteVertex",\
     "DeleteEdge", "EdgePointer", "VertexNum", "Dijkstra", "PrintGraph", "DeleteGraph"};
    while(fgets(str, 100, stdin)){
        pointer = str;
        if (!sscanf(pointer, "%s", buf) || strlen(str) > 98){
            printf("Incorrect option name. Step skipped.\n");
            continue;
        };
        buf[strlen(buf)] = '\0';
        pointer += strlen(buf);
        par_num = 0;
        while (par_num < 3) {
            x[par_num] = strtol(pointer, &p1, 10);
            if (p1 == pointer){
                break;
            }
            par_num++;
            pointer = p1;
        }

        if (*pointer != ' ' && *pointer != '\0' && *pointer != '\n'){
            printf("Bad input\n");
        } else {
            t = 0;
            while (t < 9 && strcmp(word[t], buf) != 0){
                t++;
            }
            switch (t) {
                case 0:
                    if (par_num != 1){
                        printf("Incorrect number of true param\n");
                        break;
                    }
                    addVertex(graph, x[0]);
                    break;

                case 1:
                    if (par_num != 3){
                        printf("Incorrect number of true param\n");
                        break;
                    }
                    addEdge(graph, x[0], x[1], x[2]);
                    break;

                case 2:
                    if (par_num != 1){
                        printf("Incorrect number of true param\n");
                        break;
                    }
                    deleteVertice(graph, x[0]);
                    break;

                case 3:
                    if (par_num != 2){
                        printf("Incorrect number of true param\n");
                        break;
                    }
                    deleteNode(graph, x[0], x[1]);
                    break;

                case 4:
                    if (par_num != 2){
                        printf("Incorrect number of true param\n");
                        break;
                    }
                    res =(long long) nodeExist(graph, x[0], x[1]);
                    if (res){
                        printf("Node exist from %ld to %ld\n", x[0], x[1]);
                    } else {
                        printf("Node don't exist from %ld to %ld\n", x[0], x[1]);
                    }
                    break;

                case 5:
                    if (par_num != 1){
                        printf("Incorrect number of true param\n");
                        break;
                    }
                    res = find_elem(graph, x[0]);
                        if (res){
                            printf("Vertex %ld is exist\n", x[0]);
                        } else {
                            printf("Vertex %ld isn'n exist\n", x[0]);
                        }
                    break;

                case 6:
                    if (par_num != 2){
                        printf("Incorrect number of true param\n");
                        break;
                    }
                    res = Dijkstra(graph, x[0], x[1]);
                    if (res < 0) {
                        printf("Way from %ld to %ld didn't exist\n", x[0], x[1]);
                    } else {
                        printf("Way length from %ld to %ld is %lld\n", x[0], x[1], res);
                    }
                    break;

                case 7:
                    printGraph(graph);
                    break;

                case 8:
                    delete_graph(&graph);
                    graph = createGraph();
                    break;

                default:
                    printf("Incorrect input.\n");
            }
        }
    };
    delete_graph(&graph);
    return 0;
}