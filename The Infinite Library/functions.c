#include "functions.h"
#include <stdio.h>
#include <stdlib.h>

/* Globals declared in functions.h as extern:
   AdjList *adj_list;
   int *visited;
   int N_places;
   We define them here. */

AdjList *adj_list = NULL;
int *visited = NULL;
int N_places = 0;

/* Internal compact graph representation helpers (we keep per-node neighbor arrays
   as adj_list[i].neighbors with length equal to degree[i]) */

/* Free graph memory (internal helper). Call if you want to destroy graph. */
static void free_graph_internal(void) {
    if (adj_list) {
        for (int i = 1; i <= N_places; ++i) {
            free(adj_list[i].neighbors);
            adj_list[i].neighbors = NULL;
            adj_list[i].size = 0;
        }
        free(adj_list);
        adj_list = NULL;
    }
    if (visited) {
        free(visited);
        visited = NULL;
    }
    N_places = 0;
}

/* Build adjacency lists from input edges (eu/ev arrays of size M).
   Uses 1-based node indices. */
static void build_adj_lists_from_edges(int N, int M, int *eu, int *ev) {
    N_places = N;
    // allocate adj_list (1..N)
    adj_list = (AdjList*)malloc((N_places + 1) * sizeof(AdjList));
    if (!adj_list) {
        fprintf(stderr, "Allocation error (adj_list)\n");
        exit(1);
    }

    // degree count
    int *deg = (int*)calloc(N_places + 1, sizeof(int));
    if (!deg) { fprintf(stderr, "Allocation error (deg)\n"); exit(1); }
    for (int i = 0; i < M; ++i) {
        int u = eu[i], v = ev[i];
        if (u >= 1 && u <= N_places) deg[u]++;
        if (v >= 1 && v <= N_places) deg[v]++;
    }

    // allocate neighbor arrays with exact sizes
    for (int i = 1; i <= N_places; ++i) {
        adj_list[i].size = 0;
        if (deg[i] > 0) {
            adj_list[i].neighbors = (int*)malloc(deg[i] * sizeof(int));
            if (!adj_list[i].neighbors) { fprintf(stderr, "Allocation error (nbr)\n"); exit(1); }
        } else {
            adj_list[i].neighbors = NULL;
        }
    }

    // fill neighbors using index pointer
    int *idx = (int*)calloc(N_places + 1, sizeof(int));
    if (!idx) { fprintf(stderr, "Allocation error (idx)\n"); exit(1); }
    for (int i = 0; i < M; ++i) {
        int u = eu[i], v = ev[i];
        if (u < 1 || u > N_places || v < 1 || v > N_places) continue; // ignore invalid
        adj_list[u].neighbors[idx[u]++] = v;
        adj_list[u].size++;
        adj_list[v].neighbors[idx[v]++] = u;
        adj_list[v].size++;
    }

    free(deg);
    free(idx);
}

/* Iterative DFS using a stack to mark reachable nodes starting from 's' (1-based). */
static void iterative_dfs_mark(int s) {
    if (!visited) return;
    int *stack = (int*)malloc((N_places + 5) * sizeof(int));
    if (!stack) { fprintf(stderr, "Allocation error (dfs stack)\n"); exit(1); }
    int top = 0;
    stack[top++] = s;
    visited[s] = 1;
    while (top > 0) {
        int u = stack[--top];
        AdjList *alist = &adj_list[u];
        for (int i = 0; i < alist->size; ++i) {
            int v = alist->neighbors[i];
            if (!visited[v]) {
                visited[v] = 1;
                stack[top++] = v;
            }
        }
    }
    free(stack);
}

