#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "mergelogic.c"
#include "vectorlogic.c"



int get_length_of_line(FILE * ifile)
{
    char * noread;                      // for not reading anything
    int startofline = ftell(ifile);     // for storing the initial position
    fscanf(ifile,"%*[^\n]s",noread);    // advancing the file pointer by one line
    int endofline = ftell(ifile);       // for storing the final position
    fseek(ifile,startofline,SEEK_SET);  // to restore the initial position
    while(isspace(fgetc(ifile)))
    ;
    fseek(ifile,-1,SEEK_CUR);
    return endofline - startofline;     // to return the length of string
}

char * get_string_of_line_of_file(FILE * ifile)
{
    // getting the length of a line in the file
    int len = get_length_of_line(ifile)+1;

    // allocating space for it and reading it
    char * numstring = malloc(len * sizeof(char));
    
    // while(isspace(fgetc(ifile)))
    // ;
    // fseek(ifile,-1,SEEK_CUR);
    fgets(numstring, len, ifile);
    numstring[len-1]=0;

    // returning the read string
    return numstring;   
    // !! please free after its use !!
}

struct Vector get_vector_list_from_a_line_string(char * numberlist_string)
{
    char * token;
    char * remaining = numberlist_string;

    struct Vector v = make_vector();
    while((token = __strtok_r(remaining, ",", &remaining)))
    {
        vector_push(v,atoi(token));
    }

    return v;
}

struct Vector get_vector_list_from_a_line_in_file(FILE * ifile)
{
    char * numstring = get_string_of_line_of_file(ifile);
    struct Vector v = get_vector_list_from_a_line_string(numstring);
    free(numstring);
    return v;
}

int test2()
{
    FILE * ofile = fopen(outputfilename, "r");
    struct Vector v = get_vector_list_from_a_line_in_file(ofile);
    char * str = get_string_list(v.v->start,v.v->max_len);
    fflush(stdin);
    printf("%c",*str);
    printf("%.10s",str);
    free(str);
}

int test1()
{
    FILE * ifile = fopen("merge_sort_input.csv","r");
    FILE * ofile = fopen("merge_sort_output.csv","w");

    int c=0;
    for(int i=0; c!=EOF ; i++)
    {
        // read line of the file
        int len = 0;
        char * numstring = get_string_of_line_of_file(ifile);
        printf("\n\nIteration number = %d",i);
        printf("\nNumber_list = \n{\n%s%n\n}\n",numstring,&len);
        printf("Length = %d\n",len);

        // generate the vector list and merge sort it
        struct Vector v = get_vector_list_from_a_line_string(numstring);
        vector_merge_sort(v);

        // display for debugging purposes
        displaylist(numstring,v.v->start, v.v->array_length);
        fprintf(ofile,"%s",numstring);

        c = fgetc(ifile);
        

        printf("'%s'",(c=='\n')?"enter":((c=='\0')?"Null":((c==EOF)?"EOF":(printf("'%d' ASCII = ((%c)) i.e.",c,c),"Other character"))));
        if(c) // if no null character (EOF)
        fprintf(ofile,"\n");

        destroy_vector(v);
        free(numstring);
    }
    
    fclose(ofile);
    fclose(ifile);
    return 0;
}