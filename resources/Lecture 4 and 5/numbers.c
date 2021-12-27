#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char** argv)
{
	if(argc != 4) {
		printf("%d\n", argc);
		printf("Usage: %s seed [seed] [count] [max]\n", argv[0]);
        return 1;
	}

    srand(atoi(argv[1]));

    int ascending = rand() & 1;

    const char *filename = "input.txt";
    int count = atoi(argv[2]);
    uint32_t max_number = atoi(argv[2]);

    const int minimum_count = 1000000;

    if (count < minimum_count) {
        printf("Using Minimum count of %d intead of %d\n", minimum_count, count);
        count = minimum_count;
    }

    printf("Generating %d numbers in %s\n", count, filename);

    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "Failed to create file: %s\n", filename);
        return 1;
    }

    fprintf(file, "%d %c\n", count, ascending ? 'a' : 'd');

    for(int i = 0; i < count; ++i) {        
        fprintf(file, "%d ", rand() % max_number);
        if ((i + 1) % 20 == 0) {
            fprintf(file, "\n");
        }
    }

    fclose(file);

    return 0;
}
