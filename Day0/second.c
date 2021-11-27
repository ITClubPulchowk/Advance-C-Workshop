#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char * readNum(char *,double *);

int main()
{
    char filename[100]="expressions.txt";
    FILE *fp;
    fp=fopen(filename,"r");
    if(fp==NULL)
    {
        printf("File not found\n");
        return 0;
    }
    char ch;
    double result = 0.0;

    fseek(fp, 0L, SEEK_END);
  
    // calculating the size of the file
    long int fileSize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char* fileBuffer = (char*)malloc(fileSize);
    memset(fileBuffer, 0, fileSize);
    int size=0;
    
    while((ch=fgetc(fp))!=EOF)
    {
        fileBuffer[size]=ch;
        size++;
    }
    fileBuffer[size]='\0';
    fclose(fp);

    //Creating result file
    FILE *fp1;
    fp1=fopen("results.txt","w");
    if(fp1==NULL)
    {
        printf("File not found\n");
        return 0;
    }


    int i = 0;
    char *p = fileBuffer;
    while(*p != '\0')
    {
        p = readNum(p,&result);

        //If there were no numbers in the file left
        if(*p == '\0' && result == 0.0)
        {
            break;
        }
        while(*p != '\0' && *p != '\n')
        {
            double nextNum=0;
            p = readNum(p,&nextNum);
            //Skipping whitespaces
            while(*p == ' ' || *p == '\t')
            {
                p++;
            }
            switch(*p)
            {
                case '+':
                    result += nextNum;
                    break;
                case '-':
                    result -= nextNum;
                    break;
                case '*':
                    result *= nextNum;
                    break;
                case '/':
                    result /= nextNum;
                    break;
                default:
                    break;
            }            
            p++;
        }
        fprintf(fp1,"%lf\n",result);
    }
    free(fileBuffer);
    fclose(fp1);
    return 0;
}

char * readNum(char *p,double *result)
{
    //Skipping the spaces
    while(*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == '\f' || *p == '\v')
    {
        p++;
    }

    *result = 0.0;
    //Checking if the string is a number
    if(*p >= '0' && *p <= '9')
    {
        *result = *result * 10 + (*p - '0');
        p++;
        while(*p >= '0' && *p <= '9')
        {
            *result = *result * 10 + (*p - '0');
            p++;
        }
    }
    return p;
}