// Главный файл программы.
// Отвечает за загрузку точек, запуск режимов KD-дерева и DBSCAN, а также замер времени.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>

#include "kd_tree.h"
#include "dbscan.h"
#include "app_utils.h"

// Точка входа в программу.
// Поддерживает режимы: построение KD-дерева, nearest, range и DBSCAN.
int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Использование: %s <файл> <операция>\n", argv[0]);

		return 1;
	}

	printf("Файл: %s\n", argv[1]);
	printf("Операция: %s\n", argv[2]);

    // Сначала загружаем все точки в память, затем по ним строим KD-дерево.
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
    // Режим поиска ближайшего соседа с сравнением KD-Tree и brute force.
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
    // Режим диапазонного поиска с проверкой совпадения результатов.
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
    // Режим кластеризации DBSCAN с выводом краткой статистики и времени работы.
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

        char *output_filename = make_dbscan_output_filename(argv[1]);

        if (output_filename == NULL) {
            printf("Не удалось сформировать имя выходного файла\n");
        }
        else {
            if (save_dbscan_result_csv(output_filename, data.points, data.count, &result)) {
                printf("Результат DBSCAN сохранён в %s\n", output_filename);
            }
            else {
                printf("Не удалось сохранить результат DBSCAN в CSV\n");
            }
    
            free(output_filename);
        }

        free_dbscan_result(&result);
    }
    else {
        printf("Неизвестная операция: %s\n", argv[2]);
        return 1;
    }

    // Освобождаем память перед завершением программы.
    free_tree(root);
    free(data.points);

	return 0;
}
