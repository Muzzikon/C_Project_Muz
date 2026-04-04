#include <stdio.h>
#include <stdlib.h>
#include "kd_tree.h"

Node* insert(Node* root, Point point, int depth) {
    	if (root == NULL) {
        	Node* new_node = (Node*)malloc(sizeof(Node));
        	new_node->point = point;
        	new_node->left = new_node->right = NULL;
        	return new_node;
    	}

    	int cd = depth % 2;  // Чередуем оси: 0 - x, 1 - y

    	if (cd == 0) {  // Если ось x
        	if (point.x < root->point.x) {
            		root->left = insert(root->left, point, depth + 1);
        	}
		else {
            		root->right = insert(root->right, point, depth + 1);
        	}
    	}
	else {  // Если ось y
        	if (point.y < root->point.y) {
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
    	printf("Point: (%lf, %lf)\n", root->point.x, root->point.y);
    	print_tree(root->left);
    	print_tree(root->right);
}
