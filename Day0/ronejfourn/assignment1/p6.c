#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif
#define B_THRESHOLD 8

int main(int argc, char *argv[]){
    FILE *inp_file = fopen("render.txt", "rb");
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

    int pos = 0;

    int img_width = 0, img_height = 0;
    while(isspace(buffer[pos])) pos ++;
    for (; buffer[pos] >= '0' && buffer[pos] <= '9'; pos ++)
        img_width = img_width * 10 + buffer[pos] - '0';
    while(isspace(buffer[pos])) pos ++;
    for (; buffer[pos] >= '0' && buffer[pos] <= '9'; pos ++)
        img_height = img_height * 10 + buffer[pos] - '0';
    int img_area = img_height * img_width;

    unsigned char *colors = malloc(sizeof(*colors) * img_area * 3);

    char type = 1;
    while(pos < size){
        type = buffer[pos];
        if (type == 'c'){
            pos ++;
            int center_x = 0, center_y = 0, radius = 0, r = 0, g = 0, b = 0;
            while(isspace(buffer[pos])) pos ++;
            for (; buffer[pos] >= '0' && buffer[pos] <= '9'; pos ++)
                center_x = center_x * 10 + buffer[pos] - '0';
            while(isspace(buffer[pos])) pos ++;
            for (; buffer[pos] >= '0' && buffer[pos] <= '9'; pos ++)
                center_y = center_y * 10 + buffer[pos] - '0';
            while(isspace(buffer[pos])) pos ++;
            for (; buffer[pos] >= '0' && buffer[pos] <= '9'; pos ++)
                radius = radius * 10 + buffer[pos] - '0';
            while(isspace(buffer[pos])) pos ++;
            for (; buffer[pos] >= '0' && buffer[pos] <= '9'; pos ++)
                r = r * 10 + buffer[pos] - '0';
            while(isspace(buffer[pos])) pos ++;
            for (; buffer[pos] >= '0' && buffer[pos] <= '9'; pos ++)
                g = g * 10 + buffer[pos] - '0';
            while(isspace(buffer[pos])) pos ++;
            for (; buffer[pos] >= '0' && buffer[pos] <= '9'; pos ++)
                b = b * 10 + buffer[pos] - '0';
            int minm = max(center_y - radius + 1, 0);
            int maxm = min(center_y + radius, img_height);
            for(int y = minm; y < maxm; y ++){
                float chordsqr = radius * radius - (center_y - y) * (center_y - y);
                int mn = center_x - radius, mx = center_x;
                int chord;
                char ok = 0;
                for (; mx - mn < B_THRESHOLD;){
                    int mid = (mx + mn) / 2;
                    float rat = (mid - center_x) * (mid - center_x) / chordsqr;
                    if (rat > 1) mx = mid;
                    else if (rat < 1) mn = mid;
                    else {
                        ok = 1;
                        chord = center_x - mid;
                    }
                }
                for (int i = mn; i < mx && !ok; i ++){
                    float rat = (i - center_x) * (i - center_x) / chordsqr;
                    if (rat <= 1) {
                        chord = center_x - i;
                        break;
                    }
                }
                chord *= 2;
                int tmp = max(center_x - chord / 2, 0);
                int pos = y * img_width + tmp;
                if (!tmp) chord = center_x + chord / 2;
                chord = min(chord, img_width - tmp);
                pos *= 3;
                for (int i = 0; i < chord * 3; i += 3){
                    colors[pos + i] = r;
                    colors[pos + i + 1] = g;
                    colors[pos + i + 2] = b;
                }
            }
        } else if (type == 'r') {
            pos ++;
            int center_x = 0, center_y = 0, width = 0, height = 0, r = 0, g = 0, b = 0;
            while(isspace(buffer[pos])) pos ++;
            for (; buffer[pos] >= '0' && buffer[pos] <= '9'; pos ++)
                center_x = center_x * 10 + buffer[pos] - '0';
            while(isspace(buffer[pos])) pos ++;
            for (; buffer[pos] >= '0' && buffer[pos] <= '9'; pos ++)
                center_y = center_y * 10 + buffer[pos] - '0';
            while(isspace(buffer[pos])) pos ++;
            for (; buffer[pos] >= '0' && buffer[pos] <= '9'; pos ++)
                width = width * 10 + buffer[pos] - '0';
            while(isspace(buffer[pos])) pos ++;
            for (; buffer[pos] >= '0' && buffer[pos] <= '9'; pos ++)
                height = height * 10 + buffer[pos] - '0';
            while(isspace(buffer[pos])) pos ++;
            for (; buffer[pos] >= '0' && buffer[pos] <= '9'; pos ++)
                r = r * 10 + buffer[pos] - '0';
            while(isspace(buffer[pos])) pos ++;
            for (; buffer[pos] >= '0' && buffer[pos] <= '9'; pos ++)
                g = g * 10 + buffer[pos] - '0';
            while(isspace(buffer[pos])) pos ++;
            for (; buffer[pos] >= '0' && buffer[pos] <= '9'; pos ++)
                b = b * 10 + buffer[pos] - '0';
            int maxm = min(center_y + height, img_height);
            for(int y = center_y; y < maxm; y ++){
                int pos = y * img_width + center_x;
                int len = min(width, img_width - center_x - 1);
                pos *= 3;
                for (int i = 0; i < len * 3; i += 3){
                    colors[pos + i] = r;
                    colors[pos + i + 1] = g;
                    colors[pos + i + 2] = b;
                }
            }
        }
        pos ++;
    }

    FILE *out_file = fopen("output.ppm", "wb");
    fprintf(out_file, "P6\n%d %d\n255\n", img_width, img_height);
    fwrite(colors, sizeof(*colors), img_area * 3, out_file);
    fclose(out_file);
    return 0;
}
