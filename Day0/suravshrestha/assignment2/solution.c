/*
    C program to evaluate postfix expressions from a given file and store the results in a new file

    Postfix Notation is popularly known as Reverse Polish Notation (RPN) or Polish postfix notation
    RPN: https://en.wikipedia.org/wiki/Reverse_Polish_notation

    Algorithm for evaluating RPN:
    1.  Create a stack to store the operands
    2.  Loop through every element of the expression from left to right:
        a)  If the element is a number, push it to the stack
        b)  If the element is an operator, pop the last two operands of the stack and evaluate them with the operator.
            Push the result to the stack.
    3.  When the expression is empty, the last element of the stack is the result.

    Algorithm reference: 
    https://www.includehelp.com/c/evaluation-of-postfix-expressions-using-stack-with-c-program.aspx

    Major limitations:
    1.  It can't evaluate negative operands.
    2.  It can evaluate only the 4 basic operators: +, -, *, /
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILENAME "expressions.txt"
#define OUTPUT_FILENAME "results.txt"

float eval_postfix(char *exp)
{
    // Create a stack of size excluding whitespaces from the expression for storing operands
    float *stack = (float *)calloc(strlen(exp) / 2, sizeof(float));

    int top_idx = -1;

    for (int i = 0; exp[i]; i++)
    {
        char ch = exp[i];
        if (ch == '+' || ch == '-' || ch == '*' || ch == '/')
        {
            float res;

            // Get the last two operands from the stack
            float a = *(stack + top_idx - 1); // Second last operand
            float b = *(stack + top_idx);     // Last operand

            switch (ch)
            {
            case '+':
                res = a + b;
                break;
            case '-':
                res = a - b;
                break;
            case '*':
                res = a * b;
                break;
            case '/':
                res = a / b;
                break;
            default:
                break;
            }

            // Remove the last two operands and append the result
            top_idx -= 1;
            stack[top_idx] = res;
            stack[top_idx + 1] = '\0';
        }
        else if (ch > 47 && ch < 58) // ASCII code of '0' to '9'
        {
            // isdigit

            char prev_char = exp[i - 1];
            if (prev_char == 32 || i == 0 ||
                prev_char == '+' || prev_char == '-' || prev_char == '*' || prev_char == '/')
            {
                // Previous character is a whitespace or first character or operator
                top_idx++;
            }

            // Conversion of string integer to corresponding integer
            stack[top_idx] = stack[top_idx] * 10 + ch - '0';
        }
    }

    // The only remaining element in the stack is the final result
    return stack[top_idx];
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

    int buffer_length = 255;
    char expression[buffer_length];

    // Get the expressions line by line from the input file
    while (fgets(expression, buffer_length, fp_input))
    {
        expression[strlen(expression) - 1] = '\0'; // Remove the ending newline from the expression
        fprintf(fp_output, "%f\n", eval_postfix(expression));
    }

    fclose(fp_input);
    fclose(fp_output);

    return 0;
}
