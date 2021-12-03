#include <stdio.h>
#include <stdlib.h>
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
        R[j] = arr[m + j + 1];
    i = 0;
    j = 0;
    while (i < n1 && j < n1)
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
    free(R);
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
    int *arr; char *str; int file_size,array_size=1;
    char temp[100];
    int i=0,j=0,k=0;
    fp=fopen(input_file,"rb");
    if(!fp)
    {
        printf("No file");
        return 1;
    }
    fseek(fp, 0L, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    str=malloc(sizeof(*str)*(file_size+1));
    fread(str,file_size,1,fp);
    fclose(fp);
    str[file_size]=0;
    while(str[i]!='\0')
    {
        if(str[i]==',')
        array_size++;
        i++;
    }
    arr=calloc(1,sizeof(*arr)*array_size);
    i=0;
    while (1)
    {
        if(str[i]!=',' )
        {
            temp[j]=str[i];
            j++;
        }
        else
        {
            temp[j]='\0';
            j=0;
            arr[k]=atoi(temp);
            k++;
        }
        i++;
        if(str[i]=='\0')
        {
        arr[k]=atoi(temp);
        break;
        }
    }

    merge_sort(arr,0,array_size);
    i=j=k=0;

    i += snprintf(str+i,file_size-i, "%d",arr[0]);
    for ( k = 1; k < array_size; k++)
    {
        i += snprintf(str+i,file_size-i+1, ",%d",arr[k]);
    }
    fp=fopen(output_file,"wb");
    fwrite(str,i,1,fp);
    return 0;
}
