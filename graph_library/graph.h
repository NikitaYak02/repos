struct node * createNode(long v, long edge_len);

struct Graph * createGraph(void);

long addVertex(struct Graph* graph, long key);

long find_elem (struct Graph* graph, long key);

struct node * nodeExist(struct Graph * graph, long src, long dest);

void addEdge(struct Graph* graph, long src, long dest, long edge_len);

void delete_graph(struct Graph **graph);

void deleteNode(struct Graph* graph, long src, long dest);

void deleteVertice(struct Graph *graph, long src);

long long Dijkstra(struct Graph* graph, long src, long dest);