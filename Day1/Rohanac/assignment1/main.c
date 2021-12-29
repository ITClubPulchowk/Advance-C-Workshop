#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define input_file "mergesort-input.csv"
#define output_file "mergesort-output.csv"
void merge(int arr[], int l, int m, int r)
{
    int *L, *R;
    int n1 = m - l + 1, n2 = r - m;
    int i, j, k = l;
    L = malloc(sizeof(*L) * (n1 + 1));
    R = malloc(sizeof(*R) * (n2 + 1));
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + j+1 ];
    i = 0;
    j = 0;
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }
    free(L);
    L=NULL;
    free(R);
    R=NULL;
}
void merge_sort(int arr[], int l, int r)
{
    if (l < r)
    {
        int m = (l + r) / 2;
        merge_sort(arr, l, m);
        merge_sort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}
int main()
{
    FILE *fp;
    int *arr;
    char *str;
    int exit = 0;
    int file_size = 0, array_size = 0, p = 0;
    char *strtmp;
    char temp[100];
    int abs_length = 0;
    int i = 0, j = 0, k = 0;
    fp = fopen(input_file, "rb");
    if (!fp)
    {
        printf("No file");
        return 1;
    }
    fseek(fp, 0L, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    str = malloc(sizeof(*str) * (file_size + 1));
    fread(str, file_size, 1, fp);
    fclose(fp);
    strtmp = malloc(sizeof(*strtmp) * (file_size + 1));
    str[file_size] = 0;
    while (!exit)
    {
        p=i;
        while (1)
        {
            if (str[p] == ',')
                array_size++;
            else if (str[p] == '\n' || str[p] == '\0')
                break;
            p++;
        }
        p = 0;
        arr = malloc(sizeof(*arr) * (array_size + 1));
        k = 0;
        while (1)
        {
            if (str[i] != ','&&str[i] != '\n'&&str[i] != '\0')
            {
                temp[j] = str[i];
                j++;
            }
            else if (str[i] == '\n')
            {
                temp[j] = '\0';
                j = 0;
                arr[k] = atoi(temp);
                i++;
                k++;
                break;
            }
            else if (str[i] == '\0')
            {
                temp[j] = '\0';
                j = 0;
                arr[k] = atoi(temp);
                k++;
                exit = 1;
                i++;
                break;
            }
            else if(str[i]=='\r')
            {}
            else
            {
                temp[j] = '\0';
                j = 0;
                arr[k] = atoi(temp);
                k++;
            }
            i++;
        }

        merge_sort(arr, 0, array_size);
        j = k = 0;
        abs_length += snprintf(strtmp + abs_length, file_size - abs_length, "%d", arr[0]);
        for (k = 1; k <= array_size; k++)
        {
            abs_length += snprintf(strtmp + abs_length, file_size - abs_length + 1, ",%d", arr[k]);
        }
        j = k = 0;
        if(!exit)
        abs_length += snprintf(strtmp + abs_length, file_size - abs_length, "\r\n");
        else
        abs_length += snprintf(strtmp + abs_length, file_size - abs_length, "\0");

        free(arr);
        arr=NULL;
        array_size = 0;
        temp[0] = '\0';
    }
    fp = fopen(output_file, "wb");
    fwrite(strtmp, abs_length, 1, fp);
    return 0;
}