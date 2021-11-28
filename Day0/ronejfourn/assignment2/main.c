#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define STK_SIZE 8

typedef struct {
    float *stack;
    int cap, ptr;
} stack;

stack create_stack() {
    stack ret;
    ret.stack = malloc(sizeof(*ret.stack) * STK_SIZE);
    ret.cap = STK_SIZE;
    ret.ptr = -1;
    return ret;
}

void push(stack *stack, unsigned int val){
    stack->ptr += 1;
    if (stack->ptr >= stack->cap){
        stack->cap *= 2;
        stack->stack = realloc(stack->stack, sizeof(*stack->stack) * stack->cap);
    }
    stack->stack[stack->ptr] = val;
}

int main(int argc, char *argv[]){
    FILE *inp_file = fopen("expressions.txt", "rb");
    if (!inp_file){
        printf("What file?\n");
        return -2;
    }
    fseek(inp_file, SEEK_SET, SEEK_END);
    int size = ftell(inp_file);
    rewind(inp_file);

    char *buffer = malloc(size);
    fread(buffer, 1, size, inp_file);
    fclose(inp_file);

    stack stack = create_stack();
    int pos = 0;

    FILE *out_file = fopen("results.txt", "wb");
    while (pos < size){
        switch (buffer[pos]) {
            case '\n':{
                fprintf(out_file, "%f\n", stack.stack[0]);
                stack.ptr = -1;
            } break;
            case '+':{
                stack.ptr --;
                stack.stack[stack.ptr] = stack.stack[stack.ptr] + stack.stack[stack.ptr + 1];
            } break;
            case '-':{
                stack.ptr --;
                stack.stack[stack.ptr] = stack.stack[stack.ptr] - stack.stack[stack.ptr + 1];
            } break;
            case '*':{
                stack.ptr --;
                stack.stack[stack.ptr] = stack.stack[stack.ptr] * stack.stack[stack.ptr + 1];
            } break;
            case '/':{
                stack.ptr --;
                stack.stack[stack.ptr] = stack.stack[stack.ptr] / stack.stack[stack.ptr + 1];
            } break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':{
                unsigned int a = 0;
                for (; buffer[pos] >= '0' && buffer[pos] <= '9'; pos ++)
                    a = a * 10 + buffer[pos] - '0';
                push(&stack, a);
            } break;
            default: break;
        }
        pos ++;
    }
    fclose(out_file);
}
