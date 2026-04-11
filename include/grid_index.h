#ifndef GRID_INDEX_H
#define GRID_INDEX_H

#include "kd_tree.h"

typedef struct GridPointNode {
    int point_index;
    struct GridPointNode *next;
} GridPointNode;

typedef struct GridCell {
    int cx, cy, cz;
    GridPointNode *points;
    struct GridCell *next;
} GridCell;

typedef struct {
    GridCell **buckets;
    int bucket_count;
    double cell_size;
} GridIndex;

GridIndex build_grid_index(Point *points, int count, double cell_size);
void free_grid_index(GridIndex *grid);
int grid_region_query(const GridIndex *grid, Point *points, int point_index, double eps_sq, int *neighbors);

#endif