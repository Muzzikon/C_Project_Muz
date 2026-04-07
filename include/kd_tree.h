#ifndef KD_TREE_H
#define KD_TREE_H

typedef struct Point {
    double x, y, z;  // Для 3D-данных
} Point;

typedef struct Node {
    Point point;
    struct Node *left, *right;
} Node;

// Декларации функций
Node* insert(Node* root, Point point, int depth);
void print_tree(Node* root);
Point nearest_neighbor(Node* root, Point target, int depth);
void range_query(Node* root, Point lower, Point upper, int depth, Point* result, int* count);

#endif
