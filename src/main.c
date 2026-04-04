#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <kd_tree.h>

// Функция для обработки операций
void process_operation(Node** root, char *operation, Point point) {
   	if (strcmp(operation, "-kd_insert") == 0) {
        *root = insert(*root, point, 0);
        printf("Точка (%lf, %lf) вставлена в дерево.\n", point.x, point.y);
    }
	else if (strcmp(operation, "-kd_nearest") == 0) {
        printf("Операция поиска ближайшего соседа будет реализована позже.\n");
    }
	else if (strcmp(operation, "-cmeans") == 0) {
        printf("Операция Fuzzy C-means будет реализована позже.\n");
    }
	else if (strcmp(operation, "-dbscan") == 0) {
        printf("Операция DBSCAN будет реализована позже.\n");
    }
	else {
        printf("Неизвестная операция: %s\n", operation);
    }
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Использование: %s <файл> <операция>\n", argv[0]);
		return 1;
	}

	printf("Файл: %s\n", argv[1]);
	printf("Операция: %s\n", argv[2]);

	Node* root = NULL;

   	// Пример точек для вставки, можно заменить на точки из файла
   	Point p1 = {3.0, 6.0};
   	Point p2 = {2.0, 7.0};
   	Point p3 = {17.0, 15.0};

    // Применяем операцию в зависимости от аргумента
   	process_operation(&root, argv[2], p1);
   	process_operation(&root, argv[2], p2);
   	process_operation(&root, argv[2], p3);

	// Вывод дерева
	print_tree(root);

	return 0;
}
