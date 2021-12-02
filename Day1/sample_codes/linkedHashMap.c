#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define no_hashslots 16

uint32_t adler32(const void* buf, size_t buflength) {
    const uint8_t* buffer = (const uint8_t*)buf;

    uint32_t s1 = 1;
    uint32_t s2 = 0;

    for (size_t n = 0; n < buflength; n++) {
        s1 = (s1 + buffer[n]) % 65521;
        s2 = (s2 + s1) % 65521;
    }
    uint32_t a = (s2 << 16) | s1;
    return a ? a : 65521;
}

uint32_t (*hashFunction)(const void* buf, size_t buflength) = adler32;


struct Node {
    uint32_t hash;
    char* key;
    int val;
    struct Node* next;
};

int addLinkedNode(struct Node** hashtableslot, struct Node* head, uint32_t hash, char* key, int val) {

    char* keycpy = NULL;
    int size = strlen(key) + 1;
    keycpy = malloc(size);
    if (keycpy == NULL) {
        printf("%s:%d malloc failed!", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    memcpy(keycpy, key, size);



    struct Node* new_node = NULL;
    new_node = malloc(sizeof(struct Node));
    if (new_node == NULL) {
        printf("%s:%d malloc failed!", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    new_node->hash = hash;
    new_node->key = keycpy;
    new_node->next = NULL;
    new_node->val = val;



    if (head == NULL) {
        *hashtableslot  = new_node;
        return 0;
    }
    new_node->next = head;
    *hashtableslot = new_node;
    return 0;
}

int freeLinkedList(struct Node* head) {
    while (1) {
        if (head == NULL) {
            break;
        }
        struct Node* temp = head->next;
        free(head->key);
        free(head);
        head = temp;
    }
    return 0;
}

struct HashMap {
    struct Node* hashtable[no_hashslots];
};

int hashmapInit(struct HashMap* hashmap) {
    for (int i = 0; i < no_hashslots; i++)
    {
        hashmap->hashtable[i] = NULL;
    }
    return 0;
}

int keySame(struct HashMap* hashmap, int hashindex, char* key) {
    struct Node* head = hashmap->hashtable[hashindex];
    
    while (1) {
        if (head == NULL)
            return 0;
        if (strcmp(head->key, key) == 0)
            return 1;
        head = head->next;
    }
    return 0;
}

int hashmapAdd(struct HashMap* hashmap, char* key, int val) {
    uint32_t hash = adler32(key, (uint64_t)strlen(key) + 1);
    int index = hash & (no_hashslots - 1);

    if (keySame(hashmap, index, key))
        return 0;

    struct Node* head = hashmap->hashtable[index];
    addLinkedNode(hashmap->hashtable + index, head, hash, key, val);
    return 0;
}

int hashmapGet(struct HashMap* hashmap, char* key, int* val) {
    uint32_t hash = adler32(key, (uint64_t)strlen(key) + 1);
    int index = hash & (no_hashslots - 1);

    struct Node* head = hashmap->hashtable[index];
    while (1) {
        if (head == NULL)
            return 0;
        if (strcmp(head->key, key) == 0)
            break;
        head = head->next;
    }

    *val = head->val;
    return 1;
}

int hashmapDel(struct HashMap* hashmap, char* key) {
    uint32_t hash = adler32(key, (uint64_t)strlen(key) + 1);
    int index = hash & (no_hashslots - 1);

    struct Node* head = hashmap->hashtable[index];
    while (1) {
        if (head == NULL)
            return 0;
        if (strcmp(head->key, key) == 0)
            break;
        head = head->next;
    }
    struct Node* temp = (head->next == NULL) ? NULL : head->next->next;
    free(head->key);
    free(head);
    hashmap->hashtable[index] = temp;
    return 0;
}

int hashmapFree(struct HashMap* hashmap) {
    
    for (int i = 0; i < no_hashslots; i++) {
        hashmap->hashtable[i] ? freeLinkedList(hashmap->hashtable[i]) : 0;
        hashmap->hashtable[i] = NULL;
    }
    return 0;
}

int main() {
    struct HashMap hashmap;
    hashmapInit(&hashmap);
    hashmapAdd(&hashmap, "Wrry13", 13);
    hashmapAdd(&hashmap, "Wrry15", 15);
    
    int a;
    int error = hashmapGet(&hashmap, "Wrry13", &a);
    if (error) printf("%d\n", a);

    error = hashmapGet(&hashmap, "Wrry15", &a);
    if (error) printf("%d\n\n",a);
    
    hashmapDel(&hashmap, "Wrry13");
    hashmapDel(&hashmap, "Wrasda");
    
    error = hashmapGet(&hashmap, "Wrry13", &a);
    if (error) printf("%d\n", a);
    
    error = hashmapGet(&hashmap, "Wrry15", &a);
    if (error) printf("%d\n", a);

    hashmapFree(&hashmap);
    
    return 0;
}
