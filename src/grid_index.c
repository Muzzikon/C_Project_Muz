// Реализация пространственной сетки для ускорения поиска соседей.
// Используется в DBSCAN вместо полного перебора всех точек.
#include <stdlib.h>
#include <math.h>

#include "grid_index.h"

// Перевод вещественной координаты точки в индекс ячейки сетки.
static int grid_coord(double value, double cell_size) {
    return (int)floor(value / cell_size);
}

// Хеш-функция для трёх целочисленных координат ячейки.
static unsigned long hash_coords(int cx, int cy, int cz) {
    unsigned long x = (unsigned long)(unsigned int)cx;
    unsigned long y = (unsigned long)(unsigned int)cy;
    unsigned long z = (unsigned long)(unsigned int)cz;

    return x * 73856093u ^ y * 19349663u ^ z * 83492791u;
}

// Поиск уже существующей ячейки в хеш-таблице.
static GridCell *find_cell(const GridIndex *grid, int cx, int cy, int cz) {
    unsigned long hash = hash_coords(cx, cy, cz);
    int bucket = (int)(hash % (unsigned long)grid->bucket_count);

    GridCell *cell = grid->buckets[bucket];
    while (cell != NULL) {
        if (cell->cx == cx && cell->cy == cy && cell->cz == cz) {
            return cell;
        }
        cell = cell->next;
    }

    return NULL;
}

// Возвращает существующую ячейку или создаёт новую, если её ещё нет.
static GridCell *get_or_create_cell(GridIndex *grid, int cx, int cy, int cz) {
    unsigned long hash = hash_coords(cx, cy, cz);
    int bucket = (int)(hash % (unsigned long)grid->bucket_count);

    GridCell *cell = grid->buckets[bucket];
    while (cell != NULL) {
        if (cell->cx == cx && cell->cy == cy && cell->cz == cz) {
            return cell;
        }
        cell = cell->next;
    }

    GridCell *new_cell = malloc(sizeof(GridCell));
    if (new_cell == NULL) {
        return NULL;
    }

    new_cell->cx = cx;
    new_cell->cy = cy;
    new_cell->cz = cz;
    new_cell->points = NULL;
    new_cell->next = grid->buckets[bucket];
    grid->buckets[bucket] = new_cell;

    return new_cell;
}

// Построение пространственной сетки по массиву точек.
// Каждая точка попадает в ячейку, определяемую её координатами.
GridIndex build_grid_index(Point *points, int count, double cell_size) {
    GridIndex grid;
    grid.buckets = NULL;
    grid.bucket_count = 0;
    grid.cell_size = cell_size;

    if (points == NULL || count <= 0 || cell_size <= 0.0) {
        return grid;
    }

    grid.bucket_count = count * 2 + 1;
    if (grid.bucket_count < 1024) {
        grid.bucket_count = 1024;
    }

    grid.buckets = calloc((size_t)grid.bucket_count, sizeof(GridCell *));
    if (grid.buckets == NULL) {
        grid.bucket_count = 0;
        return grid;
    }

    // Раскладываем все точки по ячейкам сетки и сохраняем их индексы.
    for (int i = 0; i < count; i++) {
        int cx = grid_coord(points[i].x, cell_size);
        int cy = grid_coord(points[i].y, cell_size);
        int cz = grid_coord(points[i].z, cell_size);

        GridCell *cell = get_or_create_cell(&grid, cx, cy, cz);
        if (cell == NULL) {
            free_grid_index(&grid);
            return grid;
        }

        GridPointNode *node = malloc(sizeof(GridPointNode));
        if (node == NULL) {
            free_grid_index(&grid);
            return grid;
        }

        node->point_index = i;
        node->next = cell->points;
        cell->points = node;
    }

    return grid;
}

// Освобождение памяти всех ячеек и списков точек внутри сетки.
void free_grid_index(GridIndex *grid) {
    if (grid == NULL || grid->buckets == NULL) {
        return;
    }

    for (int i = 0; i < grid->bucket_count; i++) {
        GridCell *cell = grid->buckets[i];

        while (cell != NULL) {
            GridCell *next_cell = cell->next;

            GridPointNode *node = cell->points;
            while (node != NULL) {
                GridPointNode *next_node = node->next;
                free(node);
                node = next_node;
            }

            free(cell);
            cell = next_cell;
        }
    }

    free(grid->buckets);
    grid->buckets = NULL;
    grid->bucket_count = 0;
    grid->cell_size = 0.0;
}

// Поиск соседей точки через пространственную сетку.
// Проверяются только текущая и соседние ячейки, после чего идёт точная фильтрация по расстоянию.
int grid_region_query(const GridIndex *grid, Point *points, int point_index, double eps_sq, int *neighbors) {
    if (grid == NULL || grid->buckets == NULL || points == NULL || neighbors == NULL || eps_sq < 0.0) {
        return 0;
    }

    Point center = points[point_index];

    int cx = grid_coord(center.x, grid->cell_size);
    int cy = grid_coord(center.y, grid->cell_size);
    int cz = grid_coord(center.z, grid->cell_size);

    int found = 0;

    // Для 3D-случая достаточно проверить 27 ячеек: текущую и соседние по всем осям.
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            for (int dz = -1; dz <= 1; dz++) {
                GridCell *cell = find_cell(grid, cx + dx, cy + dy, cz + dz);
                if (cell == NULL) {
                    continue;
                }

                GridPointNode *node = cell->points;
                while (node != NULL) {
                    int idx = node->point_index;

                    double px = points[idx].x - center.x;
                    double py = points[idx].y - center.y;
                    double pz = points[idx].z - center.z;
                    double dist_sq = px * px + py * py + pz * pz;

                    if (dist_sq <= eps_sq) {
                        neighbors[found] = idx;
                        found++;
                    }

                    node = node->next;
                }
            }
        }
    }

    return found;
}