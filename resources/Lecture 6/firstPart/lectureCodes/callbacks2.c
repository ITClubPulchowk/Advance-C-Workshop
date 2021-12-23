#include<stdio.h>
#include<stdlib.h>


// creating type of a pointer to a function that takes 2 integers and returns an integer, that we will pass later as a callback function  
typedef int (*callback)(int, int);

// a function with same signature signature as above callback type, returns 1 if the first int is greater than second and 0 otherwise
int ascending(int a, int b)
{
    return a > b;
}
 
 // a function with same signature signature as above callback type, returns 1 if the first int is smaller than second and 0 otherwise
int descending(int a, int b)
{
    return a < b;
}

//generic insertion sort that can sort an array in ascending as well as descending order, based on callback function it recieves for comparison
// it take an inegeer array to sort, length of the array and comparison function as input
void insertionSort(int *array, int n, callback compare)
{
    int i, j, key;
    // iterate from position 1 to n-1 of an array of length n
    for(j=1; j<=n-1;j++)
    {   
        // select array[j] as key
        key=array[j];
        i=j-1;
        //iterate over predecessors of the key element from the last to beginnning
        //for sorting in ascending order, keep shifting the predecessor to right till the predecessor element is greater than the key  
        //similarly for descending, keep shifting the predecessor to right till the predecessor element is smaller than the key 
        while(i >=0 && compare(array[i], key))
        {
            array[i+1]=array[i];
            i=i-1;
        }
        // shift the key element immediately after the first smaller predecessor we hit on moving backwards for ascending 
        // and after the first greater for descending
        array[i+1]=key;
    }
}
// we can observe that the same code can work for both descending and ascending sorting, just by making the comaprison a generic function
// the callback function needs to be a comparison function that returns 1 if the first element is greater than second else 0 for ascending sorting 
// and the other way around for descending
// we have aleady defined those compariosn function above, so we can just pass the one we need to make the function work for both sortings
 
int main(void)
{
    int i;
    int choice;
    int array[10] = {22,66,55,11,99,33,44,77,88,0};
    printf("ascending 1: descending 2 \n");
    printf("enter your choice = ");
    scanf("%d",&choice);
    switch(choice)
    {
        case 1:
            // pass 'ascending' function for sorting in ascending order as described above
            insertionSort(array,10, ascending);
            break;
        case 2:
            // pass 'descending' function for sorting in descending order as described above
            insertionSort(array,10, descending);
            break;
        case 3:
            exit(0);
        default:
            printf("no such option\n");
    }
 
    printf("after insertionSort\n");
    for(i=0;i<10;i++)
        printf("%d\t", array[i]);
    printf("\n");
     return 0;
}