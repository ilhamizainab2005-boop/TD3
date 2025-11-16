#include <stdio.h>
#include <stdlib.h>

// Structure d'un noeud AVL
typedef struct Node {
    int id;
    int height;
    struct Node *left, *right;
} Node;

static Node *root = NULL;

// Utility pour la hauteur
static int height(Node *n) {
    return n ? n->height : 0;
}

// Maximum de deux entiers
static int max(int a, int b) {
    return (a > b) ? a : b;
}

// Crée un nouveau noeud
static Node* create_node(int id) {
    Node *n = (Node*)malloc(sizeof(Node));
    if(!n) { fprintf(stderr, "Memory allocation failed!\n"); exit(1); }
    n->id = id;
    n->left = n->right = NULL;
    n->height = 1;
    return n;
}

// Rotation droite
static Node* right_rotate(Node *y) {
    Node *x = y->left;
    Node *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

// Rotation gauche
static Node* left_rotate(Node *x) {
    Node *y = x->right;
    Node *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

// Balance factor
static int get_balance(Node *n) {
    return n ? height(n->left) - height(n->right) : 0;
}

// Insertion AVL
static Node* insert(Node *node, int id) {
    if (!node) return create_node(id);

    if (id < node->id)
        node->left = insert(node->left, id);
    else if (id > node->id)
        node->right = insert(node->right, id);
    else
        return node; // pas de doublon

    node->height = 1 + max(height(node->left), height(node->right));

    int balance = get_balance(node);

    // Rotations pour équilibrer
    if (balance > 1 && id < node->left->id) return right_rotate(node);
    if (balance < -1 && id > node->right->id) return left_rotate(node);
    if (balance > 1 && id > node->left->id) {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }
    if (balance < -1 && id < node->right->id) {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }

    return node;
}

// Chercher un ID
static int search(Node *node, int id) {
    if (!node) return 0;
    if (id == node->id) return 1;
    if (id < node->id) return search(node->left, id);
    return search(node->right, id);
}

// Libérer l'arbre
static void free_tree(Node *node) {
    if (!node) return;
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

// Fonctions publiques
void add_book(int id) {
    root = insert(root, id);
}

int contains_book(int id) {
    return search(root, id);
}

void reset_lib() {
    free_tree(root);
    root = NULL;
}
