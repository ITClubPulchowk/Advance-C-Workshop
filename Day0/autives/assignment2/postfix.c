#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define STACK_SIZE 256
#define STACK_BASE STACK_SIZE - 2

int* stack;
unsigned int stackPointer;

void InitStack() {
    stack = malloc(STACK_SIZE * sizeof(int));
    stackPointer = STACK_BASE;
}

void Push(int num) {
    if (stackPointer == STACK_BASE + 1) {
        printf("Too many consecutive operands. Maximum supported number of operands is 255.\n");
        exit(2);
    }
    stack[stackPointer] = num;
    stackPointer--;
}

int Pop() {
    stackPointer++;
    return stack[stackPointer];
}

float Evaluate(char* expression, int size)
{
    char* tmp = expression;
    float results;
    char firstOperator = 1;
    int value1, value2;

    while(tmp - expression <= size) {
        if (*tmp == ' ') {
            tmp++;
            continue;
        }
        if (*tmp >= '0' && *tmp <= '9') {
            Push(strtoll(tmp, &tmp, 10));
            continue;
        }
        else if (*tmp == '+') {
            if (firstOperator) {
                value1 = Pop();
                value2 = Pop();
                results = value2 + value1;
                firstOperator--;
            }
            while (stackPointer < STACK_BASE) {
                results += Pop();
            }
        }
        else if (*tmp == '-') {
            if (firstOperator) {
                value1 = Pop();
                value2 = Pop();
                results = value2 - value1;
                firstOperator--;
            }
            else {
                results -= Pop();
            }
        }
        else if (*tmp == '*') {
            if (firstOperator) {
                value1 = Pop();
                value2 = Pop();
                results = value2 * value1;
                firstOperator--;
            }
            while (stackPointer < STACK_BASE) {
                results *= Pop();
            }
        }
        else if (*tmp == '/') {
            if (firstOperator) {
                value1 = Pop();
                value2 = Pop();
                results = value2 / value1;
                firstOperator--;
            }
            else {
                results /= Pop();
            }
        }
        tmp++;
    }

    return results;
}

int FileSize(FILE* ptr) {
    fseek(ptr, 0, SEEK_END);
    int size = ftell(ptr);
    fseek(ptr, 0, SEEK_SET);
    return size;
}

int main() {
    FILE* sourceFile = fopen("expressions.txt", "rb");
    if (!sourceFile)
        exit(1);
    InitStack();
    int fileSize = FileSize(sourceFile);
    char* sourceTxt = malloc((fileSize + 1) * sizeof(char));
    fread(sourceTxt, sizeof(char), fileSize, sourceFile);
    sourceTxt[fileSize] = '\0';
    fclose(sourceFile);

    FILE* destFile = fopen("results.txt", "w");

    while (*sourceTxt) {
        int expSize = 0;
        char init = 0;
        for (; sourceTxt[expSize] != '\n'; expSize++);
        char* expression;
        expression = sourceTxt;
        sourceTxt += expSize + 1;
        fprintf(destFile, "%f\n", Evaluate(expression, expSize));
    }
    fclose(destFile);

    return 0;
}