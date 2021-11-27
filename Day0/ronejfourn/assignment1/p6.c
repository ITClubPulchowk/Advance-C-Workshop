#include <stdio.h>
#include <math.h>
#include <stdlib.h>

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

    int center_x, center_y, radius, width, height;
    int r, g, b;

    int img_width, img_height;
    fscanf(inp_file, "%d %d", &img_width, &img_height);
    int img_area = img_height * img_width;

    unsigned char *colors = malloc(sizeof(*colors) * img_area * 3);

    while (!feof(inp_file)){
        char type = fgetc(inp_file);
        if (type == 'c'){
            fscanf(inp_file, "%d %d %d %d %d %d", &center_x, &center_y, &radius, &r, &g, &b);
            int minm = max(center_y - radius, 0);
            int maxm = min(center_y + radius, img_height);
            for(int y = minm; y < maxm; y ++){
                int chord = 2 * (int)sqrt(radius * radius - (center_y - y) * (center_y - y));
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
            fscanf(inp_file, "%d %d %d %d %d %d %d", &center_x, &center_y, &width, &height, &r, &g, &b);
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
    }
    fclose(inp_file);

    FILE *out_file = fopen("output.ppm", "wb");
    fprintf(out_file, "P6\n%d %d\n255\n", img_width, img_height);
    fwrite(colors, sizeof(*colors), img_area * 3, out_file);
    fclose(out_file);
    return 0;
}
