#include <stdio.h>
#include "bump.c"

/*
 * less efficient implementation of merge sort
 * to get a better comparison between bump and malloc
 */

void merge_sort(int *arr, int arr_len){
    if (arr_len <= 1) return;

    int n = arr_len / 2, m = arr_len - arr_len / 2;
    int *left  = bump_aligned(n * sizeof(int), sizeof(int)),
        *right = bump_aligned(m * sizeof(int), sizeof(int));
    for (int i = 0; i < n; i ++)
        left[i] = arr[i];
    for (int i = n; i < arr_len; i ++)
        right[i - n] = arr[i];

    merge_sort(left , n);
    merge_sort(right, m);
    int i = 0, j = 0, k = 0;
    int rem = 0;

    while (i < n && j < m) {
        if (left[i] < right[j]) {
            arr[k++] = left[i] ;
            i ++;
        } else {
            arr[k++] = right[j];
            j ++;
        }
    }

    if (i < n) {
        for (; i < n; i ++)
            arr[k++] = left[i];
    } else {
        for (; j < m; j ++)
            arr[k++] = right[j];
    }
    debump(m * sizeof(int));
    debump(n * sizeof(int));
}

typedef struct {
    char *data;
    int number;
} lexer;

void get_next_num(lexer *lex) {
    lex->number = 0;
    for (; *lex->data >= '0' && *lex->data <= '9'; lex->data++)
        lex->number = lex->number * 10 + *lex->data - '0';
}

int make_array(int *arr, lexer *lex) {
    int num_count = 0;
    while (*lex->data && *lex->data != '\n' && *lex->data != '\r') {
        get_next_num(lex);
        lex->data += *lex->data == ',';
        arr[num_count] = lex->number;
        num_count ++;
    }
    lex->data += *lex->data != 0;
    lex->data += *lex->data == '\n';
    return num_count;
}

int main() {
    init_bump_context(megabytes(512));
    FILE *inp_file = fopen("mergesort_input.csv", "rb");
    if (!inp_file) {
        printf("What file?\n");
        return -2;
    }
    fseek(inp_file, SEEK_SET, SEEK_END);
    int size = ftell(inp_file);
    rewind(inp_file);

    char *data = bump(size + 1);
    fread(data, 1, size, inp_file);
    data[size] = 0;
    fclose(inp_file);

    lexer lex = {data, 0};

    char *trav = data;
    int count = 1, max = 1;
    for(; *trav; trav++) {
        if (!*trav || *trav == '\n' || *trav == '\r') {
            max = count > max ? count : max;
            count = 1;
        }
        count += *trav == ',';
    }
    max = count > max ? count : max;

    int *array = bump_aligned(max * sizeof(int), sizeof(int));

    while (*lex.data) {
        int len = make_array(array, &lex);
        merge_sort(array, len);
    }

    end_bump_context();
    return 0;
}
