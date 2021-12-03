#include <stdio.h>
#include <stdlib.h>

int main()
{
    FILE *fp;
    char *file_data;
    int file_size = 0, multiplier = 1;
    float a = 0, b = 0, result = 0, c = 0;
    char operator= '0';
    fp = fopen("expressions.txt", "rb");
    if(!fP)
    {printf("no file");
     return 1;
    }
    fseek(fp, 0L, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    file_data = malloc(sizeof(*file_data) * (file_size + 1));
    fread(file_data, file_size, 1, fp);
    fclose(fp);
    file_data[file_size + 1] = '\0';
    fp = fopen("results.txt", "w");
    while (*file_data)
    {
        switch (*file_data)
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
            a *= multiplier;
            a += (float)(*file_data - '0');
            multiplier *= 10;
            break;
        case ' ':
            if (((int)(file_data[1])) > ((int)'0') && (int)(file_data[1]) < ((int)'9'))
            {
                c = a;
                a = 0;
                multiplier = 1;
            }
            break;
        case '+':
            result = c ? (c + a) : (b + a);
            b = result;
            a = result;
            multiplier = 1;
            break;

        case '-':
            result = c ? (c - a) : (b - a);
            b = result;
            a = result;
            multiplier = 1;
            break;

        case '*':
            result = c ? (c * a) : (b * a);
            b = result;
            a = result;
            multiplier = 1;
            break;

        case '/':
            result = c ? (c / a) : (b / a);
            b = result;
            a = result;
            multiplier = 1;
            break;
        case '\n':
            a = 0;
            b = 0;
            multiplier = 1;
            fprintf(fp, "%f\n", result);
            result = 0;
            break;
        default:
            break;
        }

        file_data++;
    }

    return 0;
}
