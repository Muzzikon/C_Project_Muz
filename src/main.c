#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>

#include "kd_tree.h"

int parse_point(const char *text, Point *point) {
    return sscanf(text, "%lf,%lf", &point->x, &point->y) == 2;
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
        if (parse_point(line, &point)) {
            *root = insert(*root, point, 0);
            count++;
        }
    }

    fclose(file);
    return count;
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

        if (argc < 4) {
            printf("Для -kd_nearest нужно передать точку-запрос, например 1.0,2.0\n");
            return 1;
        }

        if (!parse_point(argv[3], &target)) {
            printf("Неверный формат точки: %s\n", argv[3]);
            return 1;
        }

        nearest = nearest_neighbor(root, target, 0);
        if (nearest.x == DBL_MAX && nearest.y == DBL_MAX) {
            printf("Не найдено ближайшего соседа для точки (%lf, %lf)\n", target.x, target.y);
        }
        else {
            printf("Ближайший сосед к точке (%lf, %lf) — это точка (%lf, %lf)\n", target.x, target.y, nearest.x, nearest.y);
        }
    }
    else if (strcmp(argv[2], "-kd_range") == 0) {
        printf("Для -kd_range нужно передать диапазон, например: 1.0,2.0 3.0,4.0\n");
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
