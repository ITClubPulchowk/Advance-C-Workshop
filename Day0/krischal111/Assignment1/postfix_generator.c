// updated some things

#include <stdio.h>
#include <stdlib.h>

char generate_operator()
{
	switch(rand() % 4) {
		case 0: return '+';
		case 1: return '-';
		case 2: return '*';
		case 3: return '/';
		default: return '?'; //should never hit this case, but meh the compiler flagged it as a warning and I don't want to set the flag
	} 
}

int main(int argc, char** argv)
{
	if(argc != 2 && argc != 3 && argc != 4) {
		printf("%d\n", argc);
		printf("Usage: %s seed [number of expressions] [expression length]\n", argv[0]);
	} else {
		srand(atoi(argv[1]));
		int index_limit = 10;
		int exp_length = 40;
		if(argc >= 3) index_limit = atoi(argv[2]);
		if(argc >= 4) exp_length = atoi(argv[3]);
		
		printf("Generating %d expressions\n", index_limit);
		FILE* file = fopen("expressions.txt", "wb");
		if(file) {
			for(int i = 0; i < index_limit; ++i) {
				
				fprintf(file, "%d %d", rand() % 20 + 1, rand() % 20 + 1);
				
				for(int j = 1; j < exp_length; ++j) {
					fprintf(file, " %c %d", generate_operator(), rand() % 20 + 1);
				}
				fprintf(file, "%c\n", generate_operator());
			}
			fclose(file);
			
                        FILE* file = fopen("expressions.txt", "r");
                        system("clear");
                        char c = fgetc(file);
                        while(c != EOF)
                        {
                            printf("%c",c);
                            c = fgetc(file);
                        }
                        fclose(file);
		}
	}
}
