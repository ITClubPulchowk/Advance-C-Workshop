//simple demonstration of callbacks
#include<stdio.h>

//creating type of a pointer to a function that talkes nothing and returns nothing 
typedef void (*callback) (void);

// the callback function
// a function that takes nothimg and returns nothing, simply prints some text, that we will pass later as a callback function  
void A()
{   
    printf("I am from function A\n");

}

// a function that takes in a function pointer of type "callback" and calls the function to ponted by it
void B(callback c)
{

    printf("Hello from function B\n");
    (*c) (); // callback to A
    printf("Hello from function B\n");
}
  
int main()
{   //assignment of a pointer to function 'A' to variable 'C' of type 'callback'(function pointer that takes as well as returns nothing) 
    callback C = &A;
      
    // callback registration
    // calling function B and passing address of the function A as argument (registering A as the callabck function that B needs) 
    B(C);
  
   return 0;
}