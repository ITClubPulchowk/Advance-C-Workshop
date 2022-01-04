//binary tree struct
#include <stdio.h>
#include <stdlib.h>
#define input_file "infix_input.txt"
#define output_file "infix_output.txt"
char *get_in_buffer(long *file_size)
{
    char *str = NULL;
    FILE *fp = fopen(input_file, "rb");
    if (!fp)
    {
        printf("No file");
        return NULL;
    }
    fseek(fp, 0L, SEEK_END);
    *file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    str = malloc(sizeof(*str) * (*file_size + 1));
    fread(str, *file_size, 1, fp);
    fclose(fp);
    str[*file_size] = '\0';
    return str;
}
float create_BST(char **str)
{
    float operand[2] = {0};
    char prev_op = 0;
    float calc_val = 0;
    char temp[10];
    int operand_count = 0;
    float value = 0;
    int k = 0;
    int i = 0;
    for (i; (*str)[i] != '\n' && (*str)[i] != '\0'; i++)
    {
        if (i == 0)
            prev_op = 1;

        switch ((*str)[i])
        {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
            temp[k] = (*str)[i];
            k++;
            if ((*str)[i + 1] < '0' || (*str)[i + 1] > '9')
            {
                temp[k] = '\0';
                k = 0;
                operand[operand_count] = atof(temp);
                if (prev_op != 0)
                {
                    switch (prev_op)
                    {
                    case '*':
                        value = operand[0] * operand[1];
                        calc_val += value;
                        operand_count = 0;
                        prev_op = 0;
                        break;
                    case '/':
                        value = operand[0] / operand[1];
                        calc_val += value;

                        prev_op = 0;
                        operand_count = 0;
                        break;
                    case '-':
                        operand_count = 0;
                    default:
                        break;
                    }
                }
                if ((*str)[i + 1] == '\n' || (*str)[i + 1] == '\r')
                {
                    if (prev_op == 1)
                    {
                        calc_val += operand[0];
                    }
                    operand_count = 0;
                }
            }
            break;
        case '*':
        case '/':
            if (prev_op == '-')
            {
                operand[0] = -operand[0];
            }
            prev_op = (*str)[i];
            operand_count++;
            break;
        case '-':
            calc_val += operand[0];

            prev_op = (*str)[i];
            break;
        case '+':
            if (prev_op == 1)
            {
                calc_val += operand[0];
            }
            prev_op = 1;
            operand_count = 0;
            break;
        case '(':
            for (k = 0; (*str)[k + i] != ')' && (*str)[k + i] != '\n'; k++)
                ;
            char *arr = malloc((k + 1));
            for (int j = 0; j < k; j++)
            {
                arr[j] = (*str)[i + j + 1];
            }
            arr[k - 1] = '\0';
            value = create_BST(&arr);
            if (!prev_op || prev_op == 1)
            {
                operand[operand_count] = value;
            }
            else
            {
                switch (prev_op)
                {
                case '*':
                    value = operand[0] * value;
                    calc_val += value;
                    operand_count = 0;
                    prev_op = 0;
                    break;
                case '/':
                    value = operand[0] / value;
                    calc_val += value;

                    prev_op = 0;
                    operand_count = 0;
                    break;
                case '-':
                    operand_count = 0;
                    break;

                default:
                    break;
                }
            }
            i += k - 1;
            k = 0;
            break;
        default:
            break;
        }
    }
    (*str) += i+1;
    return calc_val;
}

void main()
{
    long file_size = 0;
    char *str = get_in_buffer(&file_size);
    if (str == NULL)
        return;
    // needed_trees(str);
    int j = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '\n')
            j++;
    }
    FILE *fp = fopen(output_file, "w");
    for (j; j > 0; j--)
    {
        fprintf(fp, "%f\n", create_BST(&str));
    }
}