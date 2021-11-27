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
    char newline = 1;
    int pos = 0;
    float a = 0, b = 0;
    while (pos < size){
        switch(buffer[pos]){
            case '+':{
                a += b;
            }break;
            case '-':{
                a -= b;
            }break;
            case '*':{
                a *= b;
            }break;
            case '/':{
                a /= b;
            }break;
            case '\n':{
                fprintf(out_file, "%f\n", a);
                newline = 1;
            }break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if (newline){
                    newline = 0;
                    a = 0;
                    int start = pos;
                    int end = pos;
                    for (; buffer[end] >= '0' && buffer[end] <= '9'; end ++);
                    pos = end;
                    for (; start < end; start ++)
                        a = a * 10 + buffer[start] - '0';
                } else {
                    b = 0;
                    int start = pos;
                    int end = pos;
                    for (; buffer[end] >= '0' && buffer[end] <= '9'; end ++);
                    pos = end;
                    for (; start < end; start ++)
                        b = b * 10 + buffer[start] - '0';
                }
                break;
            default:break;
        }
        pos ++;
    }
    fclose(out_file);
}
