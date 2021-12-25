
// code to demonstrate use of typedefs for function pointers in C
// use of typedef helps to name a type of function pointer associated with a signature of function with a particular 
// return type and function parameters 
#include<stdio.h>

// a function that recieves an int n and displays integers from 1 to n
void print_to_n(int n)
{
printf("from print to n\n");
for (int i = 1; i <= n; ++i)
printf("%d\n", i);
}

// a function that recieves an int n displays the same
void print_n(int n)
{
printf("from print n\n");
printf("%d\n", n);
}

//we can see that both these functions take an int as parameter and return nothing. Both of there declarations have same signature
// without typedef, we would create and use pointers to these functions in the following way

/*
int main(){

void (*p1)(int) = &print_to_n;
void (*p2)(int) = &print_n;
 
(*p1)(10);
(*p2)(5);
return 0;

}
*/


// the same is done with typedef as below. This is definately neater and a more convinient way to follow, when we need to do this for many functions



// This creates a type, named printer_t for a pointer to a function that takes a single int argument and returns nothing
//To use this, we can create a variable of the type and assign to it a pointer to one of the functions with the same signature


/*
typedef void (*printer_t) (int);

int main(){

printer_t p1 = &print_to_n;
printer_t p2 = &print_n;

(*p1)(10);
(*p2)(5);
return 0;

}

//uncomment any of the two blocks of main function at a time to see the output