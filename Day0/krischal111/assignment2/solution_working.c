#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define expressionfile "expressions.txt"
#define resultfile "result.txt"

#define solveshow 1

/* datas and structures used */
typedef struct backchainf
{
    float data;
    struct backchainf * prevchain;
} backchainf;

typedef struct Float_stack
{
    backchainf * chaintop;
    int count;
}Float_stack;

/* functions used */
void test();
int getnum(char**myexp);
float solve_postfix(char * myexp);

/* boolean returning inline functions*/
inline int is_this_char_number(char * c);
inline int is_this_char_operator(char *c);
inline int is_this_char_space(char *c);
inline int is_this_char_newlinechar(char *c);
inline int is_this_char_null(char *c);
/* and their definitions */
 int is_this_char_number(char * c){return ('0' <= *c && *c <= '9' );}
 int is_this_char_operator(char *c)
{ return ('+' == *c ||  '-' == *c ||  '*' == *c ||  '/' == *c)? *c : 0; }
 int is_this_char_space(char *c){return (' ' == *c);}
 int is_this_char_newlinechar(char *c){return (*c == '\n' || *c == '\r');}
 int is_this_char_null(char *c){return (*c == '\0');}

/* stack functions declarations */
Float_stack initialize_float_stack();
void pushf( Float_stack * s, float data);
float popf( Float_stack * s);
float peekf( Float_stack * s);
void deleteentirestack( Float_stack *s);

/* stack inline functions */
inline int hassomethingf( Float_stack *s)  ;
inline int isemptyf( Float_stack *s)       ;
inline int howmanydatas(Float_stack *s)    ;
/* and their definitions */
 int hassomethingf( Float_stack *s)  {return   s->count ;}
 int isemptyf( Float_stack *s)       {return !(s->count);}
 int howmanydatas(Float_stack *s)    {return   s->count ;}

/* stack function definitions */
Float_stack initialize_float_stack()
{
    return (Float_stack){.chaintop = NULL, .count=0};
}

void pushf(Float_stack * s, float data)
{
    backchainf* newbackchain = (backchainf *) malloc(sizeof(backchainf));

    if(newbackchain)
    {
        newbackchain->data = data;
        
        newbackchain->prevchain = s->chaintop; // assuming empty stack is created with a chaintop of NULL
        
        s->count++;
        s->chaintop = newbackchain;
    }
}

float popf( Float_stack * s)
{
    float data;
    if(hassomethingf(s)) // if stack contains some data
    {
        data = s->chaintop->data; // data is stored in top chain

        // we need top chain's previous chain, before top chain is freeed
        backchainf * prevchain = (backchainf *)s->chaintop->prevchain; 
        free(s->chaintop);

        s->count--;
        s->chaintop = prevchain;
    }
    return data;
}

float peekf( Float_stack *s)
{
    return s->chaintop->data;
}

void deleteentirestack(Float_stack *s)
{
    while(hassomethingf(s))
    {
        popf(s);
    }
}

/* function definitions*/
int getnum(char**myexp)
{
    char numberstring[21] = "                   ";
    for(int i = 0; i<20; ++i)
    {
        char c = **myexp;
        if(c < '0' || c > '9') // break loop when numbers are finished, leaving pointer pointing other than number
            break;
        numberstring[i] = c;   // add all the number into a string
        (*myexp)++;
    }
    // (*myexp)--;
    return atoi(numberstring); // convert the numeric string to an actual number
}

float solve_postfix(char * myexp)
{
    if(solveshow)
    printf("\nTrying to solve: %s\n", myexp);

    Float_stack myfloatstack = initialize_float_stack();
    while(!is_this_char_null(myexp))
    {
        while(is_this_char_space(myexp))
            myexp++;    // if first character of string is space, erase it, somehow.
        
        
        if(is_this_char_number(myexp))
        {
            pushf(&myfloatstack, (float) getnum(&myexp));
            continue;
        }

        char opr = is_this_char_operator(myexp);
        if(opr)
        {
            float b = popf(&myfloatstack);
            float a = popf(&myfloatstack);

            switch(opr)
            {
                case '+':
                pushf(&myfloatstack, a+b);
                break;

                case '-':
                pushf(&myfloatstack, a-b);
                break;

                case '*':
                pushf(&myfloatstack, a*b);
                break;

                case '/':
                pushf(&myfloatstack, a/b);
                break;

                default:
                break;
            }
            if(solveshow) printf("%.2f %c %.2f = %.2f\n",a,opr,b,peekf(&myfloatstack));
            myexp++;    // points to another than operator
            continue;
        }

        myexp++; // just in case, the character is neither a num nor an operator
    }

    if(solveshow)
    printf("The solution is %.2f\n",peekf(&myfloatstack));

    float a = popf(&myfloatstack); // the last value in stack is the answer
    deleteentirestack(&myfloatstack); // just in case when the expression causes to do some funny things
    return a;
}

int main()
{
    test();

    FILE * myfile = fopen(expressionfile, "rb");
    if(myfile)
    {
		FILE * resfile = fopen(resultfile, "wb");
        void * something;
        while(something)
        {
            char line[200];
            something = fgets(line, 200, myfile);
            fprintf(resfile, "%f\n", solve_postfix(line));         
        }
        fclose(resfile);
    }
    fclose(myfile);
    return 0;
}

void test()
{
    assert(solve_postfix("2 2+") == (float) 4);
    assert(solve_postfix("3 3 +") == (float) 6);
    assert(solve_postfix("2 3 4 + *") == (float) 14);
    assert(solve_postfix("8 2 + 15 -") == (float) (-5));
}