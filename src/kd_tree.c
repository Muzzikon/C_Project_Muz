// Реализация 3D KD-дерева.
// Здесь находятся вставка точек, поиск ближайшего соседа, диапазонный поиск и очистка памяти.
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "kd_tree.h"

// Рекурсивная вставка точки в KD-дерево.
// Ось сравнения зависит от глубины: x, y, z, затем снова x.
Node* insert(Node* root, Point point, int index, int depth) {
    if (root == NULL) {
        Node* new_node = (Node*)malloc(sizeof(Node));
        new_node->point = point;
        new_node->index = index;
        new_node->left = new_node->right = NULL;

        return new_node;
    }

    // На каждой глубине выбираем одну из трёх координат для разбиения пространства.
    int cd = depth % 3;

    if (cd == 0) {
        if (point.x < root->point.x) {
            root->left = insert(root->left, point, index, depth + 1);
		}
		else {
            root->right = insert(root->right, point, index, depth + 1);
        }
    }
	else if (cd == 1) {  // Если ось y
        if (point.y < root->point.y) {
            root->left = insert(root->left, point, index, depth + 1);
        }
		else {
            root->right = insert(root->right, point, index, depth + 1);
        }
    }
    else {
        if (point.z < root->point.z) {
            root->left = insert(root->left, point, index, depth + 1);
        }
        else {
            root->right = insert(root->right, point, index, depth + 1);
        }
    }

    return root;
}

// Простой вывод всех точек дерева в глубинном обходе.
void print_tree(Node* root) {
    if (root == NULL) return;
    printf("Point: (%lf, %lf, %lf)\n", root->point.x, root->point.y, root->point.z);
    print_tree(root->left);
    print_tree(root->right);
}

// Квадрат расстояния между двумя точками.
// Используется вместо обычного расстояния, чтобы не вызывать sqrt.
static double distance_squared(Point a, Point b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    double dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
}

// Внутренняя рекурсивная функция поиска ближайшего соседа.
// Сначала идём в более перспективную ветку, затем при необходимости проверяем вторую.
static void nearest_neighbor_recursive(Node* root, Point target, int depth, Point* best_point, double* best_dist) {
    if (root == NULL) {
        return;
    }

    double current_dist = distance_squared(root->point, target);
    if (current_dist < *best_dist) {
        *best_dist = current_dist;
        *best_point = root->point;
    }

    int axis = depth % 3;
    Node* first_branch;
    Node* second_branch;

    // Определяем, какое поддерево просматривать первым по текущей оси.
    if ((axis == 0 && target.x < root->point.x) || (axis == 1 && target.y < root->point.y) || (axis == 2 && target.z < root->point.z)) {
        first_branch = root->left;
        second_branch = root->right;
    }
    else {
        first_branch = root->right;
        second_branch = root->left;
    }

    nearest_neighbor_recursive(first_branch, target, depth + 1, best_point, best_dist);

    double axis_dist;
    if (axis == 0) {
        axis_dist = target.x - root->point.x;
    } 
    else if (axis == 1) {
        axis_dist = target.y - root->point.y;
    }
    else {
        axis_dist = target.z - root->point.z;
    }

    // Проверяем вторую ветвь только если гиперплоскость может содержать более близкую точку.
    if (axis_dist * axis_dist < *best_dist) {
        nearest_neighbor_recursive(second_branch, target, depth + 1, best_point, best_dist);
    }
}

// Внешняя функция поиска ближайшего соседа.
Point nearest_neighbor(Node* root, Point target, int depth) {
    Point best_point = {DBL_MAX, DBL_MAX, DBL_MAX};
    double best_dist = DBL_MAX;

    if (root == NULL) return best_point;

    // Рекурсивный поиск ближайшего соседа
    nearest_neighbor_recursive(root, target, depth, &best_point, &best_dist);

    // Если точка не была найдена, возвращаем ошибку
    if (best_point.x == DBL_MAX && best_point.y == DBL_MAX && best_point.z == DBL_MAX) {
        printf("Не найден ближайший сосед для точки (%lf, %lf, %lf)\n", target.x, target.y, target.z);
        best_point.x = best_point.y = best_point.z = -1.0;  // Можно вернуть ошибочное значение
    }

    return best_point;
}

// Поиск всех точек, попадающих в прямоугольный 3D-диапазон.
void range_query(Node* root, Point lower, Point upper, int depth, Point* result, int* count) {
    if (root == NULL) return;

    int cd = depth % 3;  // Чередуем оси

    // Проверяем, входит ли текущая точка в диапазон
    if (root->point.x >= lower.x && root->point.x <= upper.x &&
        root->point.y >= lower.y && root->point.y <= upper.y &&
        root->point.z >= lower.z && root->point.z <= upper.z) {
        result[*count] = root->point;
        (*count)++;
    }

    // Рекурсивно ищем в нужных поддеревьях
    if ((cd == 0 && lower.x <= root->point.x) || (cd == 1 && lower.y <= root->point.y) || (cd == 2 && lower.z <= root->point.z)) {
        range_query(root->left, lower, upper, depth + 1, result, count);
    }

    if ((cd == 0 && upper.x >= root->point.x) || (cd == 1 && upper.y >= root->point.y) || (cd == 2 && upper.z >= root->point.z)) {
        range_query(root->right, lower, upper, depth + 1, result, count);
    }
}

// Диапазонный поиск, который возвращает индексы точек вместо самих координат.
void range_query_indices(Node* root, Point lower, Point upper, int depth, int* result, int* count) {
    if (root == NULL) return;

    int cd = depth % 3;

    if (root->point.x >= lower.x && root->point.x <= upper.x &&
        root->point.y >= lower.y && root->point.y <= upper.y &&
        root->point.z >= lower.z && root->point.z <= upper.z) {
        result[*count] = root->index;
        (*count)++;
    }

    if ((cd == 0 && lower.x <= root->point.x) || 
    (cd == 1 && lower.y <= root->point.y) || 
    (cd == 2 && lower.z <= root->point.z)) {
        range_query_indices(root->left, lower, upper, depth + 1, result, count);
    }

    if ((cd == 0 && upper.x >= root->point.x) || 
    (cd == 1 && upper.y >= root->point.y) || 
    (cd == 2 && upper.z >= root->point.z)) {
        range_query_indices(root->right, lower, upper, depth + 1, result, count);
    }

}

// Полное освобождение памяти KD-дерева.
void free_tree(Node* root) {
    if (root == NULL) {
        return;
    }

    free_tree(root->left);
    free_tree(root->right);
    free(root);
}
