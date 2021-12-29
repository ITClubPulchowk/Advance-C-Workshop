#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define input_file "poly.txt"
#define out_file "results.txt"
typedef struct poly
{
    char variable;
    int power;
    int coef;
    int newline;
} poly;

void sort(poly *first, int start_index, int end_index)
{
    poly tempo = {0};
    for (int l = start_index; l < end_index; l++)
    {
        for (int j = l + 1; j < end_index; j++)
        {
            if (first[j].power < first[l].power)
            {
                tempo = first[j];
                first[j] = first[l];
                first[l] = tempo;
            }
        }
    }
    for (int j = start_index; j < end_index; j++)
    {
        if (first[j].newline == 1)
            first[j].newline = 0;
    }
    first[end_index - 1].newline = 1;
}

int calculate_derivative_to_str(char *temp, poly *first, int number)
{
    int i = 0;
    int start_index;
    int end_index = 0;
    for (i = 0; i <= number; i++)
    {
        first[i].coef = first[i].coef * first[i].power;
        first[i].power--;
    }
    i = 0;
    while (end_index <= number)
    {
        start_index = end_index;
        end_index++;
        for (end_index; first[end_index].newline != 1; end_index++)
            ;
        end_index++;
        sort(first, start_index, end_index);
    }
    i = 0;
    if (first[0].power == 0)
        first[0].variable = ' ';

    if (first[0].power == 1)
    {
        if (first[0].coef)
            i += sprintf(temp + i, "%d%c", first[0].coef, first[0].variable);
    }
    else
    {
        if (first[0].coef)
            i += sprintf(temp + i, "%d%c^%d", first[0].coef, first[0].variable, first[0].power);
    }
    if (first[0].newline)
        i += sprintf(temp + i, "\n");

    for (int k = 1; k <= number; k++)
    {
        if (first[k].coef == 0)
        {
        }
        else if (first[k].power == 0)
        {
            if (first[k - 1].newline || first[k - 1].coef == 0)
                i += sprintf(temp + i, "%d", first[k].coef);
            else
                i += sprintf(temp + i, "+%d", first[k].coef);
        }
        else if (first[k].power == 1)
        {
            if (first[k - 1].newline || first[k - 1].coef == 0)
                i += sprintf(temp + i, "%d%c", first[k].coef, first[k].variable);
            else
                i += sprintf(temp + i, "+%d%c", first[k].coef, first[k].variable);
        }
        else
        {
            if (first[k - 1].newline || first[k - 1].coef == 0)
                i += sprintf(temp + i, "%d%c^%d", first[k].coef, first[k].variable, first[k].power);
            else
                i += sprintf(temp + i, "+%d%c^%d", first[k].coef, first[k].variable, first[k].power);
        }

        if (first[k].newline)
        {
            i += sprintf(temp + i, "\n");
        }
    }
    
    return i;
}
int main()
{
    FILE *fp;
    char temp[1000];
    char *str = NULL;
    char var;
    poly *terms = NULL;
    int file_size = 0, i = 0, number_of_terms = 0;
    int p = 0, k = 0, term_count = 0;
    fp = fopen(input_file, "rb");
    if (!fp)
    {
        printf("No file");
        return 1;
    }
    fseek(fp, 0L, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    str = malloc(sizeof(*str) * (2 * file_size + 1));
    fread(str, file_size, 1, fp);
    fclose(fp);
    str[file_size] = '\0';

    while (str[i] != '\0')
    {
        if (str[i] > 'a' && str[i] < 'z' && str[i] != '\0')
            var = str[i];
        if (str[i] == '+' || str[i] == '-' || str[i] == '\n' || str[i] == '\0')
        {
            number_of_terms++;
        }
        i++;
    }
    number_of_terms++;
    i = 0;
    terms = calloc(number_of_terms + 1, sizeof(*terms));
    while (str[i])
    {
        if (str[i] == var)
        {
            terms[term_count].variable = var;
            p = i;
            k = 0;
            while (str[p] != ' ' && p > 0 && str[p] != '\n' && str[p] != '\r' && str[p] != '\0')
                p--;
            while (p < i)
            {
                temp[k] = str[p];
                p++;
                k++;
            }
            temp[k] = '\0';
            terms[term_count].coef = atoi(temp);
            for (p = i + 2; str[p] != ' ' && str[p] != var && str[p] != '\r' && str[p] != '\n' && str[p] != '\0'; p++)
                ;

            for (int j = i + 2; j <= p; j++)
                temp[j - i - 2] = str[j];

            temp[p - i - 2] = '\0';
            if (temp[0] && temp[0] != var && temp[0] != '+' && temp[0] != '-' && temp[0] != '\0')
                terms[term_count].power = atoi(temp);
            else
                terms[term_count].power = 1;
        }
        if (str[i] == '\n' || str[i] == '\0')
        {
            terms[term_count].newline = 1;
            term_count++;
        }
        else if (str[i] == var)
        {
            terms[term_count].newline = 0;
            term_count++;
        }
        k = 0;
        i++;
    }
    terms[term_count - 1].newline = 1;
    file_size = calculate_derivative_to_str(str, terms, number_of_terms);
    free(terms);
    FILE *fp1 = fopen(out_file, "wb");
    fwrite(str, sizeof(char), strlen(str) , fp1);
    return 0;
}