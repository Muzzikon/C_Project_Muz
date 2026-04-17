// Реализация вспомогательных функций проекта.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#include "app_utils.h"

// Разбор одной точки из строки CSV-файла.
static int parse_csv_point(const char *text, Point *point) {
    return sscanf(text, " %lf , %lf , %lf", &point->x, &point->y, &point->z) == 3;
}

// Проверка существования файла.
static int file_exists(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file != NULL) {
        fclose(file);
        return 1;
    }

    return 0;
}

// Разбор точки из аргумента командной строки.
// Ожидается формат: x,y,z
int parse_query_point(const char *text, Point *point) {
    int pos = 0;

    if (sscanf(text, " %lf , %lf , %lf %n", &point->x, &point->y, &point->z, &pos) != 3) {
        return 0;
    }

    while (text[pos] == ' ' || text[pos] == '\n' || text[pos] == '\r' || text[pos] == '\t') {
        pos++;
    }

    return text[pos] == '\0';
}

// Подбирает имя файла для результата DBSCAN.
char *make_dbscan_output_filename(const char *input_filename) {
    const char *base;
    const char *dot;
    size_t name_len;
    char *output_filename;
    int counter = 0;

    if (input_filename == NULL) {
        return NULL;
    }

    base = strrchr(input_filename, '/');
    if (base != NULL) {
        base++;
    }
    else {
        base = input_filename;
    }

    dot = strrchr(base, '.');
    if (dot != NULL) {
        name_len = (size_t)(dot - base);
    }
    else {
        name_len = strlen(base);
    }

    output_filename = malloc(name_len + strlen("_dbscan.csv") + 1);
    if (output_filename == NULL) {
        return NULL;
    }

    memcpy(output_filename, base, name_len);
    output_filename[name_len] = '\0';
    strcat(output_filename, "_dbscan.csv");

    if (!file_exists(output_filename)) {
        return output_filename;
    }

    free(output_filename);

    while (1) {
        int needed = snprintf(NULL, 0, "%.*s_dbscan(%d).csv", (int)name_len, base, counter + 1);
        output_filename = malloc((size_t)needed + 1);

        if (output_filename == NULL) {
            return NULL;
        }

        snprintf(output_filename, (size_t)needed + 1, "%.*s_dbscan(%d).csv", (int)name_len, base, counter + 1);

        if (!file_exists(output_filename)) {
            return output_filename;
        }

        free(output_filename);
        counter++;
    }
}

// Подбирает имя файла для результата диапазонного поиска.
char *make_range_output_filename(const char *input_filename) {
    const char *base;
    const char *dot;
    size_t name_len;
    char *output_filename;
    int counter = 0;

    if (input_filename == NULL) {
        return NULL;
    }

    base = strrchr(input_filename, '/');
    if (base != NULL) {
        base++;
    }
    else {
        base = input_filename;
    }

    dot = strrchr(base, '.');
    if (dot != NULL) {
        name_len = (size_t)(dot - base);
    }
    else {
        name_len = strlen(base);
    }

    output_filename = malloc(name_len + strlen("_range.csv") + 1);
    if (output_filename == NULL) {
        return NULL;
    }

    memcpy(output_filename, base, name_len);
    output_filename[name_len] = '\0';
    strcat(output_filename, "_range.csv");

    if (!file_exists(output_filename)) {
        return output_filename;
    }

    free(output_filename);

    while (1) {
        int needed = snprintf(NULL, 0, "%.*s_range(%d).csv", (int)name_len, base, counter + 1);
        output_filename = malloc((size_t)needed + 1);

        if (output_filename == NULL) {
            return NULL;
        }

        snprintf(output_filename, (size_t)needed + 1, "%.*s_range(%d).csv", (int)name_len, base, counter + 1);

        if (!file_exists(output_filename)) {
            return output_filename;
        }

        free(output_filename);
        counter++;
    }
}

// Чтение всех точек из CSV в динамический массив.
// Массив потом используется и для KD-дерева, и для brute force, и для DBSCAN.
PointArray load_points_array_from_csv(const char *filename) {
    FILE *file = fopen(filename, "r");
    char line[256];
    PointArray arr;
    int capacity = 1024;

    arr.points = NULL;
    arr.count = -1;

    if (file == NULL) {
        return arr;
    }

    arr.points = malloc(capacity * sizeof(Point));
    if (arr.points == NULL) {
        fclose(file);
        return arr;
    }

    arr.count = 0;
    
    while (fgets(line, sizeof(line), file) != NULL) {
        Point point;

        if (parse_csv_point(line, &point)) {
            if (arr.count >= capacity) {
                capacity *= 2;
                // Увеличиваем массив, если текущей ёмкости уже не хватает.
                Point *tmp = realloc(arr.points, capacity * sizeof(Point));
                if (tmp == NULL) {
                    free(arr.points);
                    fclose(file);
                    arr.points = NULL;
                    arr.count = -1;
                    return arr;
                }
                arr.points = tmp;
            }

            arr.points[arr.count] = point;
            arr.count++;
        }
    }

    fclose(file);
    return arr;
}

// Поиск ближайшей точки полным перебором.
// Используется для проверки корректности результата KD-дерева.
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

// Поиск точек в диапазоне полным перебором.
// Нужен как эталон для проверки диапазонного запроса KD-дерева.
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

// Сравнение двух точек для сортировки результатов перед проверкой совпадения.
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

// Проверка, что два массива точек полностью совпадают по координатам.
int points_equal(Point *a, Point *b, int count) {
    for (int i = 0; i < count; i++) {
        if (a[i].x != b[i].x || a[i].y != b[i].y || a[i].z != b[i].z) {
            return 0;
        }
    }
    return 1;
}

// Записывает найденные точки в CSV.
int save_points_csv(const char *filename, Point *points, int count) {
    FILE *file;

    if (filename == NULL || points == NULL || count < 0) {
        return 0;
    }

    file = fopen(filename, "w");
    if (file == NULL) {
        return 0;
    }

    fprintf(file, "x,y,z\n");

    for (int i = 0; i < count; i++) {
        fprintf(file, "%.6f,%.6f,%.6f\n",
                points[i].x,
                points[i].y,
                points[i].z);
    }

    fclose(file);
    return 1;
}