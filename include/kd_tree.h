#ifndef KD_TREE_H
#define KD_TREE_H

typedef struct Point {
    double x, y, z;  // Для 3D-данных
} Point;

typedef struct {
    Point *points;
    int count;
} PointArray;

typedef struct Node {
    Point point;
    int index;
    struct Node *left, *right;
} Node;

// Декларации функций
Node* insert(Node* root, Point point, int index, int depth);
void print_tree(Node* root);
Point nearest_neighbor(Node* root, Point target, int depth);

void range_query(Node* root, Point lower, Point upper, int depth, Point* result, int* count);
void range_query_indices(Node* root, Point lower, Point upper, int depth, int* result, int* count);

void free_tree(Node* root);

Point brute_force_nearest(Point *points, int count, Point target);
int brute_force_range(Point *points, int count, Point lower, Point upper, Point *result);

#endif
