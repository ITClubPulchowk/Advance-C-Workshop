#include<stdio.h>
#include<stdlib.h>

#define expressionfile "expressions.txt"
#define resultfile "result.txt"

int getnum(FILE * myfile)
{
    char numberstring[10] = "         ";
    
    // removing whitespace at the beginning
    char c = ' ';
    while(c == ' ')
        c = fgetc(myfile);
        
    // extracting number as string into a variable
    for(int i = 0; i<10; i++)
    {
        numberstring[i] = c;
        
        // break when numbers are completed
        if(c < '0' || c > '9')
        {
            fseek(myfile, -1, 1); // Ek step paxadi aija vaneko
            break;
        }
        c = fgetc(myfile);
    }
    
    // return the number in our string
    return atoi(numberstring);
}

char getoperator(FILE* myfile)
{
    char c;
    do
    {
        c = fgetc(myfile);
    } while (c == ' ') ;
    
    return c;
}

int main()
{
    FILE * myfile = fopen(expressionfile, "rb");
    if(myfile)
    {
		FILE * resfile = fopen(resultfile, "wb");
		fclose(resfile);
		
		while( 1 )
		{  
			char c = fgetc(myfile);
			fseek(myfile, -1, 1);
			
			if(c == EOF)
				break;
		
			float a = getnum(myfile);
			while(c != '\n' && c != EOF)
			{
				int b = getnum(myfile);
				
				switch(getoperator(myfile))
				{
					case '*':
					a *= b;
					break;
					
					case '/':
					a /= b;
					break;
					
					case '+':
					a += b;
					break;
					
					case '-':
					a -= b;
					break;
					
					default:
					break;
				}
				
			c = fgetc(myfile);
			fseek(myfile, -1, 1);
			}
			
			FILE * resfile = fopen(resultfile, "ab");
			fprintf(resfile, "%f\n",a);
			printf("%f\n",a);
			fclose(resfile);

				if(c == EOF)
				break;

				fgetc(myfile);
		}
    }
    fclose(myfile);
}