/* --- Public function: read graph, build adjacency, count components --- */
int countSeparateKingdoms(void) {
    // If a graph was previously built in globals, free it first
    if (adj_list != NULL || visited != NULL) {
        free_graph_internal();
    }

    int M_routes;
    int N;
    if (scanf("%d %d", &N, &M_routes) != 2) {
        return 0;
    }
    if (N < 1) return 0;

    // read all edges temporarily
    int *eu = (int*)malloc(M_routes * sizeof(int));
    int *ev = (int*)malloc(M_routes * sizeof(int));
    if ((!eu && M_routes>0) || (!ev && M_routes>0)) { fprintf(stderr,"Alloc error edges\n"); exit(1); }
    for (int i = 0; i < M_routes; ++i) {
        if (scanf("%d %d", &eu[i], &ev[i]) != 2) {
            // malformed input: free and return
            free(eu); free(ev);
            return 0;
        }
    }

    // build adjacency lists
    build_adj_lists_from_edges(N, M_routes, eu, ev);

    // allocate global visited (int) and zero it
    visited = (int*)malloc((N_places + 1) * sizeof(int));
    if (!visited) { fprintf(stderr, "Alloc error visited\n"); exit(1); }
    for (int i = 1; i <= N_places; ++i) visited[i] = 0;

    // count components using iterative DFS
    int kingdom_count = 0;
    for (int i = 1; i <= N_places; ++i) {
        if (!visited[i]) {
            kingdom_count++;
            iterative_dfs_mark(i);
        }
    }

    // We keep the graph and visited in memory so BFS can use it later.
    // If you prefer visited to be reset to zeros for BFS, we can reset it there or in BFS.
    for (int i = 1; i <= N_places; ++i) visited[i] = 0;

    free(eu);
    free(ev);

    return kingdom_count;
}

/* --- Public function: BFS shortest path using the built adj_list ---
   start and target are expected 1-based.
   Prints:
     - a line with the distance (integer, -1 if none)
     - a line with the path nodes separated by spaces (1-based). Empty line if no path.
*/
void BFS_shortest_path(int start, int target) {
    if (adj_list == NULL || N_places <= 0) {
        // graph not built
        printf("-1\n\n");
        return;
    }
    if (start < 1 || start > N_places || target < 1 || target > N_places) {
        printf("-1\n\n");
        return;
    }

    // BFS arrays
    int *queue = (int*)malloc((N_places + 5) * sizeof(int));
    int *parent = (int*)malloc((N_places + 1) * sizeof(int));
    char *vis = (char*)malloc((N_places + 1) * sizeof(char));
    if (!queue || !parent || !vis) {
        fprintf(stderr, "Alloc error (bfs)\n");
        exit(1);
    }
    for (int i = 1; i <= N_places; ++i) { vis[i] = 0; parent[i] = -1; }

    int head = 0, tail = 0;
    queue[tail++] = start;
    vis[start] = 1;
    parent[start] = -1;

    while (head < tail) {
        int u = queue[head++];
        if (u == target) break; // optional early stop
        AdjList *alist = &adj_list[u];
        for (int i = 0; i < alist->size; ++i) {
            int v = alist->neighbors[i];
            if (!vis[v]) {
                vis[v] = 1;
                parent[v] = u;
                queue[tail++] = v;
            }
        }
    }

    // if target was reached (parent[target] != -1) or start==target
    if (parent[target] != -1 || start == target) {
        // reconstruct path
        int depth = 0;
        int cur = target;
        while (cur != -1) { depth++; cur = parent[cur]; }
        int dist = depth - 1;
        printf("%d\n", dist);

        int *path = (int*)malloc((dist + 1) * sizeof(int));
        int idx = 0;
        cur = target;
        while (cur != -1) {
            path[idx++] = cur;
            cur = parent[cur];
        }
        for (int i = idx - 1; i >= 0; --i) {
            printf("%d", path[i]);
            if (i > 0) printf(" ");
        }
        printf("\n");
        free(path);
    } else {
        // no path
        printf("-1\n\n");
    }

    free(queue); free(parent); free(vis);
}

/* Optional: function to free the built graph from memory.
   If you want to free right after BFS, call this function from main. */
void free_built_graph(void) {
    free_graph_internal();
}
