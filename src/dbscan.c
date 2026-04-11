#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "dbscan.h"

static double distance_squared(Point a, Point b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    double dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
}

static int region_query(Node *root, Point *points, int count, int point_index, double eps_sq, int *neighbors) {

    Point target = points[point_index];
    Point lower = {target.x - eps_sq, target.y - eps_sq, target.z - eps_sq};
    Point upper = {target.x + eps_sq, target.y + eps_sq, target.z + eps_sq};

    int found = 0;
    range_query(root, lower, upper, 0, neighbors, &found);

    return found;
}

static void expand_cluster(Point *points, int count, int point_index, int *labels, int cluster_id, double eps_sq, int min_pts) {
    int *queue = malloc(count * sizeof(int));
    int *neighbors = malloc(count * sizeof(int));
    int *in_queue = calloc(count, sizeof(int));
    int queue_start = 0;
    int queue_end = 0;
    int neighbor_count;

    if (queue == NULL || neighbors == NULL || in_queue == NULL) {
        free(queue);
        free(neighbors);
        free(in_queue);
        return;
    }

    neighbor_count = region_query(root, points, count, point_index, eps_sq, neighbors);

    labels[point_index] = cluster_id;

    for (int i = 0; i < neighbor_count; i++) {
        int n = neighbors[i];

        if (!in_queue[n]) {
            queue[queue_end] = n;
            queue_end++;
            in_queue[n] = 1;
        }
    }

    while (queue_start < queue_end) {
        int current = queue[queue_start];
        queue_start++;

        if (labels[current] == NOISE) {
            labels[current] = cluster_id;
        }

        if (labels[current] != UNVISITED && labels[current] != NOISE) {
            continue;
        }

        labels[current] = cluster_id;

        int current_neighbor_count = region_query(points, count, current, eps_sq, neighbors);

        if (current_neighbor_count >= min_pts) {
            for (int i = 0; i < current_neighbor_count; i++) {
                int n = neighbors[i];

                if (!in_queue[n] && (labels[n] == UNVISITED || labels[n] == NOISE)) {
                    queue[queue_end] = n;
                    queue_end++;
                    in_queue[n] = 1;
                }
            }
        }
    }

    free(queue);
    free(neighbors);
    free(in_queue);
}

DBSCANResult dbscan(Point *points, int count, double eps, int min_pts) {
    DBSCANResult result;
    double eps_sq = eps * eps;
    int cluster_id = 0;

    result.labels = NULL;
    result.cluster_count = 0;
    result.noise_count = 0;

    if (points == NULL || count <= 0 || eps <= 0.0 || min_pts <= 0) {
        return result;
    }

    result.labels = malloc(count * sizeof(int));
    if (result.labels == NULL) {
        return result;
    }

    for (int i = 0; i < count; i++) {
        result.labels[i] = UNVISITED;
    }

    for (int i = 0; i < count; i++) {
        if (result.labels[i] != UNVISITED) {
            continue;
        }

        int *neighbors = malloc(count * sizeof(int));
        int neighbor_count;

        if (neighbors == NULL) {
            free(result.labels);
            result.labels = NULL;
            result.cluster_count = 0;
            result.noise_count = 0;
            return result;
        }

        neighbor_count = region_query(points, count, i, eps_sq, neighbors);

        if (neighbor_count < min_pts) {
            result.labels[i] = NOISE;
        } else {
            expand_cluster(points, count, i, result.labels, cluster_id, eps_sq, min_pts);
            cluster_id++;
        }

        free(neighbors);
    }

    result.cluster_count = cluster_id;
    result.noise_count = 0;

    for (int i = 0; i < count; i++) {
        if (result.labels[i] == NOISE) {
            result.noise_count++;
        }
    }

    return result;
}

void free_dbscan_result(DBSCANResult *result) {
    if (result == NULL) {
        return;
    }

    free(result->labels);
    result->labels = NULL;
    result->cluster_count = 0;
    result->noise_count = 0;
}

void print_dbscan_summary(const DBSCANResult *result, int point_count) {
    if (result == NULL || result->labels == NULL) {
        printf("Результат DBSCAN недоступен\n");
        return;
    }

    printf("DBSCAN: всего точек: %d\n", point_count);
    printf("DBSCAN: найдено кластеров: %d\n", result->cluster_count);
    printf("DBSCAN: шумовых точек: %d\n", result->noise_count);
}
