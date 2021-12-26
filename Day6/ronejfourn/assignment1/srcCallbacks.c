#include <stdio.h>
/*
add code here if needed
*/

int add (int a, int b) {
    printf("addition of: ");
    return a + b;
}
int sub (int a, int b) {
    printf("subtraction of: ");
    return a - b;
}
int mul (int a, int b) {
    printf("multiplication of: ");
    return a * b;
}
int div (int a, int b) {
    printf("division of: ");
    return a / b;
}
int calculator(int a, int b, int(*op)(int, int)) {
    int res = op(a, b);
    printf("%d and %d is %d\n", a, b, res);
    return res;
}

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
        ptr = add;
        calculator(a, b, ptr);
        break;
     case 2:
    /*
    add code here if needed
    */
        ptr = sub;
        calculator(a, b, ptr);
        break;
     case 3:
    /*
    add code here if needed
    */
        ptr = mul;
        calculator(a, b, ptr);
        break;
     case 4:
    /*
    add code here if needed
    */
        ptr = div;
        calculator(a, b, ptr);
        break;
     default:
        printf("no such option\n");
        break;
    }

    return 0;
}

/*
add code here if needed
*/

