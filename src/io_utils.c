// Вспомогательные функции для ввода-вывода и разбора точек.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io_utils.h"

// Универсальный разбор точки из строки.
static int parse_point_text(const char *text, Point *point) {
    int pos = 0;
    double x;
    double y;
    double z;

    if (sscanf(text, " %lf , %lf , %lf %n", &x, &y, &z, &pos) == 3) {
        point->x = x;
        point->y = y;
        point->z = z;
    }
    else if (sscanf(text, " %lf , %lf %n", &x, &y, &pos) == 2) {
        point->x = x;
        point->y = y;
        point->z = 0.0;
    }
    else {
        return 0;
    }

    while (text[pos] == ' ' || text[pos] == '\n' || text[pos] == '\r' || text[pos] == '\t') {
        pos++;
    }

    return text[pos] == '\0';
}

// Разбирает строку CSV как точку.
static int parse_csv_point(const char *text, Point *point) {
    return parse_point_text(text, point);
}

// Разбирает точку из аргумента командной строки.
int parse_query_point(const char *text, Point *point) {
    return parse_point_text(text, point);
}

// Проверяет, существует ли файл с таким именем.
static int file_exists(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file != NULL) {
        fclose(file);
        return 1;
    }

    return 0;
}

// Подбирает уникальное имя выходного CSV-файла.
static char *make_output_filename(const char *input_filename, const char *suffix) {
    const char *base;
    const char *dot;
    size_t name_len;
    char *output_filename;
    int counter = 0;

    if (input_filename == NULL || suffix == NULL) {
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

    while (1) {
        int needed;

        if (counter == 0) {
            needed = snprintf(NULL, 0, "%.*s_%s.csv", (int)name_len, base, suffix);
            output_filename = malloc((size_t)needed + 1);
            if (output_filename == NULL) {
                return NULL;
            }

            snprintf(output_filename, (size_t)needed + 1,
                     "%.*s_%s.csv", (int)name_len, base, suffix);
        }
        else {
            needed = snprintf(NULL, 0, "%.*s_%s(%d).csv",
                              (int)name_len, base, suffix, counter);
            output_filename = malloc((size_t)needed + 1);
            if (output_filename == NULL) {
                return NULL;
            }

            snprintf(output_filename, (size_t)needed + 1,
                     "%.*s_%s(%d).csv", (int)name_len, base, suffix, counter);
        }

        if (!file_exists(output_filename)) {
            return output_filename;
        }

        free(output_filename);
        counter++;
    }
}

char *make_dbscan_output_filename(const char *input_filename) {
    return make_output_filename(input_filename, "dbscan");
}

char *make_range_output_filename(const char *input_filename) {
    return make_output_filename(input_filename, "range");
}

char *make_insert_output_filename(const char *input_filename) {
    return make_output_filename(input_filename, "inserted");
}

// Загружает точки из CSV в динамический массив.
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
            // При необходимости увеличиваем ёмкость массива.
            if (arr.count >= capacity) {
                Point *tmp;

                capacity *= 2;
                tmp = realloc(arr.points, capacity * sizeof(Point));
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

// Сохраняет массив точек в CSV формата x,y,z.
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