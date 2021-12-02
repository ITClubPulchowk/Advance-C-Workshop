#include <stdio.h>
#include <malloc.h>

struct Node {
    int val;
    struct Node* next;
};

void addNodeFront(struct Node** headAddress, int data) {
    if (headAddress == NULL)
    	return;
    struct Node* temp = malloc(sizeof(struct Node));
    temp->next = *headAddress;
    temp->val = data;
    *headAddress = temp;
}

void addNodeBack(struct Node** head, int data) {
    if (head == NULL)
    	return;

    if (*head == NULL){
    	*head = malloc(sizeof(struct Node));
    	(*head)->next = NULL;
        (*head)->val = data;
        
        return;
    }
    struct Node* temp = *head;    
    while (temp->next != NULL)
        temp= temp->next;
    temp->next = malloc(sizeof(struct Node));
    temp = temp->next;
    temp->next = NULL;
    temp->val = data;
};

void printLList(struct Node* head) {
    if (head == NULL)
        return; // i am doing this because if I dont tala ko newline to prints even when we didnt write aything to console

    while (head != NULL) {
        printf("%d ", head->val);
        head = head->next;
    }

    printf("\n");
}

void freeLList(struct Node* head) {
    while (head != NULL) {
        struct Node* temp = head->next;
        free(head);
        head = temp;
    }
}

int main() {
    struct Node* head = NULL;

    // adding a node.
    head = malloc(sizeof(struct Node));
    head->next = NULL;
    head->val = 0;

    free(head);
    head = NULL;

    addNodeBack(&head, 20);
    addNodeFront(&head, 15);
    addNodeFront(&head, 16);
    addNodeFront(&head, 18);
    addNodeBack(&head, 17);
    
    printLList(head);
    freeLList(head);

    return 0;
}
