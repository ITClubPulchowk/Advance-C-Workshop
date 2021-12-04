#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    char *data;
    uint32_t length;
} string;

typedef struct entry{
    struct entry *next;
    string *values;
    uint32_t hash, value_count, value_cap;
} entry;

typedef struct {
    entry *tail;
    entry head;
} linked_list;

typedef struct {
    char *data;
    string left;
    string rght;
} lexer;

uint32_t alder32(string string) {
    uint32_t a = 1, b = 0;
    for (int i = 0; i < string.length; i ++) {
        a += *(string.data + i);
        b += a;
    }
    return (b % 65521) * 65536 + (a % 65521);
}

#define isnewline(ch) (!ch || ch == '\r' || ch == '\n')

void get_line(lexer *lex){
    lex->left.data = lex->data;
    lex->left.length = 0;
    for (; !isnewline(*lex->data) && *lex->data != ','; lex->data ++)
        lex->left.length ++;
    lex->data++;
    lex->rght.data = lex->data;
    lex->rght.length = 0;
    for (; !isnewline(*lex->data); lex->data ++)
        lex->rght.length ++;
    lex->data += *lex->data != 0;
    lex->data += *lex->data == '\n';
}

void init_lexer(lexer *ini, char *data) {
    ini->data = data;
    ini->rght.data = NULL;
    ini->left.data   = NULL;
    ini->rght.length = 0;
    ini->left.length   = 0;
}

linked_list *create_hash_table() {
    linked_list *table;
    table = malloc(sizeof(*table) * 16);
    for (int i = 0; i < 16; i ++) {
        table[i].head.next = NULL;
        table[i].tail  = &table[i].head;
    }
    return table;
}

void add_entry(linked_list *table, string key, string value) {
    uint32_t hash  = alder32(key);
    uint32_t index = hash & 15;

    for (entry *trav = table[index].head.next; trav; trav = trav->next){
        if (trav->hash == hash) {
            if (trav->value_count >= trav->value_cap) {
                trav->value_cap *= 2;
                trav->values = realloc(trav->values, trav->value_cap);
            }
            trav->values[trav->value_count] = value;
            trav->value_count ++;
            return;
        }
    }

    entry *insert  = malloc(sizeof(entry));
    insert->hash = hash;
    insert->next = NULL;
    insert->value_cap = 4;
    insert->values  = malloc(sizeof(string) * 4);
    *insert->values = value;
    insert->value_count = 1;
    table[index].tail->next = insert;
    table[index].tail = insert;
}

entry *most_incoming(linked_list *table) {
    entry *ret = NULL;
    uint32_t greatest = 0;

    for (int i = 0; i < 16; i ++) {
        for (entry *trav = table[i].head.next; trav; trav = trav->next) {
            if (trav->value_count > greatest) {
                greatest = trav->value_count;
                ret = trav;
            }
        }
    }

    return ret;
}

string build_out_string(uint32_t count, string* values){
    // 1 extra for \0
    uint32_t out_srt_len = count;
    uint32_t off = 1; // comma after number
    for (int cpy = count; cpy; cpy /= 10) off ++;
    out_srt_len += off;
    for (int i = 0; i < count; i ++)
        out_srt_len += values[i].length;
    string ret;
    ret.data = malloc(out_srt_len + 1);
    ret.length = out_srt_len - 1;
    sprintf(ret.data, "%d,", count);
    char *trav = ret.data + off;
    for (int i = 0; i < count - 1; i ++) {
        for (int j = 0; j < values[i].length; j ++) {
            *trav = values[i].data[j];
            trav ++;
        }
        *trav = ',';
        trav ++;
    }
    for (int j = 0; j < values[count - 1].length; j ++) {
        *trav = values[count - 1].data[j];
        trav ++;
    }
    ret.data[out_srt_len] = 0;
    return ret;
}

int main() {
    FILE *inp_file = fopen("airport_b.csv", "rb");
    if (!inp_file) {
        printf("What file?\n");
        return -2;
    }
    fseek(inp_file, SEEK_SET, SEEK_END);
    int size = ftell(inp_file);
    rewind(inp_file);

    char *data = malloc(size + 1);
    fread(data, 1, size, inp_file);
    data[size] = 0;
    fclose(inp_file);

    lexer lex;
    init_lexer(&lex, data);

    linked_list *flight_table = create_hash_table();

    while (*lex.data) {
        get_line(&lex);
        add_entry(flight_table, lex.rght, lex.left);
    }

    entry *result = most_incoming(flight_table);
    string write_str = build_out_string(result->value_count, result->values);

    FILE *out_file = fopen("airport_output.csv", "wb");
    fwrite(write_str.data, 1, write_str.length, out_file);
    fclose(out_file);

    return 0;
}
