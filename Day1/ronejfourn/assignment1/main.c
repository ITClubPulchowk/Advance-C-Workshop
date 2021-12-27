#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    char *data;
    int number;
} lexer;

void merge_sort(int *arr, int *buf, int arr_len){
    if (arr_len <= 1) return;

    int n = arr_len / 2, m = arr_len - arr_len / 2;
    int *left     = arr, *right     = arr + n;
    int *left_buf = buf, *right_buf = buf + n;

    merge_sort(left , left_buf , n);
    merge_sort(right, right_buf, m);
    int i = 0, j = 0, k = 0;

    while (i < n && j < m) {
        if (left_buf[i] < right_buf[j]) {
            arr[k++] = left_buf[i++];
        } else {
            arr[k++] = right_buf[j++];
        }
    }

    if (i < n) {
        for (; i < n; i ++)
            arr[k++] = left_buf[i];
    } else {
        for (; j < m; j ++)
            arr[k++] = right_buf[j];
    }
    memcpy(buf, arr, arr_len * sizeof(int));
}

void get_next_num(lexer *lex) {
    lex->number = 0;
    for (; *lex->data >= '0' && *lex->data <= '9'; lex->data++)
        lex->number = lex->number * 10 + *lex->data - '0';
}

int make_array_and_buffer(int *arr, int *buf, lexer *lex) {
    int num_count = 0;
    while (*lex->data && *lex->data != '\n' && *lex->data != '\r') {
        get_next_num(lex);
        lex->data += *lex->data == ',';
        arr[num_count] = lex->number;
        buf[num_count] = lex->number;
        num_count ++;
    }
    lex->data += *lex->data != 0;
    lex->data += *lex->data == '\n';
    return num_count;
}

int make_string(int *arr, int len, char *out) {
    int off = 0;
    for (int i = 0; i < len - 1; i ++) {
        int tmp = sprintf(out, "%d,", arr[i]);
        out += tmp;
        off += tmp;
    }
    off += sprintf(out, "%d\n", arr[len - 1]);
    return off;
}

int main() {
    FILE *inp_file = fopen("mergesort_input.csv", "rb");
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

    lexer lex = {data, 0};
    int *array, *buffr;

    char *trav = data;
    int count = 1, max = 1;
    int crlf = 0; //fuck you windows
    for(; *trav; trav++) {
        if (!*trav || *trav == '\n' || *trav == '\r') {
            crlf += *trav == '\r' && *(trav + 1) == '\n';
            max = count > max ? count : max;
            count = 1;
        }
        count += *trav == ',';
    }
    max = count > max ? count : max;

    int *scratch = malloc(2 * max * sizeof(int));
    int out_size = size - crlf;
    char *out_str = malloc(out_size);
    out_str[out_size] = 0;
    trav = out_str;

    while (*lex.data) {
        array = scratch;
        buffr = scratch + max;
        int len = make_array_and_buffer(array, buffr, &lex);
        merge_sort(array, buffr, len);
        trav += make_string(array, len, trav);
    }

    FILE *out_file = fopen("mergesort_output.csv", "wb");
    fwrite(out_str, 1, out_size, out_file);
    fclose(out_file);

    return 0;
}
