#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>

#include "kd_tree.h"

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

int load_points_from_csv(const char *filename, Node **root) {
    FILE *file = fopen(filename, "r");
    char line[256];
    int count = 0;

    if (file == NULL) {
        printf("Не удалось открыть файл: %s\n", filename);
        return -1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        Point point;
        if (parse_csv_point(line, &point)) {
            *root = insert(*root, point, 0);
            count++;
        }
    }

    fclose(file);
    return count;
}

Point brute_force_nearest_from_csv(const char *filename, Point target) {
    FILE *file = fopen(filename, "r");
    char line[256];
    Point best = {DBL_MAX, DBL_MAX};
    double best_dist = DBL_MAX;

    if (file == NULL) {
        return best;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        Point p;
        if (parse_csv_point(line, &p)) {
            double dx = p.x - target.x;
            double dy = p.y - target.y;
            double dz = p.z - target.z;
            double dist = dx * dx + dy * dy + dz * dz;

            if (dist < best_dist) {
                best_dist = dist;
                best = p;
            }
        }
    }

    fclose(file);
    return best;
}

int brute_force_range_from_csv(const char *filename, Point lower, Point upper, Point *result) {
    FILE *file = fopen(filename, "r");
    char line[256];
    int count = 0;

    if (file == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        Point p;
        if (parse_csv_point(line, &p)) {
            if (p.x >= lower.x && p.x <= upper.x &&
                p.y >= lower.y && p.y <= upper.y &&
                p.z >= lower.z && p.z <= upper.z) {
                result[count] = p;
                count++;
            }
        }
    }

    fclose(file);
    return count;
}

int compare_points(const void *a, const void *b) {
    const Point *p1 = (const Point *)a;
    const Point *p2 = (const Point *)b;

    if (p1->x < p2->x) return -1;
    if (p1->x > p2->x) return 1;
    if (p1->y < p2->y) return -1;
    if (p1->y > p2->y) return 1;
    return 0;
}

int points_equal(Point *a, Point *b, int count) {
    for (int i = 0; i < count; i++) {
        if (a[i].x != b[i].x || a[i].y != b[i].y) {
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

	Node* root = NULL;
	int loaded = load_points_from_csv(argv[1], &root);

    if (loaded < 0) {
        return 1;
    }

    printf("Загружено точек: %d\n", loaded);

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
            printf("Для -kd_nearest нужно передать точку-запрос, например 1.0,2.0\n");
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
        brute = brute_force_nearest_from_csv(argv[1], target);
        brute_end = clock();

        kd_time = ((double)(kd_end - kd_start)) / CLOCKS_PER_SEC;
        brute_time = ((double)(brute_end - brute_start)) / CLOCKS_PER_SEC;

        if (nearest.x == DBL_MAX && nearest.y == DBL_MAX) {
            printf("KD-Tree: ближайший сосед не найден\n");
        }
        else {
            printf("KD-Tree: ближайший сосед к точке (%lf, %lf, %lf) — это точка (%lf, %lf, %lf)\n",
            target.x, target.y, nearest.x, nearest.y, nearest.z);
        }

        if (brute.x == DBL_MAX && brute.y == DBL_MAX) {
            printf("Brute force: ближайший сосед не найден\n");
        }
        else {
            printf("Brute force: ближайший сосед к точке (%lf, %lf, %lf) — это точка (%lf, %lf, %lf)\n",
            target.x, target.y, brute.x, brute.y, brute.z);
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
            printf("Для -kd_range нужно передать диапазон, например: 1.0,2.0 3.0,4.0\n");
            return 1;
        }

        if (!parse_query_point(argv[3], &lower) || !parse_query_point(argv[4], &upper)) {
            printf("Неверный формат диапазона\n");
            return 1;
        }

        if (lower.x > upper.x || lower.y > upper.y) {
            printf("Неверный диапазон: нижняя граница должна быть меньше или равна верхней\n");
            return 1;
        }

        kd_start = clock();
        range_query(root, lower, upper, 0, kd_result, &kd_count);
        kd_end = clock();

        brute_start = clock();
        brute_count = brute_force_range_from_csv(argv[1], lower, upper, brute_result);
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
        printf("Операция DBSCAN будет реализована позже.\n");
    }
    else {
        printf("Неизвестная операция: %s\n", argv[2]);
        return 1;
    }

	return 0;
}
