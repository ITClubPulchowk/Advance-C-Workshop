// 8 Buckets 8 slots

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define bucket 8
#define slot 8

// struct for bucket
struct Bucket {
    int remaining; // how many slots are remaining
    int index[slot];
    int hash[slot];
};

// the 3 arrays of a hashtable
struct HashTable {
    struct Bucket buckettable[bucket];
    char* key[bucket*slot];
    int value[bucket*slot];
    int key_val_current_index; // keeps track of how many slots in key and value array are filled
};

struct HashTableCoord {
    int bucketindex;
    int slotindex;
};

void init(struct HashTable* hashmap) {
    hashmap->key_val_current_index = 0;
    for (int i = 0; i < bucket; ++i) {
        hashmap->buckettable[i].remaining = 8;
    }
    for (int i = 0; i < bucket; ++i) {
        for (int j = 0; j < slot; ++j) {
            hashmap->buckettable[i].hash[j] = 0;
        }
    }
    for (int i = 0; i < bucket; ++i) {
        for (int j = 0; j < slot; ++j) {
            hashmap->buckettable[i].index[j] = -1;
        }
    }
    for (int i = 0; i < bucket*slot; ++i) {
        hashmap->key[i] = NULL;
    }
    for (int i = 0; i < bucket*slot; ++i) {
        hashmap->value[i] = 0;
    }
}

//the hash function
//sdbm
//https://stackoverflow.com/a/14409947
int hashfunction(char* key) {
    int hash = 0;
    for (int i = 0; key[i] != '\0'; ++i ) {
        hash = key[i] + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

int addKeyVal (struct HashTable* hashmap, char* key, int value) {
    hashmap->key[hashmap->key_val_current_index] = key;
    hashmap->value[hashmap->key_val_current_index] = value;

    hashmap->key_val_current_index++;

    return hashmap->key_val_current_index - 1;
}

int addHashMap (struct HashTable* hashmap, char* key, int value) {
    int keyhash = hashfunction(key);
    int pos = keyhash & (bucket*slot - 1);
    int bucketindex = pos / slot;
    int slotindex = pos & (slot - 1);

    // check if there is room in the bucket
    if (hashmap->buckettable[bucketindex].remaining == 0) {
        // run code to resize hashmap
        printf("You run into a condition which would be normally mitigated by resizing the hashtable. But this is a completely static implementation so that cant happen. Exiting!\n");
        exit(EXIT_FAILURE);
    }

    // once there is room get a index to a free slot
    while (1) {
        if (hashmap->buckettable[bucketindex].hash[slotindex] != 0) {
            slotindex++;
            if (slotindex == 8)
                slotindex = 0;
            continue;
        }
        break;
    }

    int index = addKeyVal(hashmap, key, value);
    hashmap->buckettable[bucketindex].hash[slotindex] = keyhash;
    hashmap->buckettable[bucketindex].index[slotindex] = index;
    hashmap->buckettable[bucketindex].remaining--;
    return 0;
}

struct HashTableCoord findStoredHash(struct HashTable* hashmap, char* key) {
    int keyhash = hashfunction(key);
    int pos = keyhash & (bucket*slot - 1);
    int bucketindex = pos / slot;
    int slotindex = pos & (slot - 1);

    int temp = slotindex;
    while (1) {
        if (strcmp(hashmap->key[hashmap->buckettable[bucketindex].index[slotindex]], key)) {
            // if they are equal strcmp returns 0 and it breaks
            // else slotindex increases to find the correct index
            slotindex++;
            if (slotindex == 8)
                slotindex = 0;
            if (slotindex == temp) {
                printf("Not found\n");
                struct HashTableCoord a;
                a.bucketindex = -1;
                a.slotindex = -1;
                return a;
            }
            continue;
        }
        break;
    }

    struct HashTableCoord a;
    a.bucketindex = bucketindex;
    a.slotindex = slotindex;
    return a;
}

int deleteHashMap (struct HashTable* hashmap, char* key) {
    struct HashTableCoord pos = findStoredHash(hashmap, key);

    if (pos.bucketindex == -1 || pos.slotindex == -1) {
        return 0;
    }

    // check if something is in the bucket, else we can skip
    if (hashmap->buckettable[pos.bucketindex].remaining == 8) {
        // run code to resize hashmap
        printf("Nothing to delete it seems\n");
        return 0;
    }

    // delete it
    hashmap->buckettable[pos.bucketindex].hash[pos.slotindex] = 0;
    int index = hashmap->buckettable[pos.bucketindex].index[pos.slotindex];
    hashmap->buckettable[pos.bucketindex].index[pos.slotindex] = -1;

    // now bring the last entry to the just emptied vaccum
    hashmap->key[index] = hashmap->key[hashmap->key_val_current_index - 1];
    hashmap->value[index] = hashmap->value[hashmap->key_val_current_index - 1]; //because of the remaining variable we simply wont acess this so this is redundant

    hashmap->key_val_current_index--;

    // now goto hashtable and change the entry which references the just moved thing
    struct HashTableCoord xy = findStoredHash(hashmap, hashmap->key[index]);
    hashmap->buckettable[xy.bucketindex].index[xy.slotindex] = index;


    hashmap->key[hashmap->key_val_current_index] = NULL;
    hashmap->value[hashmap->key_val_current_index] = 0;

    return 0;
}

int getHashMap(struct HashTable* hashmap, char* key) {
    // this function is left as an excercise for you
    // use findStoredHashFunction
    // its simple really
    // you get indexes
    // and use them
    // wont be surp if some of you do it in fewer lines than these comments
    // good luck
}

int main() {
    struct HashTable hashmap;
    init(&hashmap);
    addHashMap(&hashmap, "stealth major", 202);
    addHashMap(&hashmap, "senbonzakura", 2045);
    deleteHashMap(&hashmap, "stealth major");

    return 0;
}
