#include <stdio.h>
#include <stdlib.h>

struct node {
    int data;
    struct node* left;
    struct node* right;      
};

typedef struct node node;

node* createNewNode(int data) {
    node* newNode = (node*) malloc(sizeof(node));
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

void addElementLeft(node** root, int data) {
    (*root)->left = createNewNode(data); 
}

void addElementRight(node** root, int data) {
    (*root)->right = createNewNode(data); 
}

void printTabs(int num) {
    for (int i = 0; i < num; i++) {
        printf("\t");
    }
}

void printTree(node* root, int level) {
    if (root == NULL) return;
    printTabs(level);
    printf("data = %d\n", root->data);
    printTabs(level);
    printf("left\n");
    printTree(root->left, level+1);
    printTabs(level);
    printf("right\n");
    printTree(root->right, level+1);
}

void freeTree (node* root) {
	if (root->left) {
		freeTree(root->left);
	}
	if (root->right) {
		freeTree(root->right);
	}
	free(root);
}

int main() {
    node* n1 = createNewNode(12);
    node* n2 = createNewNode(13);
    n1->left = n2;
    addElementRight(&n1, 15);
    addElementLeft(&n2, 21);
    addElementRight(&n2, 54);
    printTree(n1, 0);
    freeTree(n1);
    return 0;
}
