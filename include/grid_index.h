// Пространственная сетка для ускорения поиска соседей в DBSCAN.
// Пространство разбивается на ячейки, внутри которых хранятся индексы точек.
#ifndef GRID_INDEX_H
#define GRID_INDEX_H

#include "kd_tree.h"

// Элемент связного списка индексов точек внутри одной ячейки.
typedef struct GridPointNode {
    int point_index;
    struct GridPointNode *next;
} GridPointNode;

// Ячейка сетки с целочисленными координатами и набором точек в этой ячейке.
typedef struct GridCell {
    int cx, cy, cz;
    GridPointNode *points;
    struct GridCell *next;
} GridCell;

// Вся сетка: массив корзин, число корзин и размер одной ячейки.
typedef struct {
    GridCell **buckets;
    int bucket_count;
    double cell_size;
} GridIndex;

// Построение сетки, освобождение памяти и поиск соседей через сетку.
GridIndex build_grid_index(Point *points, int count, double cell_size);
void free_grid_index(GridIndex *grid);
int grid_region_query(const GridIndex *grid, Point *points, int point_index, double eps_sq, int *neighbors);

#endif