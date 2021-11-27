#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char *argv[]){
    FILE *inp_file = fopen("expressions.txt", "rb");
    if (!inp_file){
        printf("What file?\n");
        return -2;
    }
    fseek(inp_file, SEEK_SET, SEEK_END);
    int size = ftell(inp_file);
    rewind(inp_file);

    char *buffer = malloc(size);
    fread(buffer, 1, size, inp_file);
    fclose(inp_file);

    FILE *out_file = fopen("results.txt", "wb");

    int start = 0, pos = 0;
    float total = 0;

    while (start < size){
        int ptr = start;
        while (buffer[ptr] && buffer[ptr] != '+' && buffer[ptr] != '-' && buffer[ptr] != '*' && buffer[ptr] != '/'){
            if (buffer[ptr] == '\n'){
                start = ptr;
                fprintf(out_file, "%f\n", total);
                total = 0;
            }
            ptr ++;
        }
        if (!buffer[ptr]) break;
        char op = buffer[ptr];
        while(start < ptr){
            float a = 0;
            int str = start;
            int end = start;
            for (; buffer[end] >= '0' && buffer[end] <= '9'; end ++);
            start = end;
            for (; str < end; str ++)
                a = a * 10 + buffer[str] - '0';
            if (a){
                switch(op){
                    case '+':
                        total += a;
                        break;
                    case '-':
                        total -= a;
                        break;
                    case '*':
                        total *= a;
                        break;
                    case '/':
                        total /= a;
                        break;
                    default: break;

                }
            }
            while(isspace(buffer[start])) start ++;
        }
        start = ptr + 1;
    }

    fclose(out_file);
}
