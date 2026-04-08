#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "kd_tree.h"

Node* insert(Node* root, Point point, int depth) {
    if (root == NULL) {
        Node* new_node = (Node*)malloc(sizeof(Node));
        new_node->point = point;
        new_node->left = new_node->right = NULL;

        return new_node;
    }

    int cd = depth % 3;  // Чередуем оси: 0 - x, 1 - y

    if (cd == 0) {  // Если ось x
        if (point.x < root->point.x) {
            root->left = insert(root->left, point, depth + 1);
		}
		else {
            root->right = insert(root->right, point, depth + 1);
        }
    }
	else if (cd == 1) {  // Если ось y
        if (point.y < root->point.y) {
            root->left = insert(root->left, point, depth + 1);
        }
		else {
            root->right = insert(root->right, point, depth + 1);
        }
    }
    else {
        if (point.z < root->point.z) {
            root->left = insert(root->left, point, depth + 1);
        }
        else {
            root->right = insert(root->right, point, depth + 1);
        }
    }

    return root;
}

void print_tree(Node* root) {
    if (root == NULL) return;
    printf("Point: (%lf, %lf, %lf)\n", root->point.x, root->point.y, root->point.z);
    print_tree(root->left);
    print_tree(root->right);
}

static double distance_squared(Point a, Point b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    double dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
}

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

    if (axis_dist * axis_dist < *best_dist) {
        nearest_neighbor_recursive(second_branch, target, depth + 1, best_point, best_dist);
    }
}

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

// Функция для поиска точек в диапазоне
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
    if ((cd == 0 && lower.x < root->point.x) || (cd == 1 && lower.y < root->point.y) || (cd == 2 && lower.z < root->point.z)) {
        range_query(root->left, lower, upper, depth + 1, result, count);
    }

    if ((cd == 0 && upper.x > root->point.x) || (cd == 1 && upper.y > root->point.y) || (cd == 2 && upper.z > root->point.z)) {
        range_query(root->right, lower, upper, depth + 1, result, count);
    }
}

void free_tree(Node* root) {
    if (root == NULL) {
        return;
    }

    free_tree(root->left);
    free_tree(root->right);
    free(root);
}
