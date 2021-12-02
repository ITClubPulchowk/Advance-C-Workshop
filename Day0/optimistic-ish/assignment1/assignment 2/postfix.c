#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

double num[10];
int stack_tracker = 0;
char* digit_extracter(char* p)
{

	if (*p >= '0' && *p <= '9')
	{
		float digit = 0;
		while (1)
		{
			if (!(*p >= '0' && *p <= '9'))
			{
				num[stack_tracker] = digit;
				//printf("%f\n", digit);
				stack_tracker++;
				digit = 0;
				return p;
			}
			digit = digit * 10 + ((*p) - '0');
			p++;
		}
	}
}
char* skip_white_spaces(char* p)
{
	while (1)
	{
		if (*p == ' ')
			p++;
		else
			return p;
	}
}
char* calculator(char* p)
{
	if (*p == '+' || *p == '-' || *p == '*' || *p == '/')
	{
		switch (*p)
		{
		case '+':
		{
			num[stack_tracker - 2] = num[stack_tracker - 2] + num[stack_tracker - 1];
			stack_tracker--;
			//printf("%f\n", num[stack_tracker - 1]);
			break;
		}

		case '-':
		{
			num[stack_tracker - 2] = num[stack_tracker - 2] - num[stack_tracker - 1];
			stack_tracker--;
			//printf("%f\n", num[stack_tracker - 1]);
			break;
		}
		case '*':
		{
			num[stack_tracker - 2] = num[stack_tracker - 2] * num[stack_tracker - 1];
			stack_tracker--;
			//printf("%f\n", num[stack_tracker - 1]);
			break;
		}
		case '/':
		{
			num[stack_tracker - 2] = num[stack_tracker - 2] / num[stack_tracker - 1];
			stack_tracker--;
			//printf("%f\n", num[stack_tracker - 1]);
			break;
		}
		default:
			;
		}
		p++;
		return p;
	}
}
void work(char* buffer)
{
	FILE* ptr = fopen("results.txt", "a+");
	char* pointer = buffer;
	while (1)
	{
		if (*pointer == '+' || *pointer == '-' || *pointer == '*' || *pointer == '/')
		{
			pointer = calculator(pointer);
			continue;
		}
		if (*pointer >= '0' && *pointer <= '9')
		{
			pointer = digit_extracter(pointer);
			continue;
		}
		if (*pointer == ' ')
		{
			pointer = skip_white_spaces(pointer);
			continue;
		}
		if (*pointer == '\n')
		{
			printf("%f\n", num[stack_tracker - 1]);
			fprintf(ptr, "%f\n", num[stack_tracker - 1]);
			pointer++;
			for (int i = 0; i < 10; i++)
				num[i] = 0;
			stack_tracker = 0;

		}
		if (*pointer == '\0')
		{
			printf("%f\n", num[stack_tracker - 1]);
			fprintf(ptr, "%f\n", num[stack_tracker - 1]);
			break;
		}
		pointer++;

	}
}
void read_file()
{
	FILE* ptr = fopen("expressions.txt", "rb");
	fseek(ptr, 0, SEEK_END);
	long buffer_size = ftell(ptr);
	fseek(ptr, 0, SEEK_SET);
	char* buffer = malloc(buffer_size + 1);
	fread(buffer, 1, buffer_size + 1, ptr);
	buffer[buffer_size] = '\0';
	if (buffer == NULL)
		exit(1);
	work(buffer);
}

void main()
{
	read_file();
}
