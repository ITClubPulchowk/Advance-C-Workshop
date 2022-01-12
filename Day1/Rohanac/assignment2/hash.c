#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
typedef struct ht_item
{
    char key[100];
    char value[100];
    int number_of_flights;
} ht_item;
typedef struct hash_table
{
    ht_item **item;
    int count;
    int size;
} hash_table;
void print_hash_table(hash_table *table);
ht_item *ht_item_create(char *key, char *value, int *index);
int adler32(char *data, size_t len);
hash_table *hash_table_create(int size);
char *hash_table_add(char *key, char *value, hash_table **table, int ind);
void sort_table(hash_table **table);

int main()
{
    FILE *fp;
    char depart[100];
    char arrive[100];
    char *str = NULL;
    int file_size = 0;
    fp = fopen("airport_b.csv", "rb");
    if (!fp)
    {
        printf("No file");
        return 1;
    }
    fseek(fp, 0L, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    str = malloc(sizeof(*str) * (file_size + 2));
    fread(str, file_size, 1, fp);
    fclose(fp);
    str[file_size] = '\n';
    str[file_size + 1] = '\0';
    hash_table *table = hash_table_create(16);
    if (!fp)
    {
        printf("No file");
        return 1;
    }
    int arrival = 1;
    for (int i = 0, j = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '\r')
            continue;
        if (str[i] == '\n')
        {
            arrival = 1;
            arrive[j] = '\0';

            j = 0;
            hash_table_add(arrive, depart, &table, -1);
            continue;
        }
        if (str[i] == ',')
        {
            arrival = 0;
            depart[j] = '\0';
            j = 0;

            continue;
        }
        if (arrival == 0)
        {
            arrive[j] = str[i];
            j++;
        }
        if (arrival == 1)
        {
            depart[j] = str[i];
            j++;
        }
    }
    sort_table(&table);
    print_hash_table(table);
    return 0;
}
ht_item *ht_item_create(char *key, char *value, int *index)
{
    ht_item *new = malloc(sizeof(*new));
    *index = adler32(key, strlen(key)) & 15;
    strcpy(new->value, value);
    strcpy(new->key, key);
    new->number_of_flights = 0;
    return new;
}

int adler32(char *data, size_t len)
{
    int a = 1, b = 0;
    size_t index;
    // Process each byte of the data in order
    for (index = 0; index < len; ++index)
    {
        a = (a + data[index]) % 65521;
        b = (b + a) % 65521;
    }

    return ((b << 16) | a);
}
hash_table *hash_table_create(int size)
{
    hash_table *hash_t = malloc(sizeof(hash_t));
    hash_t->item = calloc(size, sizeof(*(hash_t->item)));
    hash_t->size = size;
    hash_t->count = 0;
    return hash_t;
}

char *hash_table_add(char *key, char *value, hash_table **table, int ind)
{
    if ((*table)->count > (*table)->size)
    {
        *table = realloc(*table, 2 * (*table)->size);
    }
    if (!(table))
        return "null pointer passed ";
    int index = 0;
    ht_item *item = ht_item_create(key, value, &index);
    if (ind != -1)
        index = ind;
    if (!((*table)->item)[index])
    {
        ((*table)->item)[index] = item;
        (*table)->count++;
    }
    else if (strcmp(item->key, (*table)->item[index]->key) == 0)
    {
        (*table)->item[index]->number_of_flights++;
        strcat((*table)->item[index]->value, ",");
        strcat((*table)->item[index]->value, item->value);
        return "appended";
    }
    else
    {
        if (index < (*table)->size - 1)
        {
            index++;
            return hash_table_add(key, value, table, index);
        }
        else
        {
            index = 0;
            return hash_table_add(key, value, table, index);
        }
    }
    return "success\n";
}
void print_hash_table(hash_table *table)
{
    FILE *fp = fopen("airport_output.csv", "w");
    fprintf(fp,"%d,%s\n", table->item[0]->number_of_flights + 1, table->item[0]->value);
    fclose(fp);
}
void sort_table(hash_table **table)
{
    ht_item *temp=NULL;
    for (int i = 0; i < (*table)->size; i++)
    {
        for (int j = i + 1; j < (*table)->size; j++)
        {
            if((*table)->item[i] && (*table)->item[j])
            {
                if ((*table)->item[j]->number_of_flights > (*table)->item[i]->number_of_flights)
            {
                temp = (*table)->item[j];
                (*table)->item[j] = (*table)->item[i];
                (*table)->item[i] = temp;
            }
            }
        }   
    }
    
}
