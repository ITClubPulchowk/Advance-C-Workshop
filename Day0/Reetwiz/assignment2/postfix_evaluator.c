#include <stdio.h>
#include <stdlib.h>

int main()
{
    char *buffer, ch;

    int length, top = -1, i = 0, multiplier;

    float result, *stack, final, num, operand1, operand2;

    FILE *fp = fopen("expressions.txt", "rb");
    if (!fp)
    {
        fprintf(stderr, "cannot open file");
        exit(1);
    }

    FILE *fp2 = fopen("results.txt", "wb");
    if (!fp2)
    {
        fprintf(stderr, "cannot open file");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    rewind(fp);
    buffer = (char *)malloc(length * sizeof(char));

    if (!buffer)
    {
        printf("cannot allocate memory");
        exit(1);
    }

    while (!feof(fp))
        fread(buffer, sizeof(char), length, fp);

    stack = (float *)malloc(length * sizeof(float));
    if (!stack)
    {
        printf("cannot allocate memory");
        exit(1);
    }

    while (i <= length)
    {
        while (buffer[i] != '\n' && i <= length)
        {
            ch = buffer[i];

            if (ch == ' ')
            {
                i++;
                continue;
            }

            else if (ch >= '0' && ch <= '9')
            {
                if (!(buffer[i + 1] >= '0' && buffer[i + 1] <= '9'))
                {
                    top++;
                    *(stack + top) = ch - '0';
                }

                else
                {
                    multiplier = 10;
                    num = buffer[i] - '0';
                    while ((buffer[i + 1] >= '0' && buffer[i + 1] <= '9'))
                    {
                        num = num * multiplier + (buffer[i + 1] - '0');
                        i++;
                    }
                    top++;
                    *(stack + top) = num;
                }
            }

            else if (ch == '+' || ch == '-' || ch == '/' || ch == '*')
            {

                operand2 = *(stack + top);
                top--;

                operand1 = *(stack + top);
                top--;

                switch (ch)
                {
                case '+':
                    result = (operand1 + operand2);
                    break;
                case '-':
                    result = (operand1 - operand2);
                    break;
                case '/':
                    result = (operand1 / operand2);
                    break;
                case '*':
                    result = (operand1 * operand2);
                    break;
                }

                top++;
                *(stack + top) = result;
            }
            i++;
        }
        i++;

        result = *(stack + top);
        top--;

        fprintf(fp2, "%f\n", result);
    }

    fclose(fp);
    fclose(fp2);

    free(buffer);
    free(stack);
}