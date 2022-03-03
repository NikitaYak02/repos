struct node {
    long vertex;
    long length;
    struct node* next;
};

struct Graph {
    long numVertices;
    long max;
    struct node** adjLists;
    long *map;
};