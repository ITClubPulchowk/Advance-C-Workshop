#ifndef MERGELOCIC
#define MERGELOGIC 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void mergesort(int * array, int length)
{
    if(length < 2)
        return;
    if(length ==2 )
    {
        if(array[0] > array[1])
        {
            int temp = array[0];
            array[0] = array[1];
            array[1] = temp;
        }    
        return ;
    }
    
    int l1 = length/2;
    int l2 = length-length/2;

    int * array1 = array;
    int * array2 = array +length/2;
    
    mergesort(array1, l1);
    mergesort(array2, l2);

    //merge
    int j = 0, k = 0;
    int sorted[length];
    for(int i = 0; i<length; i++)
    {
        // element from second splitted array is selected when
        // 1. Either first splitted array is finished
        // 2. Or element of second splitted array is smaller, when second splitted array is not finished
        if(j>=l1 || ((array2[k] < array1[j]) && !(k == l2)))
        {
            sorted[i] = array2[k];
            k++;
        }
        else
        {
            sorted[i] = array1[j];
            j++;
        }
    }

    memcpy(array,sorted,length*sizeof(int));
    return;
}

void displaylist(char * s, int * array, int size)
{
    *s = 0;
    // sprintf(s,"{",);
    sprintf(s, "%s%d",s, *array);
    for (int i  = 1; i<size; i++)
    {
        sprintf(s, "%s,%d",s,array[i]);
    }
    // sprintf(s,"%s}",s);
    return ;
}

void strcat_using_realloc(char * dest, int *slen, int * mlen, const char * input, const int inputlen)
{
    if(*slen + inputlen + 1 >= *mlen) // current length + new string + comma (+ null) > available space
    {
        do
        {
            *mlen *=2;
        } while(*slen + inputlen >= *mlen) ;

        dest = realloc(dest,*mlen);
    }
    *slen += inputlen;
    strncat(dest,input,inputlen);
}

char * get_string_list(int * array, int size)
{
    /* !! rules for changing this code: !!
        * string_length must be modified as soon as length of s is change
        * memory_length must be modified only when memory is changed
        * testlen is calculated during creation of test string
    */
    // actual string to be generated
    char *s = malloc(16*sizeof(char));
    *s = 0; // first character is null (of an empty string)
    int string_length = 0;
    int memory_length = 16;

    // test string use for concatenation of individual numbers
    char test[12];
    int testlen = 0;

    // generating string of number in a test string, and counting the letters
    sprintf(test,"%d%n",*array, &testlen);
    strcat_using_realloc(s, &string_length, &memory_length,  test, testlen);
    // s[string_length++]=','


    sprintf(s, "%s%d",s, *array);
    for (int i  = 1; i<size; i++)
    {
        sprintf(test,"%d%n",array[i],&testlen);
        s[string_length++] = ',';
        s[string_length++] = 0;
        strcat_using_realloc(s, &string_length, &memory_length,  test, testlen);
    }
    // sprintf(s,"%s}",s);
    return s;
}

int maun()
{
    char displaystring[1000];
    int array[] = {1,3,5,7,9,8,6,4,2,0};
    int size = sizeof(array)/sizeof(array[0]);

    displaylist(displaystring,array, size);
    printf("\nBefore sorting:\n{%s}\n\n", displaystring);

    mergesort(array,size);

    displaylist(displaystring,array, size);
    printf("\nAfter sorting:\n{%s}\n\n", displaystring);

    return 0;    
}

#endif