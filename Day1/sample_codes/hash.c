#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int no_buckets = 8;
#define no_slots 8

uint32_t adler32(const void *buf, size_t buflength) {
    const uint8_t *buffer = (const uint8_t*)buf;

    uint32_t s1 = 1;
    uint32_t s2 = 0;

    for (size_t n = 0; n < buflength; n++) {
        s1 = (s1 + buffer[n]) % 65521;
        s2 = (s2 + s1) % 65521;
    }
    uint32_t a = (s2 << 16) | s1;
    return a?a:65521;
}

uint32_t(*hash)(const void *buf, size_t buflength) = adler32;

struct Bucket {
    uint32_t hash[no_slots]; // pointer to array, hashes of keys, default = 0
    int index[no_slots]; // pointer to array, index of key-vals array of this hash, default = -1
};

struct HashMap {
    struct Bucket* buckettable; // pointer to bucket array

    // no globals because we modify them when resizing
    // imp to have a copy of buck n slot no when the hashmap was init-ed
    int m_buckets;
    int m_slots;
    int index_keyval; // 0 to no_buckets*no_slots, basically the index of keys, values and sizes_of_keys

    char* * keys;
    int* sizes_of_keys; // to store string lengths
    int* values; // pointer to values array, default = 0
};

int hashmapInit(struct HashMap* hashmap);
int hashmapDeInit(struct HashMap* hashmap);
int hashmapGet (struct HashMap* hashmap, char* k, int* v);
int hashmapResize(struct HashMap* hashmap);
int hashmapAdd(struct HashMap* hashmap, char* k, int v);
int hashmapDel (struct HashMap* hashmap, char* k);
int check_full(struct HashMap* hashmap, int bucketindex);
int check_something(struct HashMap* hashmap, int bucketindex);

int hashmapInit(struct HashMap* hashmap) {
    hashmap->m_buckets = no_buckets;
    hashmap->m_slots = no_slots;
    hashmap->index_keyval = 0;

    hashmap->buckettable = (struct Bucket*)malloc(sizeof(struct Bucket)*hashmap->m_buckets);

    // initializing the buckettable
    for (int i = 0; i < hashmap->m_buckets; ++i) {
        for (int j = 0; j < hashmap->m_slots; ++j) {
            hashmap->buckettable[i].hash[j] = 0;
        }
    }
    for (int i = 0; i < hashmap->m_buckets; ++i) {
        for (int j = 0; j < hashmap->m_slots; ++j) {
            hashmap->buckettable[i].index[j] = -1;
        }
    }

    hashmap->keys = (char**)malloc(sizeof(char*)* hashmap->m_buckets*hashmap->m_slots);
    hashmap->sizes_of_keys = (int*)malloc(sizeof(int)*hashmap->m_buckets*hashmap->m_slots);
    hashmap->values = (int*)malloc(sizeof(int)*hashmap->m_buckets*hashmap->m_slots);

    for (int i = 0; i < hashmap->m_buckets*hashmap->m_slots; ++i) {
        hashmap->keys[i] = NULL;
    }
    for (int i = 0; i < hashmap->m_buckets*hashmap->m_slots; ++i) {
        hashmap->values[i] = 0;
    }
    for (int i = 0; i < hashmap->m_buckets*hashmap->m_slots; ++i) {
        hashmap->sizes_of_keys[i] = 0;
    }
    return 0;
}

int hashmapDeInit(struct HashMap* hashmap) {
    
    for(int i = 0; i < hashmap->index_keyval; i++) {
        free(hashmap->keys[i]);
    }

    free(hashmap->buckettable);
    hashmap->buckettable = NULL;

    free(hashmap->keys);
    hashmap->keys = NULL;

    free(hashmap->sizes_of_keys);
    hashmap->sizes_of_keys = NULL;

    free(hashmap->values);
    hashmap->values = NULL;

    return 0;
}

int hashmapAdd(struct HashMap* hashmap, char* k, int v) {
    size_t size = strlen(k);
    uint32_t hash = adler32((const void*)k, size);
    int pos = hash & (hashmap->m_buckets*hashmap->m_slots - 1);
    int bucketindex = pos >> 3;
    int slotindex = pos & (hashmap->m_slots - 1);

    // check if the key has been already used in a entry
    int t = slotindex;
    while (1) {
        if (hashmap->buckettable[bucketindex].hash[slotindex] != hash) {
            slotindex++;
            if (slotindex == hashmap->m_slots)
                slotindex = 0;
            if (slotindex == t)
                break;
            continue;
        } else {
            if(memcmp(hashmap->keys[hashmap->buckettable[bucketindex].index[slotindex]], k,hashmap->sizes_of_keys[hashmap->buckettable[bucketindex].index[slotindex]])==0) {
                printf("Something Something Already Added. %d @ %s", __LINE__, __FILE__);
                return -1;
            }
        }
    }
    slotindex = t;

    // check if there is room in the bucket
    if (check_full(hashmap, bucketindex)) {
        // run code to resize hashmap
        hashmapResize(hashmap);
    }

    // since there is a free slot find one
    while (1) {
        if (hashmap->buckettable[bucketindex].hash[slotindex] != 0) {
            slotindex++;
            if (slotindex == hashmap->m_slots)
                slotindex = 0;
            continue;
        }
        break;
    }

    // now that we have a bucketindex and slotindex, use them to fill in the buckettable
    hashmap->buckettable[bucketindex].hash[slotindex] = hash;
    hashmap->buckettable[bucketindex].index[slotindex] = hashmap->index_keyval;

    hashmap->keys[hashmap->index_keyval] = (char*)malloc(sizeof(int)*size);
    memcpy(hashmap->keys[hashmap->index_keyval], k, size*sizeof(int));

    hashmap->sizes_of_keys[hashmap->index_keyval] = size;
    hashmap->values[hashmap->index_keyval] = v;

    hashmap->index_keyval++;

    return 0;
}

