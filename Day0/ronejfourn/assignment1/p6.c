#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

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

    uint8_t *colors = malloc(sizeof(*colors) * img_area * 3);

    char type = 1;
    while(pos < size){
        type = buffer[pos];
        if (type == 'c'){
            pos ++;
            int center_x = 0, center_y = 0, radius = 0;
            uint8_t r = 0, g = 0, b = 0;
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
            int minm = max(center_y - radius, 0);
            int maxm = min(center_y + radius + 1, img_height);
            for(int y = minm; y < maxm; y ++){
                int chordsqr = radius * radius - (center_y - y) * (center_y - y);
                int mn = center_x - radius, mx = center_x;
                int chord;
                for (; (mx - mn) > 1;){
                    int mid = (mx + mn) / 2;
                    int rat = (mid - center_x) * (mid - center_x);
                    if (rat > chordsqr) mn = mid;
                    else                mx = mid;
                }
                if ((mn - center_x) * (mn - center_x) <= chordsqr)
                    chord = center_x - mn;
                else
                    chord = center_x - mx;
                chord *= 2;
                int tmp = max(center_x - chord / 2, 0);
                int pos = y * img_width + tmp;
                if (!tmp) chord = center_x + chord / 2;
                chord = min(chord + 1, img_width - tmp);
                pos *= 3;
                for (int i = 0; i < chord * 3; i += 3){
                    colors[pos + i] = r;
                    colors[pos + i + 1] = g;
                    colors[pos + i + 2] = b;
                }
            }
        } else if (type == 'r') {
            pos ++;
            int center_x = 0, center_y = 0, width = 0, height = 0;
            uint8_t r = 0, g = 0, b = 0;
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
            int minm = max(center_y - height / 2, 0);
            int maxm = min(center_y + height / 2 + 1, img_height);
            for(int y = minm; y < maxm; y ++){
                int pos = y * img_width + center_x - width / 2;
                int len = min(width + 1, img_width - center_x - 1);
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
    fprintf(out_file, "P6\n%d %d 255\n", img_width, img_height);
    fwrite(colors, sizeof(*colors), img_area * 3, out_file);
    fclose(out_file);
    return 0;
}
