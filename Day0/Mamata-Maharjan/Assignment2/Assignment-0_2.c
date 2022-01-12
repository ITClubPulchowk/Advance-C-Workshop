//a code in c for postfix calculation

#include<stdio.h>
#include<conio.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>

int calculations(float* ar, int i, char ch, float* res)
{

    //calculations
    switch (ch)
    {
    case '+':
        *(ar + i - 2) = (*(ar + i - 2)) + (*(ar + i - 1)) ;
        *res = (*(ar + i - 2));
        break;

    case '-':
        *(ar + i - 2) = (*(ar + i - 2)) - (*(ar + i - 1));
        *res = (*(ar + i - 2));
        break;

    case '*':
        *(ar + i - 2) = (*(ar + i - 2)) * (*(ar + i - 1)) ;
        *res = (*(ar + i - 2));
        break;

    case '/':
        *(ar + i - 2) =  (*(ar + i - 2)) / (*(ar + i - 1)) ;
        *res = (*(ar + i - 2));
        break;

    default:
        printf("\n COULDN'T RECOGNIZE THE ARTHIMATIC OPERATOR:\n\n");
        break;
    }

    return(i - 1);
}


void main()
{
    FILE* file1, * file2;
    char ch, temp[100];
    int i = 0, j, k, l;
    float number, multiplier, exp[100], res;


    // reading the given file
    file1 = fopen("expressions.txt", "rb");
    if (file1 == NULL)
    {
        printf("COULDN'T OPEN THE FILE.");
        exit(1);
    }

    fseek(file1, 0, SEEK_END);
    long fsize = ftell(file1);
    fseek(file1, 0, SEEK_SET);

    char* data = (char*)malloc(fsize + 1);
    fread(data, fsize, 1, file1);
    fclose(file1);

    data[fsize] = 0;

    file2 = fopen("results.txt", "wb");//to write the calculated data
    if (file2 == NULL)
    {
        printf("COULD'T OPEN THE FILE.");
        exit(1);
    }


    k = 0; l = 0;
    while (*(data + k) != data[fsize])
    {
        ch = *(data + k);

        if (ch == '\n' || ch == data[fsize])
        {
            printf(" = %f \n", res);
            fprintf(file2, "%f\n", res);
            memset(exp, 0, sizeof exp);
            k++;
            ch = *(data + k);
        }


        else if (ch >= '0' && ch <= '9')//number_check
        {
            multiplier = 1;
            number = 0;

            for (i = 0; (ch >= '0' && ch <= '9'); i++)
            {
                temp[i] = ch;
                k++;
                ch = *(data + k);
            }

            for (j = i - 1; j >= 0; --j)
            {
                number += multiplier * (temp[j] - '0');
                multiplier *= 10;
            }
            exp[l] = number;
            l++;

             printf("%f ", number);

        }


        else if (ch == '+' || ch == '-' || ch == '/' || ch == '*')//operator check
        {
            printf("%c ", ch);
            l = calculations(exp, l, ch, &res);
            k++;
            ch = *(data + k);
        }

        else
        {
            k++;
            ch = *(data + k);
        }

    }
    printf("\nResults are also written in file 'results.txt'\n");

    fclose(file2);
    free(data);

}
