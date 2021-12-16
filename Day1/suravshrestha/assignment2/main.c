#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> // For data type uint32_t

#define AIRPORTS_FILENAME "airport_a.txt"
#define TRANSPORTS_FILENAME "airport_b.csv"

#define OUTPUT_FILENAME "airport_output.csv"

#define BUFFER_LENGTH 255

int no_buckets = 8;
#define no_slots 8

uint32_t adler32(const void *buf, size_t buflength)
{
    const uint8_t *buffer = (const uint8_t *)buf;

    uint32_t s1 = 1;
    uint32_t s2 = 0;

    for (size_t n = 0; n < buflength; n++)
    {
        s1 = (s1 + buffer[n]) % 65521;
        s2 = (s2 + s1) % 65521;
    }

    uint32_t a = (s2 << 16) | s1;

    return a ? a : 65521;
}

uint32_t (*hash)(const void *buf, size_t buflength) = adler32;

struct Bucket
{
    uint32_t hash[no_slots]; // pointer to array, hashes of keys, default = 0
    int index[no_slots];     // pointer to array, index of key-vals array of this hash, default = -1
};

struct HashMap
{
    struct Bucket *bucket_table; // pointer to bucket array

    // no globals because we modify them when resizing
    // imp to have a copy of bucket and slot no when the hashmap was init-ed
    int m_buckets;
    int m_slots;
    int index_keyval; // 0 to no_buckets*no_slots, basically the index of keys, values and sizes_of_keys

    char **keys;
    int *sizes_of_keys; // to store string lengths
    int *values;        // pointer to values array, default = 0
};

int hashmap_init(struct HashMap *hashmap);
int hashmap_deinit(struct HashMap *hashmap);
int hashmap_get(struct HashMap *hashmap, char *k, int *v);
int hashmap_resize(struct HashMap *hashmap);
int hashmap_add(struct HashMap *hashmap, char *k, int v);
int hashmap_del(struct HashMap *hashmap, char *k);
int check_full(struct HashMap *hashmap, int bucket_index);
int check_something(struct HashMap *hashmap, int bucket_index);

int hashmap_init(struct HashMap *hashmap)
{
    hashmap->m_buckets = no_buckets;
    hashmap->m_slots = no_slots;
    hashmap->index_keyval = 0;

    hashmap->bucket_table = (struct Bucket *)malloc(sizeof(struct Bucket) * hashmap->m_buckets);

    // initializing the bucket_table
    for (int i = 0; i < hashmap->m_buckets; ++i)
    {
        for (int j = 0; j < hashmap->m_slots; ++j)
        {
            hashmap->bucket_table[i].hash[j] = 0;
        }
    }

    for (int i = 0; i < hashmap->m_buckets; ++i)
    {
        for (int j = 0; j < hashmap->m_slots; ++j)
        {
            hashmap->bucket_table[i].index[j] = -1;
        }
    }

    hashmap->keys = (char **)malloc(sizeof(char *) * hashmap->m_buckets * hashmap->m_slots);
    hashmap->sizes_of_keys = (int *)malloc(sizeof(int) * hashmap->m_buckets * hashmap->m_slots);
    hashmap->values = (int *)malloc(sizeof(int) * hashmap->m_buckets * hashmap->m_slots);

    for (int i = 0; i < hashmap->m_buckets * hashmap->m_slots; ++i)
    {
        hashmap->keys[i] = NULL;
    }

    for (int i = 0; i < hashmap->m_buckets * hashmap->m_slots; ++i)
    {
        hashmap->values[i] = 0;
    }

    for (int i = 0; i < hashmap->m_buckets * hashmap->m_slots; ++i)
    {
        hashmap->sizes_of_keys[i] = 0;
    }

    return 0;
}

int hashmap_deinit(struct HashMap *hashmap)
{

    for (int i = 0; i < hashmap->index_keyval; i++)
    {
        free(hashmap->keys[i]);
    }

    free(hashmap->bucket_table);
    hashmap->bucket_table = NULL;

    free(hashmap->keys);
    hashmap->keys = NULL;

    free(hashmap->sizes_of_keys);
    hashmap->sizes_of_keys = NULL;

    free(hashmap->values);
    hashmap->values = NULL;

    return 0;
}

