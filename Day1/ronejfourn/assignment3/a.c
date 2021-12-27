#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

typedef struct {
    int coeff, power;
} term;

typedef struct {
    char *data;
    term val;
} lexer;

typedef struct node{
    int next;
    term val;
} node;

typedef struct{
    node *allocator;
    int count, headindex;
} linked_list;

void get_next(lexer *lex){
    lex->val.coeff = 1;
    lex->val.power = 0;
    int sign = 1;

    for (;isspace(*lex->data) && *lex->data != '\r' && *lex->data != '\n';lex->data++);
    if (*lex->data == '+' || *lex->data == '-') {
        sign = *lex->data == '-' ? -1 : 1;
        lex->data ++;
        for (;isspace(*lex->data) && *lex->data != '\r' && *lex->data != '\n';lex->data++);
    }
    if (isdigit(*lex->data)) {
        lex->val.coeff = 0;
        for (;isdigit(*lex->data); lex->data ++)
            lex->val.coeff = lex->val.coeff * 10 + *lex->data - '0';
        lex->val.coeff *= sign;
        for (; isspace(*lex->data) && *lex->data != '\r' && *lex->data != '\n'; lex->data++);
    }
    if (*lex->data != 'x') return;
    lex->data ++;
    for (;isspace(*lex->data) && *lex->data != '\r' && *lex->data != '\n';lex->data++);
    if (*lex->data != '^') {
        lex->val.power = 1;
        return;
    }
    lex->data ++;
    sign = 1;
    for (;isspace(*lex->data) && *lex->data != '\r' && *lex->data != '\n';lex->data++);
    if (*lex->data == '+' || *lex->data == '-') {
        sign = *lex->data == '-' ? -1 : 1;
        lex->data ++;
        for (;isspace(*lex->data) && *lex->data != '\r' && *lex->data != '\n';lex->data++);
    }
    lex->val.power = 0;
    for (;isdigit(*lex->data); lex->data ++)
        lex->val.power = lex->val.power * 10 + *lex->data - '0';
    lex->val.power *= sign;
}

void init_list(linked_list *list, int len) {
    list->allocator = malloc(len * sizeof(*list->allocator));
    list->count = 0;
}

void insert_into_list(linked_list *list, term data, int is_head) {
    int pos = list->count;
    list->count ++;
    list->allocator[pos].val = data;
    if (is_head) return;
    node head = list->allocator[list->headindex];
    node *allocator = list->allocator;
    if (head.val.power < data.power){
        allocator[pos].next = list->headindex;
        list->headindex = pos;
    } else if (head.val.power == data.power){
        allocator[list->headindex].val.coeff += data.coeff;
    } else {
        int trav = list->headindex;
        node cur = allocator[trav];
        while (cur.next != -1) {
            trav = cur.next;
            if (allocator[trav].val.power < data.power) {
                allocator[pos ].next = trav;
                allocator[trav].next = pos ;
                return;
            } else if (allocator[trav].val.power == data.power){
                allocator[trav].val.coeff += data.coeff;
                return;
            }
            cur = allocator[trav];
        }
        allocator[trav].next = pos;
        allocator[pos ].next = -1;
    }
}

void parse_line(lexer *lex, linked_list *output) {
    output->count = 0;
    output->headindex = 0;
    output->allocator[0].next = -1;
    int is_head = 1;
    while (*lex->data && *lex->data != '\n' && *lex->data != '\r') {
        get_next(lex);
        if (lex->val.power) {
            insert_into_list(output, (term){lex->val.coeff * lex->val.power, lex->val.power - 1}, is_head);
            is_head = 0;
        }
    }
    lex->data += *lex->data != 0;
    lex->data += *lex->data == '\n';
}

void write_derivative(FILE *dest, linked_list data){
    node head = data.allocator[data.headindex];
    if (head.val.power > 1)
        fprintf(dest, "%dx^%d", head.val.coeff, head.val.power);
    else if (head.val.power == 1)
        fprintf(dest, "%dx", head.val.coeff);
    else
        fprintf(dest, "%d", head.val.coeff);
    int trav = head.next;
    while (trav != -1) {
        node cur = data.allocator[trav];
        if (cur.val.coeff > 0)
            fprintf(dest, "%c", '+');
        if (cur.val.power > 1 || cur.val.power < 0)
            fprintf(dest, "%dx^%d", cur.val.coeff, cur.val.power);
        else if (cur.val.power == 1)
            fprintf(dest, "%dx", cur.val.coeff);
        else
            fprintf(dest, "%d", cur.val.coeff);
        trav = cur.next;
    }
    fprintf(dest, "\n");
}

int main() {
    FILE *inp_file = fopen("polynomial_derivative.txt", "rb");
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

    char *trav = data;
    int  count = 0, max = 0;
    for(; *trav; trav++) {
        if (!*trav || *trav == '\n' || *trav == '\r') {
            max = count > max ? count : max;
            count = 0;
        }
        count += *trav == 'x';
    }

    linked_list derivative;
    init_list(&derivative, max);

    lexer lex;
    lex.data = data;

    FILE *out_file = fopen("polynomial_derivative_out.txt", "wb");
    while (*lex.data) {
        parse_line(&lex, &derivative);
        write_derivative(out_file, derivative);
    }
    fclose(out_file);
}
