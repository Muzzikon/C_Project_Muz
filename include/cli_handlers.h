#ifndef CLI_HANDLERS_H
#define CLI_HANDLERS_H

#include "kd_tree.h"

// Обрабатывает вставку точки и сохранение обновлённого CSV.
int handle_kd_insert(const char *input_filename, int argc, char *argv[], Node **root, PointArray *data);

// Обрабатывает поиск ближайшей точки с проверкой brute force.
int handle_kd_nearest(Node *root, PointArray *data, int argc, char *argv[]);

// Обрабатывает диапазонный поиск и сохранение результата в CSV.
int handle_kd_range(const char *input_filename, Node *root, PointArray *data, int argc, char *argv[]);

// Обрабатывает запуск DBSCAN и сохранение размеченного CSV.
int handle_dbscan(const char *input_filename, PointArray *data, int argc, char *argv[]);

#endif