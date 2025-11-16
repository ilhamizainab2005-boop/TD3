// functions.h
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

typedef struct {
    int *neighbors;
    int size;
} AdjList;

extern AdjList *adj_list;
extern int *visited;
extern int N_places;

int countSeparateKingdoms(void);
void BFS_shortest_path(int start, int target);
void free_built_graph(void);  // <- ajouter cette ligne

#endif

