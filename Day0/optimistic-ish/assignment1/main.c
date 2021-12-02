#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int width,
height,
channel = 3;

unsigned char* img;
typedef struct color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} pixel;
typedef struct circle_data
{
    int centre_x;
    int centre_y;
    int radius;
    pixel color;
} c_data;

typedef struct rectangle_data
{
    int centre_x;
    int centre_y;
    int width;
    int height;
    pixel color;
} r_data;

typedef struct position
{
    int x;
    int y;
} position;

void putpixel(position pos, pixel c)
{
    if (pos.x >= width || pos.y >= height || pos.x < 0 || pos.y < 0)
        return;
    int index = (pos.y * width) * channel + pos.x * channel;
    img[index] = c.r;
    img[index + 1] = c.g;
    img[index + 2] = c.b;
}

void circle(int x, int y, int radius, pixel color)
{
    position p;
    while (radius)
    {
        for (float i = 0; i < 2 * 3.15; i += 0.0001)
        {
            p.x = (int)(radius * cos(i)) + x;
            p.y = (int)(radius * sin(i)) + y;
            putpixel(p, color);
        }
        radius--;
    }
}
void rectangle(int x_coord, int y_coord, int wide, int tall, pixel color)
{
    position p;
    for (float j = -tall / 2; j < tall / 2; j++)
    {
        for (float i = 0; i < wide; i += 0.01)
        {
            p.x = x_coord - wide / 2 + i;
            p.y = y_coord + j;
            putpixel(p, color);
        }
    }
}
void initialize()
{
    img = malloc(height * width * channel);
    for (int i = 0; i < width * height * channel; i++)
        img[i] = 0;
}

void read_file()
{
    int golo[6], charkune[7];
    FILE* ptr = fopen("render.txt", "rb");
    fseek(ptr, 0, SEEK_END);
    long buffer_size = ftell(ptr);
    fseek(ptr, 0, SEEK_SET);

    unsigned char* buffer = malloc(buffer_size + 1);

    fread(buffer, 1, buffer_size, ptr);
    buffer[buffer_size] = '\0';
    {int temp = 0, index = 0, inner = 0, size[2];
    while (1)
    {
        if (buffer[index] == ' ' || buffer[index] == '\n')
        {
            size[inner] = temp;
            
            temp = 0;
            if (buffer[index] == '\n')
                break;
            index++; inner++;
            continue;
        }
        temp = temp * 10 + (buffer[index] - '0');
        index++;
    }
    width = size[0];
    height = size[1];
    }
    initialize();
    for (int i = 0; i < buffer_size; i++)
    {
        if (buffer[i] == 'c')
        {
            int k = 0, j = 2;
            int sum = 0;
            while (buffer[i + j] != 'c' && buffer[i + j] != 'r')
            {
                if (buffer[i + j] == ' ' || buffer[i + j] == '\n' || buffer[i + j] == '\0')
                {

                    golo[k] = sum;
                    sum = 0;
                    if (buffer[i + j] == '\0')
                        break;
                    k++;
                    j++;
                    continue;
                }
                sum = sum * 10 + (buffer[i + j] - '0');
                j++;
            }
            j = 2;
            k = 0;
            pixel color = { golo[3], golo[4], golo[5] };
            circle(golo[0], golo[1], golo[2], color);
        }
        if (buffer[i] == 'r')
        {
            int k = 0, j = 2;
            int sum = 0;
            while (buffer[i + j] != 'c' && buffer[i + j] != 'r')
            {
                if (buffer[i + j] == ' ' || buffer[i + j] == '\0' || buffer[i + j] == '\n')
                {

                    charkune[k] = sum;
                    sum = 0;
                    if (buffer[i + j] == '\0')
                        break;
                    k++;
                    j++;
                    continue;
                }
                sum = sum * 10 + (buffer[i + j] - '0');
                j++;
            }
            j = 2;
            k = 0;
            pixel color = { charkune[4], charkune[5], charkune[6] };
            rectangle(charkune[0], charkune[1], charkune[2], charkune[3], color);
        }
    }
}

int main()
{
    read_file();
    FILE* ptr = fopen("ouput.ppm", "w+");
    fprintf(ptr, "P3\n%d %d\n255\n", width, height);
    for (int i = 0; i < width * height * channel; i++)
        fprintf(ptr, "%d ", img[i]);
    return 0;
}