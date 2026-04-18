#ifndef BRUTEFORCE_UTILS_H
#define BRUTEFORCE_UTILS_H

#include "kd_tree.h"

// Эталонный поиск ближайшей точки полным перебором.
Point brute_force_nearest(Point *points, int count, Point target);

// Эталонный диапазонный поиск полным перебором.
int brute_force_range(Point *points, int count, Point lower, Point upper, Point *result);

// Сравнение точек для сортировки результатов.
int compare_points(const void *a, const void *b);

// Проверка, что два массива точек совпадают.
int points_equal(Point *a, Point *b, int count);

#endif