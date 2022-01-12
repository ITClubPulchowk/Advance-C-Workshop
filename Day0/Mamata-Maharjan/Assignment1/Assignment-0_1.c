// A code in C to draw different shapes in a ppm file

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

typedef unsigned char uchar;
typedef unsigned int uint;

void paint(unsigned char* canvas, int x, int y, uchar r, uchar g, uchar b, int image_width, int image_height)
{
    if (x >= 0 && x < image_width && y >= 0 && y < image_height) 
    {
        unsigned char* pixel = canvas + y * image_width * 3 + x * 3;                //filling the desired colour in the selected spaces of the canvas
        pixel[0] = r;
        pixel[1] = g;
        pixel[2] = b;
    }
}

void draw_rect(unsigned char* canvas, int si, int sj, int w, int h, uchar r, uchar g, uchar b, int image_width, int image_height)
{
    for (int i = (sj-h/2); i <= (sj + h/2); i++)                                     //detecting the position to draw a rectangle in the canvas
    {
        for (int j = (si - w/2); j <= (si + w/2); j++)
        {
            paint(canvas, i, j, r, g, b, image_width, image_height);
        }
    }
}

void draw_circle(unsigned char* canvas, int si, int sj, int rs, uchar r, uchar g, uchar b, int image_width, int image_height)
{
    for (int y = sj - rs; y <= sj + rs; ++y)                           //detecting the position to draw a circle in the canvas
    {
        for (int x = si - rs; x <= si + rs; ++x)
        {
            if ((x - si) * (x - si) + (y - sj) * (y - sj) <= rs * rs)
            {
                    paint(canvas, x, y, r, g, b, image_width, image_height);
            }
        }
    }
}

int manage(char* data, int* temp, char* index)//To convert the characters into usable integers
{
    char  de[20];
    int i = 0, j = 0, k = 0, l = 0, m= 0;

    while (*(data + i))
    {

        if (((*(data + i)) == 'c') || ((*(data + i)) == 'r'))
            {
                temp[j] = m;               // replacing the alphabet of the file with its position in the array index[]
                index[m] = *(data+i);      // placing the alphabet into the array index[]
                i++;
                j++;
                m++;
            }

        else if ((*(data + i)) >= '0' && (*(data + i)) <= '9')       //accepting certain characters to convert them to integers
        {
            int multiplier = 1, number = 0;

            while ((*(data + i)) >= '0' && (*(data + i)) <= '9')
            {
                de[k] = *(data + i);
                k++;
                i++;
            }

            for (l = k - 1; l >= 0; --l)                                      // converting into usable numbers
            {
                number += (de[l] - '0') * multiplier;
                multiplier *= 10;
            }
            temp[j] =number;                                        // keeping these numbers in an array
            j++; k = 0;

        }
        else
        {
            i++;
        }
    }
    return(m);                                                     //returning the end point of the index[] for later use
}



void main()
{
    FILE* file;
    int i, j, k;
    char index[20];
    int usables[100];


    // reading the render.txt
    file = fopen("render.txt", "rb");
    if (file == NULL)
    {
        printf("COULDN'T READ THE FILE.");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    unsigned int f_size = ftell(file);
    char* data = (char*)malloc(f_size + 1);
    fseek(file, 0, SEEK_SET);
    if (data) {
        fread(data, f_size, 1, file);
        data[f_size] = 0;
    }
    fclose(file);


    //manage the data to usables
    int stop = manage(data, usables, index);


    // assigning the datas

    int image_width = usables[0];
    int image_height = usables[1];
    int pixel_size = image_width * image_height * 3 * sizeof(unsigned char);
    unsigned char* pixels = malloc(pixel_size);
    memset(pixels, 0, pixel_size);

    
    j = 0;
    for(i=2;j<stop;)    // since first two numbers of the usables[] were height and width of the image. where, stop - 1 = the total number of indicators ('c' & 'r').
    {
        if (*(usables + i) == j)  // detecting the position of the indicators ('c' & 'r') in the usables[]
        {
            switch (index[j])
            {
            case 'r':
            {
                i++;
                int cx = (*(usables + i)); i++;
                int cy = (*(usables + i)); i++;
                int w = (*(usables + i)); i++;
                int h = (*(usables + i)); i++;
                uchar r = (uchar)(*(usables + i));  i++;
                uchar g = (uchar)(*(usables + i));  i++;
                uchar b = (uchar)(*(usables + i));   i++;
                draw_rect(pixels, cx, cy, w, h, r, g, b, image_width, image_height);
                break;
            }
            case 'c':
            {
                i++;
                int cx = (*(usables + i )); i++;
                int cy = (*(usables + i )); i++;
                int rad = (*(usables + i)); i++;
                uchar r = (uchar)(*(usables + i ));  i++;
                uchar g = (uchar) (*(usables + i ));  i++;
                uchar b = (uchar)(*(usables + i));   i++;
                draw_circle(pixels, cx, cy, rad, r, g, b, image_width, image_height);
                break;
            }
            default:
            {
                printf("\nERROR\n");
                break;
            }

            }

            j++;
        }
    }


    //writting in ppm file

    file = fopen("output.ppm", "w");
    if (file == NULL)
    {
        printf("\nERROR OPPENING A FILE.");
        exit(1);
    }
  
    fprintf(file, "P3 \n %d %d \n 255 \n", image_width, image_height);

    for (i = 0; i < image_height; i++)
    {
        for (j = 0; j < image_width; j++)
        {
            for (k = 0; k < 3; k++)
            {
                fprintf(file, " %d ", pixels[i * 800 * 3 + j * 3 + k]);                 // writing all the datas of canvas into the ppm file
            }
            fprintf(file, "\n");
        }
    }

    free(data);
    free(pixels);
    fclose(file);
}