int hashmap_add(struct HashMap *hashmap, char *k, int v)
{
    size_t size = strlen(k);
    uint32_t hash = adler32((const void *)k, size);

    int pos = hash & (hashmap->m_buckets * hashmap->m_slots - 1);
    int bucket_index = pos >> 3;
    int slot_index = pos & (hashmap->m_slots - 1);

    // check if the key has been already used in a entry
    int t = slot_index;
    while (1)
    {
        if (hashmap->bucket_table[bucket_index].hash[slot_index] != hash)
        {
            slot_index++;
            if (slot_index == hashmap->m_slots)
            {
                slot_index = 0;
            }

            if (slot_index == t)
            {
                break;
            }

            continue;
        }
        else
        {
            if (memcmp(hashmap->keys[hashmap->bucket_table[bucket_index].index[slot_index]], k, hashmap->sizes_of_keys[hashmap->bucket_table[bucket_index].index[slot_index]]) == 0)
            {
                printf("Something Something Already Added. %d @ %s", __LINE__, __FILE__);
                return -1;
            }
        }
    }

    slot_index = t;

    // check if there is room in the bucket
    if (check_full(hashmap, bucket_index))
    {
        // run code to resize hashmap
        hashmap_resize(hashmap);
    }

    // since there is a free slot find one
    while (1)
    {
        if (hashmap->bucket_table[bucket_index].hash[slot_index] != 0)
        {
            slot_index++;
            if (slot_index == hashmap->m_slots)
            {
                slot_index = 0;
            }

            continue;
        }

        break;
    }

    // now that we have a bucket_index and slot_index, use them to fill in the bucket_table
    hashmap->bucket_table[bucket_index].hash[slot_index] = hash;
    hashmap->bucket_table[bucket_index].index[slot_index] = hashmap->index_keyval;

    hashmap->keys[hashmap->index_keyval] = (char *)malloc(sizeof(int) * size);
    memcpy(hashmap->keys[hashmap->index_keyval], k, size * sizeof(int));

    hashmap->sizes_of_keys[hashmap->index_keyval] = size;
    hashmap->values[hashmap->index_keyval] = v;

    hashmap->index_keyval++;

    return 0;
}

int hashmap_del(struct HashMap *hashmap, char *k)
{
    size_t size = strlen(k);
    uint32_t hash = adler32((const void *)k, size);

    int pos = hash & (hashmap->m_buckets * hashmap->m_slots - 1);
    int bucket_index = pos >> 3;
    int slot_index = pos & (hashmap->m_slots - 1);

    // check if there is atleast one thing in the bucket
    if (!check_something(hashmap, bucket_index))
    {
        return -1; // nothing is in the bucket
    }

    // if there is the required thing in the slot find it
    int tmp = slot_index;
    while (1)
    {
        if (hashmap->bucket_table[bucket_index].hash[slot_index] == hash)
        { // hashes may match but our hash function isn't sha256, multiple inputs can have the same hash, sha256 is believed to be immune to this (and many others may be aswell) but its kinda hard to implement
            if (memcmp(hashmap->keys[hashmap->bucket_table[bucket_index].index[slot_index]], k, size) == 0)
            {
                break;
            }
        }

        slot_index++;
        if (slot_index == no_slots)
        {
            slot_index = 0;
        }

        if (slot_index == tmp)
        {
            // nothing to delete, we checked all slot indexes
            return -1;
        }
    }
    // now we have slot index to delete

    // go on a delete spree
    free(hashmap->keys[hashmap->bucket_table[bucket_index].index[slot_index]]);

    hashmap->keys[hashmap->bucket_table[bucket_index].index[slot_index]] = NULL;
    hashmap->values[hashmap->bucket_table[bucket_index].index[slot_index]] = 0;

    hashmap->bucket_table[bucket_index].index[slot_index] = -1;
    hashmap->bucket_table[bucket_index].hash[slot_index] = 0;

    return 0;
}

int hashmap_resize(struct HashMap *hashmap)
{
    // IMPORTANT
    // the code can resize itself, we dont have to call this func

    no_buckets *= 2;

    struct HashMap hTemp;
    hashmap_init(&hTemp);

    for (int i = 0; i < hashmap->index_keyval; i++)
    {
        hashmap_add(&hTemp, hashmap->keys[i], hashmap->values[i]);
    }

    hashmap_deinit(hashmap);
    hashmap->bucket_table = hTemp.bucket_table;
    hashmap->index_keyval = hTemp.index_keyval;
    hashmap->keys = hTemp.keys;
    hashmap->m_buckets = hTemp.m_buckets;
    hashmap->m_slots = hTemp.m_slots;
    hashmap->sizes_of_keys = hTemp.sizes_of_keys;
    hashmap->values = hTemp.values;

    return 0;
}

