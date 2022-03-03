#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "mytypes.h"
/*создание нового ребра для дальнейшей подвязки в список смежности*/
struct node*
createNode(long v, long edge_len) {
    struct node* newNode = malloc(sizeof(struct node));
    newNode->vertex = v;
    newNode->length = edge_len;
    newNode->next = NULL;
    return newNode;
}

/*процедура инициализации графа*/
struct Graph* 
createGraph(void) {
    struct Graph* graph = malloc(sizeof(struct Graph));
    graph->numVertices = 0;
    graph->max = 1;
    graph->adjLists = malloc(sizeof(struct node *));
    graph->map = malloc(sizeof(long));

    graph->adjLists[0] = NULL;

    return graph;
}

/*поиск элемента в массиве с заданным номером*/
long
find_elem (struct Graph* graph, long key) {
    long num;
    for (num = 0; num < graph->numVertices; num++){
        if ((graph->map)[num] == key) {
            return num;
        }
    }
    return -1;
}

/*добавление вершины с определенным номерои*/
long
addVertex(struct Graph* graph, long key) {
    if (find_elem(graph, key) > 0){
        return -1;
    }
    if (graph->max == graph->numVertices) {
        //Реаллокация элементов графа, с проверкой на их 
        if (graph->max < LONG_MAX/2){
            //printf("Udacha pri dobavlenii\n");
            graph->max *= 2;
            graph->adjLists = realloc(graph->adjLists, graph->max* sizeof(struct node_without_length *));
            graph->map = realloc(graph->map, graph->max * sizeof(long));
            for (long k = graph->max / 2; k < graph->max; k++){
                graph->adjLists[k] = NULL;
            }
            if (graph->adjLists == NULL || graph->map == NULL) {
                printf("Neudacha pri dobavlenii\n");
                exit (1);
            }
        } else {
            printf("Neudacha pri dobavlenii\n");
            exit (1);
        }
    }
    long index;
    for (index = 0; index < graph->numVertices; index++) {
        if (graph->map[index] == -1) {
            graph->map[index] = key;
            return index;   
        }
        //printf("Index: %ld\n", index);
    }

    (graph->map)[graph->numVertices] = key;
    (graph->numVertices)++;
    return (graph->numVertices - 1);
}

struct node *
nodeExist(struct Graph * graph, long src, long dest){
    long src_num = find_elem(graph, src), dest_num = find_elem(graph, dest);
    struct node *temp;
    if (src_num != -1|| dest_num != -1){
        temp = graph->adjLists[src_num];
        while (temp != NULL){
            if (temp->vertex == dest) {
                return temp;
            }
            temp = temp->next;
        }
    }
    return 0;
}

void
addEdge(struct Graph* graph, long src, long dest, long edge_len) {
    // Add edge from src to dest
    // src_num is num of massive element with src code
    // dist_num is too
    //
    //
    if (edge_len < 0) {
        printf("Edge_len from %ld to %ld is bellow zero. The length was equal to zero.\n", src, dest);
        edge_len = 0;
    }
    long src_num = find_elem(graph, src), dest_num = find_elem(graph, dest);
    if (src_num == -1) {
        src_num = addVertex(graph, src);
    }
    if (dest_num == -1) {
        dest_num = addVertex(graph, dest);
    }
    struct node * is_exist = nodeExist(graph, src, dest);
    if (is_exist) {
        is_exist->length = edge_len;
        if (dest == src) {
            return;
        }
        is_exist = nodeExist(graph, dest, src);
        is_exist->length = edge_len;
        return;
    }
    struct node* newNode = createNode(dest, edge_len);
    newNode->next = graph->adjLists[src_num];
    graph->adjLists[src_num] = newNode;

    //Если подали цикл, то добавляем его один раз и выходим 
    if (src == dest) {
        return;
    }
    // Add edge from dest to src
    newNode = createNode(src, edge_len);
    newNode->next = graph->adjLists[dest_num];
    graph->adjLists[dest_num] = newNode;
}

//функция удаления графа(нерекурсивно, 
//т.к. в этом нет смысла(это будет дольне по времени)
void
delete_graph(struct Graph **graph){
    struct Graph *temp_g = *graph;
    struct node * temp_n1, * temp_n2;
    for (long i = 0; i < temp_g->numVertices; i++) {
        temp_n1 = temp_g->adjLists[i];
        while (temp_n1 != NULL){
            temp_n2 = temp_n1->next;
            free(temp_n1);
            temp_n1 = temp_n2;
        }
    }
    free(temp_g->map);
    free(temp_g->adjLists);
    free(temp_g);
    *graph = 0;
}

