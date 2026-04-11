#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>

#include "kd_tree.h"
#include "dbscan.h"

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

int parse_csv_point(const char *text, Point *point) {
    return sscanf(text, " %lf , %lf , %lf", &point->x, &point->y, &point->z) == 3;
}

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

int points_equal(Point *a, Point *b, int count) {
    for (int i = 0; i < count; i++) {
        if (a[i].x != b[i].x || a[i].y != b[i].y || a[i].z != b[i].z) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Использование: %s <файл> <операция>\n", argv[0]);

		return 1;
	}

	printf("Файл: %s\n", argv[1]);
	printf("Операция: %s\n", argv[2]);

    PointArray data = load_points_array_from_csv(argv[1]);
    if (data.count < 0) {
        return 1;
    }

    Node *root = NULL;
    for (int i = 0; i < data.count; i++) {
        root = insert(root, data.points[i], i, 0);
    }

    printf("Загружено точек: %d\n", data.count);

    if (strcmp(argv[2], "-kd_insert") == 0) {
        printf("Дерево построено из CSV-файла.\n");
        print_tree(root);
    }
    else if (strcmp(argv[2], "-kd_nearest") == 0) {
        Point target;
        Point nearest;
        Point brute;
        clock_t kd_start, kd_end;
        clock_t brute_start, brute_end;
        double kd_time;
        double brute_time;

        if (argc < 4) {
            printf("Для -kd_nearest нужно передать точку-запрос, например 1.0,2.0,3.0\n");

            free_tree(root);
            free(data.points);
            return 1;
        }

        if (!parse_query_point(argv[3], &target)) {
            printf("Неверный формат точки: %s\n", argv[3]);
            return 1;
        }

        kd_start = clock();
        nearest = nearest_neighbor(root, target, 0);
        kd_end = clock();

        brute_start = clock();
        brute = brute_force_nearest(data.points, data.count, target);
        brute_end = clock();

        kd_time = ((double)(kd_end - kd_start)) / CLOCKS_PER_SEC;
        brute_time = ((double)(brute_end - brute_start)) / CLOCKS_PER_SEC;

        if (nearest.x == DBL_MAX && nearest.y == DBL_MAX) {
            printf("KD-Tree: ближайший сосед не найден\n");
        }
        else {
            printf("KD-Tree: ближайший сосед к точке (%lf, %lf, %lf) — это точка (%lf, %lf, %lf)\n",
            target.x, target.y, target.z, nearest.x, nearest.y, nearest.z);
        }

        if (brute.x == DBL_MAX && brute.y == DBL_MAX) {
            printf("Brute force: ближайший сосед не найден\n");
        }
        else {
            printf("Brute force: ближайший сосед к точке (%lf, %lf, %lf) — это точка (%lf, %lf, %lf)\n",
            target.x, target.y, target.z, brute.x, brute.y, brute.z);
        }

        printf("Время KD-Tree: %.6f сек.\n", kd_time);
        printf("Время Brute force: %.6f сек.\n", brute_time);
    }
    else if (strcmp(argv[2], "-kd_range") == 0) {
        Point lower;
        Point upper;
        Point kd_result[50000];
        Point brute_result[50000];
        int kd_count = 0;
        int brute_count = 0;
        clock_t kd_start, kd_end;
        clock_t brute_start, brute_end;
        double kd_time;
        double brute_time;

        if (argc < 5) {
            printf("Для -kd_range нужно передать диапазон, например: 1.0,2.0,3.0 4.0,5.0,6.0\n");
            return 1;
        }

        if (!parse_query_point(argv[3], &lower) || !parse_query_point(argv[4], &upper)) {
            printf("Неверный формат диапазона\n");
            return 1;
        }

        if (lower.x > upper.x || lower.y > upper.y || lower.z > upper.z) {
            printf("Неверный диапазон: нижняя граница должна быть меньше или равна верхней\n");
            return 1;
        }

        kd_start = clock();
        range_query(root, lower, upper, 0, kd_result, &kd_count);
        kd_end = clock();

        brute_start = clock();
        brute_count = brute_force_range(data.points, data.count, lower, upper, brute_result);
        brute_end = clock();

        kd_time = ((double)(kd_end - kd_start)) / CLOCKS_PER_SEC;
        brute_time = ((double)(brute_end - brute_start)) / CLOCKS_PER_SEC;

        printf("KD-Tree: найдено точек в диапазоне: %d\n", kd_count);
        printf("Brute force: найдено точек в диапазоне: %d\n", brute_count);

        printf("Время KD-Tree: %.6f сек.\n", kd_time);
        printf("Время Brute force: %.6f сек.\n", brute_time);

        if (kd_count == brute_count) {
            qsort(kd_result, kd_count, sizeof(Point), compare_points);
            qsort(brute_result, brute_count, sizeof(Point), compare_points);

            if (points_equal(kd_result, brute_result, kd_count)) {
                printf("Результаты KD-Tree и Brute force полностью совпадают\n");
            }
            else {
                printf("Количество совпадает, но сами точки различаются\n");
            }
        }
        else {
            printf("Количество найденных точек различается\n");
        }
    }
    else if (strcmp(argv[2], "-cmeans") == 0) {
        printf("Операция Fuzzy C-means будет реализована позже.\n");
    }
    else if (strcmp(argv[2], "-dbscan") == 0) {
        double eps;
        int min_pts;
        DBSCANResult result;
        clock_t dbscan_start, dbscan_end;
        double dbscan_time;

        if (argc < 5) {
            printf("Для -dbscan нужно передать eps и min_pts, например: 0.5 5\n");

            free_tree(root);
            free(data.points);

            return 1;
        }

        eps = atof(argv[3]);
        min_pts = atoi(argv[4]);

        if (eps <= 0.0 || min_pts <= 0) {
            printf("Неверные параметры DBSCAN: eps > 0, min_pts > 0\n");

            free_tree(root);
            free(data.points);

            return 1;
        }

        dbscan_start = clock();
        result = dbscan(data.points, data.count, eps, min_pts);
        dbscan_end = clock();

        if (result.labels == NULL) {
            printf("Не удалось выполнить DBSCAN\n");

            free_tree(root);
            free(data.points);

            return 1;
        }

        dbscan_time = ((double)(dbscan_end - dbscan_start)) / CLOCKS_PER_SEC;

        print_dbscan_summary(&result, data.count);
        printf("Время DBSCAN: %.6f сек.\n", dbscan_time);

        free_dbscan_result(&result);
    }
    else {
        printf("Неизвестная операция: %s\n", argv[2]);
        return 1;
    }

    free_tree(root);
    free(data.points);

	return 0;
}
