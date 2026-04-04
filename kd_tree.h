#ifndef KD_TREE_H
#define KD_TREE_H

typedef struct Point {
    	double x, y;  // Для 2D-данных
} Point;

typedef struct Node {
    	Point point;
    	struct Node *left, *right;
} Node;

// Декларации функций
Node* insert(Node* root, Point point, int depth);
void print_tree(Node* root);

#endif
