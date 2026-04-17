// Вспомогательные функции: парсинг, загрузка CSV,
// brute force-проверки и сохранение результатов.
#ifndef APP_UTILS_H
#define APP_UTILS_H

#include "kd_tree.h"

// Разбор точки из аргумента CLI в формате x,y,z.
int parse_query_point(const char *text, Point *point);
// Формирует имя выходного файла для результата DBSCAN.
char *make_dbscan_output_filename(const char *input_filename);
// Формирует имя выходного файла для результата range query.
char *make_range_output_filename(const char *input_filename);
// Загружает все точки из CSV в динамический массив.
PointArray load_points_array_from_csv(const char *filename);

// Эталонный поиск ближайшей точки полным перебором.
Point brute_force_nearest(Point *points, int count, Point target);
// Эталонный диапазонный поиск полным перебором.
int brute_force_range(Point *points, int count, Point lower, Point upper, Point *result);

// Сравнение точек для сортировки результатов.
int compare_points(const void *a, const void *b);
// Проверка, что два массива точек совпадают.
int points_equal(Point *a, Point *b, int count);
// Сохраняет массив точек в CSV-файл формата x,y,z.
int save_points_csv(const char *filename, Point *points, int count);

#endif