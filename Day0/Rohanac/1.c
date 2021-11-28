
/*
image_width image_height
c center_x center_y radius r g b
r center_x center_y width height r g b
*/

/* ppm file format
    magic number (p3 or p6) image_width  image_height max_value_of_color_components
    image data
*/
//TODO: SHIFT THE RGB ORDER
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#define maxvalue 255
int main()
{
    struct rect
    {
        int center_x;
        int center_y;
        int width;
        int height;
        int r, g, b;
    } rect1[4];
    struct cir
    {
        int center_x;
        int center_y;
        int radius;
        int r, g, b;
    } circle[4];
    uint32_t **r, **g, **b;
    char image_data[48];
    char *file_data;
    FILE *fp;
    long file_size;
    int height, width, count_rect = -1, count_circ = -1;
    bool shape_change = true, rect = false, circ = false;
    fp = fopen("render.txt", "rb");
    fseek(fp, 0L, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    file_data = malloc(sizeof(*file_data) * (file_size + 1));
    fread(file_data, file_size, 1, fp);
    fclose(fp);
    file_data[file_size + 1] = '\0';
    sscanf(file_data, "%d %d", &width, &height);
    // rect1=malloc(sizeof(*rect1)*4);
    // circle=malloc(sizeof(*circle)*4p );
    while ((*file_data) != '\n')
    {
        file_data++;
    }
    while (*(file_data++))
    {

        if (shape_change)
        {
            switch (*file_data)
            {
            case 'r':
                count_rect++;
                shape_change = false;
                sscanf(file_data, "r %d %d %d %d %d %d %d\n", &rect1[count_rect].center_x, &rect1[count_rect].center_y, &rect1[count_rect].width, &rect1[count_rect].height, &rect1[count_rect].r, &rect1[count_rect].g, &rect1[count_rect].b);
                break;

            case 'c':
                /* circle */
                count_circ++;
                shape_change = false;
                sscanf(file_data, "c %d %d %d %d %d %d\n", &circle[count_circ].center_x, &circle[count_circ].center_y, &circle[count_circ].radius, &circle[count_circ].r, &circle[count_circ].g, &circle[count_circ].b);
                break;
            default:
                break;
            }
        }
        if (*file_data == '\n')
        {
            shape_change = true;
        }
    }

    r = calloc(height, sizeof(uint32_t *));
    g = calloc(height, sizeof(uint32_t *));
    b = calloc(height, sizeof(uint32_t *));
    for (int i = 0; i < width; i++)
    {
        r[i] = calloc(width, sizeof(uint32_t));
        g[i] = calloc(width, sizeof(uint32_t));
        b[i] = calloc(width, sizeof(uint32_t));
    }
    for (int k = 0; k <= count_rect; k++)
    {
        for (int i = rect1[k].center_x - rect1[k].width; i < rect1[k].center_x + rect1[k].width; i++)
        {
            for (int j = rect1[k].center_y - rect1[k].height; j < rect1[k].center_y + rect1[k].height; j++)
            {
                if (i < 0 || j < 0)
                    continue;

                r[j][i] = rect1[k].r;
                g[j][i] = rect1[k].g;
                b[j][i] = rect1[k].b;
            }
        }
    }

    for (int k = 0; k <= count_circ; k++)

    {
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                if ((i - circle[k].center_x) * (i - circle[k].center_x) + (j - circle[k].center_y) * (j - circle[k].center_y) <= circle[k].radius * circle[k].radius)
                {
                    r[j][i] = circle[k].r;
                    g[j][i] = circle[k].g;
                    b[j][i] = circle[k].b;
                }
            }
        }
    }
    fp = fopen("image.ppm", "wb");
    fprintf(fp,"P6 %d\n%d\n%d\n", width, height, maxvalue);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            fwrite(&g[i][j], sizeof(uint32_t), 1, fp);
            fwrite(&b[i][j], sizeof(uint32_t), 1, fp);
            fwrite(&r[i][j], sizeof(uint32_t), 1, fp);
        }
    }
    fclose(fp);
    return 0;
}
