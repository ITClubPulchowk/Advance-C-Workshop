#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define expressionfile "expressions.txt"
#define resultfile "result.txt"

// explicit function declaration

void improve_expression(char * line_of_expression);

int getnum(char * myexp);
char getoperator(char * myexp);
char * get_either_num_or_operator(char * myexp);

int how_many_operators(char * myexp);
int how_many_numbers(char * myexp);
int how_many_numberchars(char * myexp);
float calculate(char * myexp);


char * improveexpression(char * myexp)
{
	int o = how_many_operators(myexp);
	int n = how_many_numbers(myexp);
	int m = how_many_numberchars(myexp);

	char * c = (char *) malloc(0);
	if(m + 1 == n)
	{
		c = (char *) realloc(c, sizeof(char) * (m+n+o));

		int i=0;
		int entities = o + n;




		
	}
	else
	{
		printf("some lines couldn't be executed");
	}

	return c;

}

int how_many_numberchars(char * myexp)
{
	int i = 0;
	for(char* c = myexp; *c != '\0'; c++) // coz, c++ language is better
	{
		if(c >= '0' && c <= '9')
		{
			++i;
		}
	}
	return i;
}

int how_many_operators(char * myexp)
{
	int i = 0;
	for(char* c = myexp; *c != '\0'; c++) // coz, c++ language is better
	{
		switch(c)
		{
			case '+':
			case '-':
			case '*':
			case '/':
				++i;
				break;
			
			default:
				break;
		}
	}
	return i;
}

int how_many_numbers(char * myexp)
{
	int i = 0;
	char was_previus_char_a_digit = 0;
	for(char* c = myexp; *c != '\0'; c++) // coz, c++ language is better
	{
		// if previous char is not a number, as soon as new number is encountered, the number of numbers is increased
		if(!was_previus_char_a_digit)
		{	if(c >= '0' && c <= '9')
			{
				++i;
				was_previus_char_a_digit = 1; // true
			}
		}// if previous char is already number, but new character is not a number, next time, the previous char would not be a digit
		else if(was_previus_char_a_digit)
		{
			if(c < '0' || c > '9')
			{
				was_previus_char_a_digit = 0;
			}
		}
	}
	return i;
}

int get_either_num_or_operator(char * myexp, char * outstring)
{
	char *c = myexp;
	char mycase = 0;
    while(1)
	{
		if(('0'<= *c && *c <= '9'))
		{
			mycase = 1;
		}
		if((*c=='+' || *c=='-' || *c=='*' || *c=='/') )
		{
			mycase = 2;
		}
		if(c == '\0')
		{
			mycase = 3;
		}

		if(mycase) // if any of prevchain cases are satisfied inside this while loop, break
			break;

		c++;
	}

	if(mycase == 1)
	{
		int i = 0;
		for(char * ch = c; ('0' <= *ch && *ch <='9') ; ch++)
		{
			outstring[i] = *ch;
			i++;
		}
		outstring[i] = '\0';
		return i;
	}


}

int getnum(char * myfile)
{
    char numberstring[21] = "                   ";
    
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
            fseek(myfile, -1, 1);
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

float calculate(char * line_of_expression)
{
	float a;

	return a;
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
