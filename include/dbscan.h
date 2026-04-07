#ifndef DBSCAN_H
#define DBSCAN_H

#include "kd_tree.h"

#define UNVISITED -2
#define NOISE -1

typedef struct {
    int *labels;
    int cluster_count;
    int noise_count;
} DBSCANResult;

DBSCANResult dbscan(Point *points, int count, double eps, int min_pts);
void free_dbscan_result(DBSCANResult *result);
void print_dbscan_summary(const DBSCANResult *result, int point_count);

#endif
