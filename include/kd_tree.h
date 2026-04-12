// Базовые структуры и функции для работы с 3D KD-деревом.
// Используются для вставки точек, поиска ближайшего соседа и диапазонных запросов.
#ifndef KD_TREE_H
#define KD_TREE_H

// Точка в трёхмерном пространстве.
typedef struct Point {
    double x, y, z;  // Для 3D-данных
} Point;

// Динамический массив точек, загружаемых из CSV-файла.
typedef struct {
    Point *points;
    int count;
} PointArray;

// Узел KD-дерева: хранит координаты точки, её индекс в исходном массиве
// и ссылки на левое и правое поддерево.
typedef struct Node {
    Point point;
    int index;
    struct Node *left, *right;
} Node;

// Декларации функций
// Функции построения и обхода KD-дерева.
Node* insert(Node* root, Point point, int index, int depth);
void print_tree(Node* root);
Point nearest_neighbor(Node* root, Point target, int depth);

void range_query(Node* root, Point lower, Point upper, int depth, Point* result, int* count);
void range_query_indices(Node* root, Point lower, Point upper, int depth, int* result, int* count);

void free_tree(Node* root);

#endif
