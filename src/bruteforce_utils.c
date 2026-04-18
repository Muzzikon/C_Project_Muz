// Эталонные реализации на полном переборе для проверки KD-Tree.
#include <float.h>

#include "bruteforce_utils.h"

// Ищет ближайшую точку полным перебором.
Point brute_force_nearest(Point *points, int count, Point target) {
    Point best = {DBL_MAX, DBL_MAX, DBL_MAX};
    double best_dist = DBL_MAX;

    for (int i = 0; i < count; i++) {
        double dx = points[i].x - target.x;
        double dy = points[i].y - target.y;
        double dz = points[i].z - target.z;
        double dist = dx * dx + dy * dy + dz * dz;

        if (dist < best_dist) {
            best_dist = dist;
            best = points[i];
        }
    }

    return best;
}

// Ищет все точки в диапазоне полным перебором.
int brute_force_range(Point *points, int count, Point lower, Point upper, Point *result) {
    int found = 0;

    for (int i = 0; i < count; i++) {
        if (points[i].x >= lower.x && points[i].x <= upper.x &&
            points[i].y >= lower.y && points[i].y <= upper.y &&
            points[i].z >= lower.z && points[i].z <= upper.z) {
            result[found] = points[i];
            found++;
        }
    }

    return found;
}

// Сравнивает точки для сортировки результатов.
int compare_points(const void *a, const void *b) {
    const Point *p1 = (const Point *)a;
    const Point *p2 = (const Point *)b;

    if (p1->x < p2->x) return -1;
    if (p1->x > p2->x) return 1;
    if (p1->y < p2->y) return -1;
    if (p1->y > p2->y) return 1;
    if (p1->z < p2->z) return -1;
    if (p1->z > p2->z) return 1;

    return 0;
}

// Проверяет совпадение двух массивов точек.
int points_equal(Point *a, Point *b, int count) {
    for (int i = 0; i < count; i++) {
        if (a[i].x != b[i].x || a[i].y != b[i].y || a[i].z != b[i].z) {
            return 0;
        }
    }

    return 1;
}