#ifndef APP_UTILS_H
#define APP_UTILS_H

#include "kd_tree.h"

int parse_query_point(const char *text, Point *point);
char *make_dbscan_output_filename(const char *input_filename);
PointArray load_points_array_from_csv(const char *filename);

Point brute_force_nearest(Point *points, int count, Point target);
int brute_force_range(Point *points, int count, Point lower, Point upper, Point *result);

int compare_points(const void *a, const void *b);
int points_equal(Point *a, Point *b, int count);

#endif