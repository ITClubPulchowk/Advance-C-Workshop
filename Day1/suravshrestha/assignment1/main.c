#include <stdio.h>
#include <stdlib.h>

#define INPUT_FILENAME "mergesort_input.csv"
#define OUTPUT_FILENAME "mergesort_output.csv"

// Dynamic array
typedef struct
{
    int *array;
    size_t used; // Length of the array
    size_t size;
} Array;

void init_array(Array *arr, size_t initial_size)
{
    arr->array = malloc(initial_size * sizeof(int));
    arr->used = 0;
    arr->size = initial_size;
}

void append_to_array(Array *arr, int element)
{
    // arr->used is the number of used entries, because arr->array[arr->used++] updates arr->used only *after* the array has been accessed.
    // Therefore arr->used can go up to arr->size
    if (arr->used == arr->size)
    {
        arr->size *= 2;
        arr->array = realloc(arr->array, arr->size * sizeof(int));
    }

    arr->array[arr->used++] = element;
}

void free_array(Array *arr)
{
    free(arr->array);
    arr->array = NULL;
    arr->used = arr->size = 0;
}

// Function to merge two subarrays after sorting them
void merge(int arr[], int left_idx, int mid_idx, int right_idx)
{
    int i, j, k;

    int a = mid_idx - left_idx + 1; // Size of the first subarray
    int b = right_idx - mid_idx;    // Size of the second subarray

    // Temporary arrays to store the two subarrays
    int arr1[a], arr2[b];

    // Copy data to temp arrays arr1 and arr2
    for (i = 0; i < a; i++)
    {
        arr1[i] = arr[left_idx + i];
    }

    for (j = 0; j < b; j++)
    {
        arr2[j] = arr[mid_idx + 1 + j];
    }

    i = 0;        // Initial index of first subarray
    j = 0;        // Initial index of second subarray
    k = left_idx; // Initial index of merged subarray

    // Compare the two subarrays and merge them
    while (i < a && j < b)
    {
        if (arr1[i] <= arr2[j])
        {
            arr[k] = arr1[i];
            i++;
        }
        else
        {
            arr[k] = arr2[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of arr1, if there are any (only one element is left)
    while (i < a)
    {
        arr[k] = arr1[i];
        i++;
        k++;
    }

    // Copy the remaining elements of arr2, if there are any (only one element is left)
    while (j < b)
    {
        arr[k] = arr2[j];
        j++;
        k++;
    }
}

void sort(int arr[], int left_idx, int right_idx)
{
    // Recursive call until only one element is left in the array
    if (left_idx < right_idx)
    {
        // To handle overflow, mid-point is calculated this way
        int mid_idx = left_idx + (right_idx - left_idx) / 2;

        // Divide the array passed into two halves and sort them
        sort(arr, left_idx, mid_idx);
        sort(arr, mid_idx + 1, right_idx);

        // Merge the two sorted halves
        merge(arr, left_idx, mid_idx, right_idx);
    }
}

int main()
{
    FILE *fp_input = fopen(INPUT_FILENAME, "rb");
    FILE *fp_output = fopen(OUTPUT_FILENAME, "wb");

    if (fp_input == NULL)
    {
        printf("%s can't be accessed", INPUT_FILENAME);
        exit(1);
    }

    if (fp_output == NULL)
    {
        printf("%s can't be accessed", OUTPUT_FILENAME);
        exit(1);
    }

    Array arr;
    init_array(&arr, 25); // Initially, each line is assumed to have 25 numbers
    int num = 0;

    char ch;
    while (1)
    {
        ch = fgetc(fp_input);

        if (ch == EOF)
        {
            break;
        }
        else if (ch >= '0' && ch <= '9')
        {
            // Conversion of string integer to the corresponding integer
            num = num * 10 + ch - '0';
        }
        else if (ch == '\n')
        {
            // End of individual line of numbers
            // Sort the array of numbers and append to the output file them in csv format
            sort(arr.array, 0, arr.used - 1);
            for (int i = 0; i < arr.used; i++)
            {
                if (i == arr.used - 1)
                {
                    // Last element of the array
                    fprintf(fp_output, "%d\n", arr.array[i]);
                }
                else
                {
                    fprintf(fp_output, "%d,", arr.array[i]);
                }
            }

            free_array(&arr);
            init_array(&arr, 25); // Initially, each line is assumed to have 25 numbers

            num = 0;
        }
        else
        {
            // Comma
            append_to_array(&arr, num); // Automatically resizes if necessary

            num = 0;
        }
    }

    fclose(fp_input);
    fclose(fp_output);

    return 0;
}
