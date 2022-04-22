#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    unsigned int cx, cy, r;
} CIRCLE;

typedef struct
{
    unsigned int cx, cy, width, height;
} RECT;

typedef struct
{
    unsigned char R, G, B;
} PIX;

typedef struct
{
    unsigned int row, column;
    PIX **pixel; //pixels stored in 2d array of rows and columns
} IMG;

IMG createLayout(unsigned int row, unsigned int col)
{
    IMG ppm_layout;
    ppm_layout.row = row;
    ppm_layout.column = col;
   
    ppm_layout.pixel = (PIX **)malloc(sizeof(PIX *) * ppm_layout.row);
    if (!ppm_layout.pixel)
    {
        printf("cannot allocate memory");
        exit(1);
    }

    for (int i = 0; i < ppm_layout.row; ++i)
    {
        *(ppm_layout.pixel + i) = (PIX *)malloc(sizeof(PIX *) * ppm_layout.column);
        if (!(*(ppm_layout.pixel + i)))
        {
            printf("cannot allocate memory");
            exit(1);
        }
    }
    return ppm_layout;
}

IMG BlackFill(IMG ppm_layout)
{
    for (int i = 0; i < ppm_layout.row; i++)
    {
        for (int j = 0; j < ppm_layout.column; j++)
        {
            ppm_layout.pixel[i][j].R = 0;
            ppm_layout.pixel[i][j].G = 0;
            ppm_layout.pixel[i][j].B = 0;         
        }
    }
    return ppm_layout;
}

IMG RenderRect(IMG ppm_layout, int cx, int cy, int width, int height, PIX pix)
{
    int init_posx = cx - width / 2;
    int init_posy = cy - height / 2;
    for (int i = init_posy; i < init_posy + height; i++)
    {
        for (int j = init_posx; j < init_posx + width; j++)
        {
            if (i >= 0 && i < ppm_layout.row && j >= 0 && j < ppm_layout.column) // to prevent access violation
                ppm_layout.pixel[i][j] = pix;
        }
    }
    return ppm_layout;
}

IMG RenderCircle(IMG ppm_layout, int cx, int cy, int radius, PIX pix)
{
    int init_posx = cx;
    int init_posy = cy;
    for (int i = 0; i < ppm_layout.row; i++)
    {
        for (int j = 0; j < ppm_layout.column; j++)
        {
            if ((j - cx) * (j - cx) + (i - cy) * (i - cy) <= radius * radius)
                ppm_layout.pixel[i][j] = pix;
        }
    }
    return ppm_layout;
}

void createPPM(IMG ppm_layout)
{
    FILE *fp2 = fopen("output.ppm", "wb");
    if (!fp2)
    {
        fprintf(stderr, "Cannot create file");
        exit(1);
    }

    fprintf(fp2, "P6\n%d %d\n255\n", ppm_layout.column, ppm_layout.row);
    for (int i = 0; i < ppm_layout.row; ++i)
    {
        for (int j = 0; j < ppm_layout.column; ++j)
            fprintf(fp2, "%c%c%c", ppm_layout.pixel[i][j].R, ppm_layout.pixel[i][j].G, ppm_layout.pixel[i][j].B);

        free(*(ppm_layout.pixel + i)); // freeing the heap memory accessed by each row
    }
    free(ppm_layout.pixel); // freeing the heap memory containing row starting addresses
    fclose(fp2);
}

int main()
{
    unsigned char shape;
    IMG ppm_pic;
    CIRCLE circle;
    RECT rect;
    PIX pix;
    FILE *fp = fopen("Render.txt", "rb");
    if (!fp)
    {
        fprintf(stderr, "Cannot open file");
        exit(1);
    }

    fscanf(fp, "%d %d\n", &ppm_pic.column, &ppm_pic.row);

    ppm_pic = createLayout(ppm_pic.row, ppm_pic.column);
    ppm_pic = BlackFill(ppm_pic);
    shape = getc(fp);
    while (shape == 'c' || shape == 'r')
    {
        if (shape == 'c')
        {
            fscanf(fp, " %u %u %u %hhu %hhu %hhu\n", &circle.cx, &circle.cy, &circle.r, &pix.R, &pix.G, &pix.B);
            ppm_pic = RenderCircle(ppm_pic, circle.cx, circle.cy, circle.r, pix);
             printf("%u %u %u %d %d %d\n", circle.cx, circle.cy, circle.r, pix.R, pix.G, pix.B);
        }
        else if (shape == 'r')
        {
            fscanf(fp, "%u %u %u %u %hhu %hhu %hhu\n", &rect.cx, &rect.cy, &rect.width, &rect.height, &pix.R, &pix.G, &pix.B);
            ppm_pic = RenderRect(ppm_pic, rect.cx, rect.cy, rect.width, rect.height, pix);
             printf("%u %u %u %u %hhu %hhu %hhu\n", rect.cx, rect.cy, rect.width, rect.height, pix.R, pix.G, pix.B);
        }
        shape = getc(fp);
    };

    createPPM(ppm_pic);
    fclose(fp);

    return 0;
}
