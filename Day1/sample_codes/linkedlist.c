#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
  int data;
  struct Node *next;
} Node;

typedef struct List {
  Node head; // actually stub, the data contained is invalid
  Node *tail;
} List;

void init_list(List *l) {
    l->head.data = 0;
    l->head.next = NULL;
    l->tail = &l->head;
}

void add_to_list(List *l, Node *node) {
    l->tail->next = node;
    l->tail = node;
}

void traverse_list(List *l) {
    for (Node* node = l->head.next; node; node = node->next) {
        printf("%d ", node->data);
    }
    printf("\n");
}

void remove_from_list(List *l, int val) {
    struct Node* t = l->head.next;
    struct Node* t2 = &l->head;
    for(;t;t2=t, t=t->next) {
        if(t->data == val) {
            t2->next = t->next;
            free(t);
            break;
        }
    }
}

int main () {

    List linkedlist;
    init_list(&linkedlist);
    Node* new_node = malloc(sizeof(Node));
    new_node->data = 15;
    new_node->next = NULL;
    add_to_list(&linkedlist, new_node);
    new_node = malloc(sizeof(Node));
    new_node->data = 16;
    new_node->next = NULL;
    add_to_list(&linkedlist, new_node);
    traverse_list(&linkedlist);

    remove_from_list(&linkedlist, 16);
    traverse_list(&linkedlist);
    remove_from_list(&linkedlist, 15);


    return 0;
}
