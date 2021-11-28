#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SQUARE(anything) (anything*anything)
#define renderfile "render.txt"
#define resultfile "output.ppm"

#define circlescale 5
#define ppmscale 25
#define ppmshow 1
#define circleshow 0
#define rectangleshow 0
#define circledetails 0
#define rectangledetails 0

/* Structures used  */

struct coordinate
{
    int x, y;
};

struct pixel
{
    unsigned char r, g, b;
};

struct rectangle
{
    struct coordinate start, size;
    struct pixel colour;
};

struct circle
{
    struct coordinate centre;
    int radius;
    struct pixel colour;
};

struct box
{
    struct coordinate start, end;
};

/* Functions used*/
int squareof(int number)
{
    return number*number;
}

char  writepixel(void * image_data, struct coordinate size, struct coordinate coord, struct pixel pxl)
{
    * (((struct pixel *) image_data) + (coord.y * size.x + coord.x ) ) = pxl;
    return -1;
}

struct box cut_object_box_using_image_size(struct coordinate image_size, struct box object_box)
{
    if (object_box.start.x < 0)
        object_box.start.x = 0;

    if (object_box.start.y < 0)
        object_box.start.y = 0;

    if (object_box.end.x > image_size.x)
        object_box.end.x = image_size.x;

    if (object_box.end.y > image_size.y)
        object_box.end.y = image_size.y;

    return object_box;

}

void makerectangle(void * image_data, struct coordinate image_size, struct rectangle rect)
{
    struct box rectanglebox =
    {
        rect.start,
        {rect.start.x + rect.size.x, rect.start.y + rect.size.y}
    };

    rectanglebox = cut_object_box_using_image_size(image_size, rectanglebox);

    if(rectangledetails)
    printf("\nMaking rectangle on boundary %d, %d and %d, %d of color %d, %d, %d\n",
           rectanglebox.start.x, rectanglebox.start.y, rectanglebox.end.x, rectanglebox.end.y, rect.colour.r, rect.colour.g, rect.colour.b);

    for(int x = rectanglebox.start.x; x < rectanglebox.end.x; x++)
    {
        if(rectangleshow)
        printf("\n");
        for(int y = rectanglebox.start.y; y < rectanglebox.end.y; y++)
        {
            writepixel(image_data, image_size, (struct coordinate) {x, y}, rect.colour);
            if(rectangleshow)
            printf("##");
        }
    }

    if(rectangleshow)
    printf("\n\n");
}

void makecircle(void * image_data, struct coordinate image_size, struct circle circ)
{
    struct box circlebox =
    {
        {circ.centre.x - circ.radius, circ.centre.y - circ.radius},
        {circ.centre.x + circ.radius, circ.centre.y + circ.radius}
    };

    circlebox = cut_object_box_using_image_size(image_size, circlebox);

    if(circledetails)
    printf("\nMaking circle on boundary %d, %d and %d, %d having centre %d, %d and radius %d and of color %d, %d, %d\n",
           circlebox.start.x, circlebox.start.y, circlebox.end.x, circlebox.end.y,
           circ.centre.x, circ.centre.y, circ.radius,
           circ.colour.r, circ.colour.g, circ.colour.b);

    for(int x = circlebox.start.x; x < circlebox.end.x; x++)
    {
        if(circleshow)
            if(x % circlescale == 0)
                printf("\n");

        for(int y = circlebox.start.y; y < circlebox.end.y; y++)
        {
            if(squareof(x-circ.centre.x) + squareof(y - circ.centre.y) <= squareof(circ.radius))
            {
                if(circleshow)
                    if(x % circlescale == 0 && y % circlescale == 0)
                        printf("##");
                writepixel(image_data, image_size, (struct coordinate) {x, y}, circ.colour);
            }
            else
            {
                if(circleshow)
                    if(x % circlescale == 0 && y % circlescale == 0)
                        printf("  ");
            }
        }
    }
    if(circleshow)
        printf("\n\n");
}

void showppm(void * image_data, struct coordinate image_size)
{
        for(int y = 0; y< image_size.y; y++)
        {
            if(y % ppmscale == 0)
                printf("\n");
            for(int x = 0; x < image_size.x; x++)
            {
                struct pixel p = *( (struct pixel *) image_data + x + y * (image_size.x));
                if(x % ppmscale == 0 && y % ppmscale == 0)
                {
                    int totalcolor = p.r + p.g + p.b;
                    if( (totalcolor) )
                        printf("##");
                    else {
                        printf("  ");
                    }
                }
            }
        }
}

void makeppm(char * filename, void * image_data, struct coordinate image_size, size_t sizeof_image_data)
{

    char initial_text[30];
    sprintf(initial_text,
           "P6\n"
            "%d %d\n"
            "255\n%c", image_size.x, image_size.y, '\0');
    if(ppmshow)
    {
        printf("%15s",initial_text);
        showppm(image_data, image_size);
    }
    FILE * myfile = fopen(filename, "w");

    fprintf(myfile, "%s",initial_text);
     // fwrite(initial_text, 15, 1, myfile); // for binary mode
    fwrite(image_data, sizeof_image_data, 1, myfile);
}


int main()
{
    FILE * myfile = fopen(renderfile, "r");
    if(myfile)
    {
        struct coordinate image_size;
        fscanf(myfile, "%d %d", &image_size.x, &image_size.y);

        if(image_size.x && image_size.y) /* when any of image width or
                                         * image height is zero, no image
                                         * can be formed, and the code
                                         * shouldn't run */
        {
            size_t sizeof_image_data = sizeof(struct pixel) * image_size.x * image_size.y;
            struct pixel * image_data = (struct pixel *) calloc(image_size.x * image_size.y, sizeof(struct pixel));
        
            char letter = fgetc(myfile); // Initially just consumes return character

            while(letter != EOF)
            {
                
                letter = fgetc(myfile);
                switch(letter)
                {
                    case 'r':
                    {
                        fgetc(myfile);
                        struct rectangle rect;
                        int rr, gg, bb;
                        fscanf(myfile, "%d %d %d %d %d %d %d",
                               /* Starting coordinate */ &rect.start.x, &rect.start.y,
                               /* Size of rectangle */   &rect.size.x, &rect.size.y,
                               /* Color of rectangle */  &rr, &gg, &bb);
                               
                               rect.start.x -= rect.size.x/2;
                               rect.start.y -= rect.size.y/2;

                        rect.colour = (struct pixel){rr, gg, bb};
                        makerectangle(image_data, image_size, rect);
                    }
                    break;

                    case 'c':
                    {
                        fgetc(myfile);
                        struct circle circ;
                        int rr, gg, bb;
                        fscanf(myfile, "%d %d %d %d %d %d",
                               /* Centre */&circ.centre.x, &circ.centre.y,
                               /* Radiuis */&circ.radius,
                               /* Color of circle */&rr, &gg, &bb);

                        circ.colour = (struct pixel){rr, gg, bb};
                        makecircle(image_data, image_size, circ);
                    }
                    break;
                }
            }        
            makeppm(resultfile, image_data, image_size, sizeof_image_data);
            free(image_data);
        }
        
    }
    fclose(myfile);
}