int hashmap_get(struct HashMap *hashmap, char *k, int *v)
{
    size_t size = strlen(k);
    uint32_t hash = adler32((const void *)k, size);

    int pos = hash & (hashmap->m_buckets * hashmap->m_slots - 1);
    int bucket_index = pos / hashmap->m_slots;
    int slot_index = pos & (hashmap->m_slots - 1);

    // check if at least something is in bucket
    if (!check_something(hashmap, bucket_index))
    {
        memset(v, 0, sizeof(int));
        return -1;
    }

    // since something exist start looking if its a match
    int tmp = slot_index;
    while (1)
    {
        if (hashmap->bucket_table[bucket_index].hash[slot_index] == hash)
        {
            // hashes may match but our hash function isn't sha256, multiple inputs can have the same hash,
            // sha256 is believed to be immune to this (and many others may be as well) but its kinda hard to implement,
            // so also check the actual keys
            if (memcmp(hashmap->keys[hashmap->bucket_table[bucket_index].index[slot_index]], k, size) == 0)
            {
                break;
            }
        }

        slot_index++;
        if (slot_index == no_slots)
        {
            slot_index = 0;
        }

        if (slot_index == tmp)
        {
            // key does not exist, we checked all slot indexes
            return -1;
        }
    }
    // now we have slot index to get

    *v = hashmap->values[hashmap->bucket_table[bucket_index].index[slot_index]];

    return 0;
}

int check_full(struct HashMap *hashmap, int bucket_index)
{
    // if it runs without breaking then that mean it is full that case i == 7
    int i = 0;
    for (i = 0; i < hashmap->m_slots; i++)
    {
        if (hashmap->bucket_table[bucket_index].hash[i] == 0)
        {
            break;
        }
    }

    if (i == 7)
    {
        return 1;
    }

    return 0;
}

int check_something(struct HashMap *hashmap, int bucket_index)
{
    int i = 0;

    // if we can find a non zero we have found something in the array
    for (i = 0; i < hashmap->m_slots; i++)
    {
        if (hashmap->bucket_table[bucket_index].hash[i] != 0)
        {
            return 1;
        }
    }

    return 0;
}

int main()
{
    FILE *fp_airports = fopen(AIRPORTS_FILENAME, "rb");
    FILE *fp_transports = fopen(TRANSPORTS_FILENAME, "rb");

    if (fp_airports == NULL)
    {
        printf("%s can't be accessed", AIRPORTS_FILENAME);
        return (-1);
    }

    if (fp_transports == NULL)
    {
        printf("%s can't be accessed", TRANSPORTS_FILENAME);
        return (-1);
    }

    FILE *fp_output = fopen(OUTPUT_FILENAME, "wb");

    // adler32 hashmap implemented from:
    // https://github.com/suravshrestha/Advance-C-Workshop/blob/main/resources/lecture-1/samples/hash.c
    struct HashMap hashmap;
    hashmap_init(&hashmap);

    char airport[BUFFER_LENGTH];

    char ch;
    int i, incoming_flights, max_incoming_flights = 0;

    // Initialize incoming flights of the airports in airport_a.txt to 0
    while ((ch = fgetc(fp_airports)) != EOF)
    {
        if (ch == '"')
        {
            i = 0;

            ch = fgetc(fp_airports);

            while (ch != '"')
            {
                airport[i++] = ch;
                ch = fgetc(fp_airports);
            }

            airport[i++] = '\0';

            hashmap_add(&hashmap, airport, 0);
        }
    }

    // Store the total number of incoming flights
    while ((ch = fgetc(fp_transports)) != EOF)
    {
        if (ch == ',')
        {
            i = 0;

            // Move the file pointer to the character after ,"
            ch = fgetc(fp_transports);
            ch = fgetc(fp_transports);

            while (ch != '"')
            {
                airport[i++] = ch;
                ch = fgetc(fp_transports);
            }

            airport[i++] = '\0';

            if (hashmap_get(&hashmap, airport, &incoming_flights) == 0)
            {
                hashmap_del(&hashmap, airport);
                hashmap_add(&hashmap, airport, ++incoming_flights);
            }
        }
    }

    char *airports[] = {"Bharatpur", "Biratnagar", "Birendranagar", "Dhangadhi", "Dolpa",
                        "Janakpur", "Jiri", "Jomsom", "Kathmandu", "Langtang", "Lukla",
                        "Nepalgunj", "Pokhara", "Rajbiraj", "Rara"};

    int len = sizeof(airports) / sizeof(airports[0]);

    // Get the airport with the greatest incoming flights
    for (i = 0; i < len; i++)
    {
        if (hashmap_get(&hashmap, airports[i], &incoming_flights) == 0)
        {
            // Airport found
            if (incoming_flights > max_incoming_flights)
            {
                max_incoming_flights = incoming_flights;
            }
        }
    }

    fprintf(fp_output, "%d", max_incoming_flights);

    // Print the airports with the max incoming flights in airport_output.csv
    for (i = 0; i < len; i++)
    {
        if (hashmap_get(&hashmap, airports[i], &incoming_flights) == 0)
        {
            if (incoming_flights == max_incoming_flights)
            {
                fprintf(fp_output, ",\"%s\"", airports[i]);
            }
        }
    }

    fclose(fp_airports);
    fclose(fp_airports);
    fclose(fp_output);

    return 0;
}
