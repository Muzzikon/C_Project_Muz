// Интерфейс алгоритма DBSCAN для кластеризации 3D-точек.
// Здесь описаны константы состояний точек и структура результата.
#ifndef DBSCAN_H
#define DBSCAN_H

#include "kd_tree.h"

// Точка ещё не была обработана алгоритмом.
#define UNVISITED -2
// Точка помечена как шум и не принадлежит ни одному кластеру.
#define NOISE -1

// Результат работы DBSCAN: метки точек, число кластеров и шумовых точек.
typedef struct {
    int *labels;
    int cluster_count;
    int noise_count;
} DBSCANResult;

// Запуск алгоритма, освобождение результата и краткий вывод статистики.
DBSCANResult dbscan(Point *points, int count, double eps, int min_pts);
void free_dbscan_result(DBSCANResult *result);
void print_dbscan_summary(const DBSCANResult *result, int point_count);

#endif