//функция удаления в нужном списке смежности соединяющего ребра
void
del_el(struct Graph* graph, long src_num, long dest){
    struct node *temp_n = graph->adjLists[src_num], * temp_n1;
    if (temp_n == NULL){
        return;
    }
    if (temp_n->vertex == dest) {
        graph->adjLists[src_num] = (graph->adjLists[src_num])->next;
        free(temp_n);
    } else {
        while (temp_n->next != NULL && (temp_n->next)->vertex != dest) {
            temp_n = temp_n->next; 
        }
        if (temp_n->next == NULL) {
            return ;
        }
        temp_n1 = (temp_n->next)->next;
        free(temp_n->next);
        temp_n->next = temp_n1;
    }
}

//удаление ребра 
void
deleteNode(struct Graph* graph, long src, long dest) {
    if (graph->numVertices == 0) {
        return ;
    }
    long src_num = find_elem(graph, src), dest_num = find_elem(graph, dest);
    if (src_num == -1|| dest_num == -1) {
        return;
    }
    del_el(graph, src_num, dest);
    del_el(graph, dest_num, src);
}

//удаление вершины, а как следствие всех ребер, которые с ним соединяли
void
deleteVertice(struct Graph *graph, long src) {
    if (graph->numVertices == 0) {
        return;
    }
    long src_num = find_elem(graph, src), dest_num;
    if (src_num == -1) {
        return;
    }
    struct node *temp_n = graph->adjLists[src_num];
    while(temp_n != NULL){
        graph->adjLists[src_num] = (graph->adjLists[src_num])->next;
        dest_num = find_elem(graph, temp_n->vertex);
        del_el(graph, dest_num, src);
        free(temp_n);
        temp_n = graph->adjLists[src_num];
    }
    graph->map[src_num] = -1;
}
// число неотрицательное - если путь существует, -1 -если нет
long long
Dijkstra(struct Graph* graph, long src, long dest) {
    long src_num, dest_num;
    if ((src_num = find_elem(graph, src)) == -1|| (dest_num = find_elem(graph, dest)) == -1) {
        return -1;
    }
    if (dest == src){
        return 0;
    }
    long long len_mas[graph->numVertices];
    int bool_mas[graph->numVertices];
    for (long long i = 0; i < graph->numVertices; i++) {
        len_mas[i] = -1;
        bool_mas[i] = -1;
    }
    len_mas[src_num] = 0;
    long read_peak = 0, avial_peak = 1, cur_peak = src, cur_peak_num = find_elem(graph, cur_peak);
    long temp_peak, temp_peak_num;
    struct node * temp_n;
    do {
        temp_n = graph->adjLists[cur_peak_num];
        while (temp_n != NULL) {
            temp_peak = temp_n->vertex;
            temp_peak_num = find_elem(graph, temp_peak);
            //дошли до вершины, которую встречали ранее в обходе
            if ((len_mas[temp_peak_num] > len_mas[cur_peak_num] + temp_n->length) && bool_mas[temp_peak_num] == 1){
                len_mas[temp_peak_num] = len_mas[cur_peak_num] + temp_n->length;
            }
            //нашли вершину, не встреченную ранее
            if (len_mas[temp_peak_num] == -1) {
                avial_peak++; 
                len_mas[temp_peak_num] = temp_n->length + len_mas[cur_peak_num];
                bool_mas[temp_peak_num] = 1;
                //printf("%ld %ld\n", temp_peak, temp_peak_num);
            }
            temp_n = temp_n->next;
        }
        read_peak++;
        bool_mas[cur_peak_num] = 0;
        for (long long i = 0; i < graph->numVertices; i++){
            if (bool_mas[i] == 1 && ((len_mas[i] < len_mas[cur_peak_num]) || (bool_mas[cur_peak_num] == 0)) ){
                //printf("i: %lld %lld %lld\n", i, len_mas[i],  len_mas[cur_peak_num]);
                cur_peak_num = i;
                cur_peak = graph->map[i];
            }
        }
        if (cur_peak == dest) {
            break;
        }
    } while (read_peak != avial_peak);
    return len_mas[dest_num];
}