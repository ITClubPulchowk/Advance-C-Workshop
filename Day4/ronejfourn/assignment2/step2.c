#include "bump.c"
#include "threading.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

typedef int (*fn) (const void *, const void *);

void merge(uint32_t *array, uint32_t *lhalf, uint32_t *rhalf, uint32_t n, uint32_t m, fn order) {
    uint32_t i = 0, j = 0, k = 0;

    while (i < n && j < m) {
        if (order(&rhalf[j], &lhalf[i]))
            array[k++] = lhalf[i++];
        else
            array[k++] = rhalf[j++];
    }

    for (; i < n; i ++)
        array[k++] = lhalf[i];
    for (; j < m; j ++)
        array[k++] = rhalf[j];
}

void mergesort(uint32_t *arr, uint32_t *buf, uint32_t len, fn order) {
    if (len <= 1) return;
    uint32_t n = len / 2, m = len - n;
    uint32_t *aleft = arr, *aright = arr + n;
    uint32_t *bleft = buf, *bright = buf + n;

    mergesort(bleft , aleft , n, order);
    mergesort(bright, aright, m, order);

    merge(arr, bleft, bright, n, m, order);
}

typedef struct {
    uint32_t *arr;
    uint32_t *buf;
    uint32_t  len;
    fn order;
} mt_ms_args;

typedef struct {
    uint32_t *array;
    uint32_t *lhalf;
    uint32_t *rhalf;
    uint32_t n;
    uint32_t m;
    fn order;
} mt_merge_args;

int mt_mergesort(void *dat) {
    mt_ms_args *args = dat;
    mergesort(args->arr, args->buf, args->len, args->order);
    return 0;
}

int mt_merge(void *dat) {
    mt_merge_args *args = dat;
    merge(args->array, args->lhalf, args->rhalf, args->n, args->m, args->order);
    return 0;
}

int ascending(const void *a, const void *b) {
    uint32_t na = *(uint32_t *)a;
    uint32_t nb = *(uint32_t *)b;
    return (na < nb);
}

int descending(const void *a, const void *b) {
    uint32_t na = *(uint32_t *)a;
    uint32_t nb = *(uint32_t *)b;
    return (na > nb);
}

typedef struct lexer {
    char *data;
    uint32_t token;
} lexer;

void get_next(lexer *lex) {
    while (*lex->data && isspace(*lex->data)) lex->data ++;
    if (*lex->data == 'a' || *lex->data == 'd') {
        lex->token = *lex->data;
        lex->data ++;
    } else {
        lex->token = 0;
        for (; *lex->data >= '0' && *lex->data <= '9'; lex->data ++)
            lex->token = lex->token * 10 + *lex->data - '0';
    }
}

int main() {
    init_bump_context(megabytes(2000));
    FILE *ifile = fopen("input.txt", "rb");
    if (!ifile) {
        printf("input.txt not found\n");
        return -1;
    }
    fseek(ifile, 0, SEEK_END);
    int fsize = ftell(ifile);
    rewind(ifile);

    char *fbuf  = bump(fsize + 1);
    fbuf[fsize] = 0;
    fread(fbuf, 1, fsize, ifile);
    fclose(ifile);
    lexer lex;
    lex.data = fbuf;

    get_next(&lex);
    uint32_t arr_len = lex.token;
    uint32_t *arr = bump_aligned(arr_len * sizeof(*arr), sizeof(*arr));
    uint32_t *buf = bump_aligned(arr_len * sizeof(*buf), sizeof(*buf));

    get_next(&lex);
    fn order = lex.token == 'a' ? ascending : descending;

    for (uint32_t i = 0; i < arr_len; i ++) {
        get_next(&lex);
        arr[i] = lex.token;
        buf[i] = lex.token;
    }

    uint32_t n = arr_len / 2, m = arr_len - n;

    thrd_t lthread, rthread, mthread;
    mt_ms_args largs = {
        .arr = buf,
        .buf = arr,
        .len = n,
        .order = order
    };
    mt_ms_args rargs = {
        .arr = buf + n,
        .buf = arr + n,
        .len = m,
        .order = order
    };
    mt_merge_args margs = {
        .array = arr,
        .lhalf = buf,
        .rhalf = buf + n,
        .n = n,
        .m = m,
        .order = order
    };

    thrd_create(&lthread, mt_mergesort, &largs);
    thrd_create(&rthread, mt_mergesort, &rargs);
    thrd_join(lthread, NULL);
    thrd_join(rthread, NULL);
    thrd_create(&mthread, mt_merge    , &margs);
    thrd_join(mthread, NULL);

    /* qsort(buf, arr_len, sizeof(uint32_t), order); */
    /* for (int i = 0; i < arr_len; i ++) { */
    /*     if (arr[i] != buf[i]) { */
    /*         printf("Oopsie :P %d != %d\n", arr[i], buf[i]); */
    /*         return 0; */
    /*     } */
    /* } */

    FILE *ofile = fopen("sorted.txt", "wb");
    for (int i = 0; i < arr_len - 1; i ++)
        fprintf(ofile, "%d ", arr[i]);
    fprintf(ofile, "%d", arr[arr_len - 1]);
    fclose(ofile);

    end_bump_context();
}
