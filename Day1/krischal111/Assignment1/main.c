#define inputfilename  "mergesort_input.csv"
#define outputfilename  "mergesort_output.csv"
#include "filereadlogic.c"

int main()
{
    FILE * ifile = fopen(inputfilename,"rb"),
        * ofile = fopen(outputfilename,"wb");
    
    int c = 0;
    for(int i=0; c!=EOF ; i++)
    {
        char * numstring = get_string_of_line_of_file(ifile);   // numstring for getting input (string of numbers)
        struct Vector v = get_vector_list_from_a_line_string(numstring);
        vector_merge_sort(v);
        displaylist(numstring,v.v->start, v.v->array_length); // reusing numstring for output
        fprintf(ofile,"%s",numstring);

        c = fgetc(ifile);
        if(c)
        {
            fputc('\0',ifile);
        }
        destroy_vector(v);
        free(numstring);
    }
    fclose(ifile);
    fclose(ofile);
}
