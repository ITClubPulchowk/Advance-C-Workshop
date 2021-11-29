#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
	float* data;
	unsigned int capacity;
	int top;
} Stack;

void push(Stack* stack, float n)
{
	if(stack->capacity <= stack->top + 1) {
		stack->capacity = stack->capacity * 2;
		stack->data = realloc(stack->data, stack->capacity * sizeof(float));
	}
	stack->data[++(stack->top)] = n;
}

float pop(Stack* stack)
{
	assert((stack->top) >= 0);
	return stack->data[(stack->top)--];;
}

int is_digit(char a) 
{
	return a >= '0' && a <= '9';
}

//caller's responsibility to free after using
char* read_file(const char* path) {
	FILE* file = fopen(path, "rb");
	char* result = 0;
	if(file) {
		unsigned int size = 0;
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		result = malloc(size + 1);
		fseek(file, 0, SEEK_SET);
		if(result) {
			fread(result, size, 1, file);
			result[size] = 0;
		}
		fclose(file);
	}
	return result;
}


int main()
{
	Stack stack = {0};
	stack.capacity = 8;
	stack.data = malloc(stack.capacity * sizeof(*stack.data));
	stack.top = -1;
	
	FILE* result_file = fopen("results.txt", "wb");
	
	char* content = read_file("expressions.txt");
	if(!content) return -1;
	char* row_start = content;
	unsigned int offset = 0;
	
	while(*row_start) {
		char* row_ptr = row_start;
		while(*row_ptr != '\n' && *row_ptr != 0) row_ptr++;
		if(*row_ptr != 0) *row_ptr++ = 0;
		offset = row_ptr - row_start;
		
		row_ptr = row_start;
		stack.top = -1;
		while(*row_ptr != 0) {
			if(is_digit(*row_ptr)) {
				float num = *row_ptr - '0';
				while(is_digit(*(row_ptr+ 1))) 
					num = num * 10 + (*(++row_ptr) - '0');
				if(*(row_ptr + 1) == '.') {
					int divisor = 10;
					row_ptr++;
					while(is_digit(*(row_ptr + 1))) {
						num += (float)(*(++row_ptr) - '0') / divisor;
						divisor *= 10;
					}
				}
				push(&stack, num);
			} else {
				switch(*row_ptr) {
					case '+': {
						float n1 = pop(&stack), n2 = pop(&stack);
						push(&stack, n1 + n2);
					} break;
					
					case '-': {
						float n1 = pop(&stack), n2 = pop(&stack);
						push(&stack, n2 - n1);
					} break;
					
					case '*': {
						float n1 = pop(&stack), n2 = pop(&stack);
						push(&stack, n1 * n2);
					} break;
					
					case '/': {
						float n1 = pop(&stack), n2 = pop(&stack);
						push(&stack, n2 / n1);
					} break;
					
					case ' ': break;
					case '\n': *row_ptr = 0;
					default: break;
				}
			}
			if(*row_ptr) row_ptr++;
		}
		fprintf(result_file, "%f\n", pop(&stack));
		row_start += offset;
	}
	
	fclose(result_file);
	
	free(stack.data);
	free(content);
	return 0;
}