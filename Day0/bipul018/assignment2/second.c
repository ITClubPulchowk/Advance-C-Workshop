#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Returns the position just after the number given
char * readNum(char *,double *);

//Flag = 1 means skip newline characters too
char * skipWhites(char * p,int flag );
int main(int argc, char *argv[])
{

    //Obtaining Source file
    char filename[100]="expressions.txt";

    //If given in command line, uses that file
    if(argc>1)
    {
        strcpy(filename,argv[1]);
    }
    FILE *fp;
    fp=fopen(filename,"r");
    if(fp==NULL)
    {
        printf("File not found\n");
        return 0;
    }

    fseek(fp, 0L, SEEK_END);
  
    // calculating the size of the file
    long int fileSize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    //Allocating memory to store the file
    char* fileBuffer = (char*)malloc(fileSize);
    memset(fileBuffer, 0, fileSize);
    int size=0;
    
    while((fileBuffer[size]=fgetc(fp))!=EOF)
    {
        size++;
    }
    fileBuffer[size]='\0';
    fclose(fp);

    //Creating result file
    FILE *fp1;
    char outputFile[100]="results.txt";

    //If given in command line, uses that file
    if(argc>2)
    {
        strcpy(outputFile,argv[2]);
    }
    fp1=fopen(outputFile,"w");
    if(fp1==NULL)
    {
        printf("File not found\n");
        return 0;
    }


    char *p = fileBuffer;
    double result = 0.0;
    while(*p != '\0')
    {
        //Skip all whitespace including newline
        p = skipWhites(p,1);

        //Skip if ended
        if(*p == '\0')
            break;

        //Read the first number
        p = readNum(p,&result);

        while(*p != '\0' && *p != '\n')
        {
            double nextNum=0;
            p = readNum(p,&nextNum);
            //Skipping whitespaces
            p = skipWhites(p,0);

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

char * skipWhites(char *p,int flag)
{
    while(*p == ' ' || *p == '\t' || *p == '\r' || *p == '\f' || *p == '\v'|| ((*p == '\n' ) && flag == 1))
    {
        p++;
    }

    return p;
}

char * readNum(char *p,double *result)
{
    //Skipping the spaces
    p = skipWhites(p,0);

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