int hashmapDel (struct HashMap* hashmap, char* k) {
    size_t size = strlen(k);
    uint32_t hash = adler32((const void*)k, size);
    int pos = hash & (hashmap->m_buckets*hashmap->m_slots - 1);
    int bucketindex = pos >> 3;
    int slotindex = pos & (hashmap->m_slots - 1);

    // check if there is atleast one thing in the bucket
    if (!check_something(hashmap, bucketindex)) {
        return -1; // nothing is in the bucket
    }

    // if there is the required thing in the slot find it
    int tmp = slotindex;
    while (1) {
        if (hashmap->buckettable[bucketindex].hash[slotindex] == hash) { // hashes may match but our hash function isn't sha256, multiple inputs can have the same hash, sha256 is believed to be immune to this (and many others may be aswell) but its kinda hard to implement
            if (memcmp(hashmap->keys[hashmap->buckettable[bucketindex].index[slotindex]], k, size) == 0) {
                break;
            }
        }
        slotindex++;
        if (slotindex == no_slots)
            slotindex = 0;
        if (slotindex == tmp)
            return -1; // notthing to delete, we checked all slot indexes
    } // now we have slot index to delete

    // go on a delete spree
    free(hashmap->keys[hashmap->buckettable[bucketindex].index[slotindex]]);

    hashmap->keys[hashmap->buckettable[bucketindex].index[slotindex]] = NULL;
    hashmap->values[hashmap->buckettable[bucketindex].index[slotindex]] = 0;

    hashmap->buckettable[bucketindex].index[slotindex] = -1;
    hashmap->buckettable[bucketindex].hash[slotindex] = 0;

    return 0;
}

int hashmapResize(struct HashMap* hashmap) {
    // IMPORTANT
    // the code can resize itself
    // you dont have to call this func

    no_buckets *= 2;

    struct HashMap hTemp;
    hashmapInit(&hTemp);
    for(int i = 0; i < hashmap->index_keyval; i++) {
        hashmapAdd(&hTemp, hashmap->keys[i], hashmap->values[i]);
    }

    hashmapDeInit(hashmap);
    hashmap->buckettable = hTemp.buckettable;
    hashmap->index_keyval = hTemp.index_keyval;
    hashmap->keys = hTemp.keys;
    hashmap->m_buckets = hTemp.m_buckets;
    hashmap->m_slots = hTemp.m_slots;
    hashmap->sizes_of_keys = hTemp.sizes_of_keys;
    hashmap->values = hTemp.values;

    return 0;
}

int hashmapGet (struct HashMap* hashmap, char* k, int* v) {
    size_t size = strlen(k);
    uint32_t hash = adler32((const void*)k, size);
    int pos = hash & (hashmap->m_buckets*hashmap->m_slots - 1);
    int bucketindex = pos / hashmap->m_slots;
    int slotindex = pos & (hashmap->m_slots - 1);

    // check if atleast Something is in bucket
    if(!check_something(hashmap, bucketindex)) {
        memset(v, 0, sizeof(int));
        return -1;
    }

    // since something exist start looking if its a match
    int tmp = slotindex;
    while (1) {
        if (hashmap->buckettable[bucketindex].hash[slotindex] == hash) { // hashes may match but our hash function isn't sha256, multiple inputs can have the same hash, sha256 is believed to be immune to this (and many others may be aswell) but its kinda hard to implement
            // so also check the actual keys
            if (memcmp(hashmap->keys[hashmap->buckettable[bucketindex].index[slotindex]], k, size) == 0) {
                break;
            }
        }
        slotindex++;
        if (slotindex == no_slots)
            slotindex = 0;
        if (slotindex == tmp)
            return -1; // key dose not exists, we checked all slot indexes
    } // now we have slot index to get

    *v = hashmap->values[hashmap->buckettable[bucketindex].index[slotindex]];
    return 0;
}

int check_full(struct HashMap* hashmap, int bucketindex) {
    // if it runs without breaking then thatd mean it is full
    // that case i == 7
    int i = 0;
    for (i = 0; i < hashmap->m_slots; i++)
        if (hashmap->buckettable[bucketindex].hash[i] == 0)
            break;

    if (i == 7)
        return 1;

    return 0;
}

int check_something(struct HashMap* hashmap, int bucketindex) {
    int i = 0;

    // if you can find a non zero you have found something in the array
    for (i = 0; i < hashmap->m_slots; i++)
        if (hashmap->buckettable[bucketindex].hash[i] != 0)
            return 1;
    return 0;
}

int main() {
    printf("Hello Warudo!\n");

    struct HashMap hashmap;
    hashmapInit(&hashmap);

    hashmapAdd(&hashmap, "Wrry", 15);
    hashmapAdd(&hashmap, "Hehhh", 16);
    hashmapDel(&hashmap, "Wrry");
    int ans;
    if(hashmapGet(&hashmap, "Wrry", &ans) == 0) {
        printf("%d@%d\n", ans, __LINE__);
    }
    if(hashmapGet(&hashmap, "Hehhh", &ans) == 0) {
        printf("%d@%d\n", ans, __LINE__);
    }

    hashmapDeInit(&hashmap);

    return 0;
}
