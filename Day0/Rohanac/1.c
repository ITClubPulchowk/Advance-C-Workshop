
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

    uint32_t *pixel_data;
    FILE *fp;
    int center_x;
    int center_y;
    int width;
    int height;
    int radius;
    int r, g, b;
    char *file_data;
    long file_size;
    int img_height, img_width;
    bool shape_change = true;
    fp = fopen("render.txt", "rb");
    fseek(fp, 0L, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    file_data = malloc(sizeof(*file_data) * (file_size + 1));
    fread(file_data, file_size, 1, fp);
    fclose(fp);
    file_data[file_size + 1] = '\0';
    sscanf(file_data, "%d %d", &img_width, &img_height);
    while ((*file_data) != '\n')
    {
        file_data++;
    }
    pixel_data = calloc(img_height * img_width, sizeof(uint32_t));
    while (*(file_data++))
    {
        if (shape_change)
        {
            switch (*file_data)
            {
            case 'r':
                shape_change = false;
                sscanf(file_data, "r %d %d %d %d %d %d %d\n", &center_x, &center_y, &width, &height, &r, &g, &b);
                for (int i = center_x - width/2; i < center_x + width/2; i++)
                {
                    for (int j = center_y - height/2; j < center_y + height/2; j++)
                    {
                        if (i < 0 || j < 0)
                            continue;
                        pixel_data[i * img_width + j] = (r << 24 | g << 16 | b << 8 | 0xff);
                    }
                }
                break;

            case 'c':
                /* circle */
                shape_change = false;
                sscanf(file_data, "c %d %d %d %d %d %d\n", &center_x, &center_y, &radius, &r, &g, &b);
                for (int i = 0; i < img_width; i++)
                {
                    for (int j = 0; j < img_height; j++)
                    {
                        if ((i - center_x) * (i - center_x) + (j - center_y) * (j - center_y) <= radius * radius)
                        {
                            pixel_data[j * img_width + i] = (r << 24 | g << 16 | b << 8 | 0xff);
                        }
                    }
                }
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

    fp = fopen("image.ppm", "wb");
    fprintf(fp, "P6 %d\n%d\n%d\n", img_width, img_height, maxvalue);
    for (int i = 0; i < img_width; i++)
    {
        for (int j = 0; j < img_height; j++)
        {
            unsigned char ar=(pixel_data[j*img_width+i]>>(8*3)) & 0xff;
            unsigned char ag=(pixel_data[j*img_width+i]>>(8*2)) & 0xff;
            unsigned char ab=(pixel_data[j*img_width+i]>>(8*1)) & 0xff;
            fprintf(fp,"%c%c%c",ar,ag,ab);

        }
    }
    fclose(fp);
    return 0;
}
