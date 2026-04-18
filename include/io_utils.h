#ifndef IO_UTILS_H
#define IO_UTILS_H

#include "kd_tree.h"

// Разбор точки из аргумента CLI в формате x,y или x,y,z.
int parse_query_point(const char *text, Point *point);

// Формирует имя выходного файла для результата DBSCAN.
char *make_dbscan_output_filename(const char *input_filename);
// Формирует имя выходного файла для результата range query.
char *make_range_output_filename(const char *input_filename);
// Формирует имя выходного файла после вставки новой точки.
char *make_insert_output_filename(const char *input_filename);

// Загружает все точки из CSV в динамический массив.
PointArray load_points_array_from_csv(const char *filename);

// Сохраняет массив точек в CSV-файл формата x,y,z.
int save_points_csv(const char *filename, Point *points, int count);

#endif