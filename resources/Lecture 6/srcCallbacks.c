#include <stdio.h>
/*
add code here if needed
*/

int main(){

    int a, b, operation;

    typedef int (*calcPtr) (int, int);
    calcPtr ptr;

    printf("Enter int a: \n");
    scanf("%d", &a);

    printf("Enter int b: \n");
    scanf("%d", &b);

    printf("Choose:\n 1 for addition\n 2 for subtraction\n 3 for multiplication\n 4 for division\n");
    scanf("%d", &operation);

    switch (operation)
    {
     case 1:
    /*
    add code here if needed
    */ 
        calculator(a, b, ptr);
     case 2:
    /*
    add code here if needed
    */
        calculator(a, b, ptr);
     case 3:
    /*
    add code here if needed
    */
        calculator(a, b, ptr);
     case 4:
    /*
    add code here if needed
    */  
        calculator(a, b, ptr);
     default:
        printf("no such option\n");   
    }

    return 0;
}

/*
add code here if needed
*/

