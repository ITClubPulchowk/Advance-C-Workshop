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
    PIX *pixel;
} IMG;

IMG createLayout(unsigned int row, unsigned int col)
{
    IMG ppm_layout;
    ppm_layout.row = row;
    ppm_layout.column = col;
    int a = sizeof(PIX);
    ppm_layout.pixel = (PIX *)malloc(sizeof(PIX) * ppm_layout.row * ppm_layout.column);

    if (!ppm_layout.pixel)
    {
        printf("cannot allocate memory");
        exit(1);
    }
    return ppm_layout;
}

void BlackFill(IMG ppm_layout)
{
    for (int i = 0; i < ppm_layout.row; i++)
    {
        for (int j = 0; j < ppm_layout.column; j++)
        {
            ppm_layout.pixel[i * ppm_layout.column + j].R = 0;
            ppm_layout.pixel[i * ppm_layout.column + j].G = 0;
            ppm_layout.pixel[i * ppm_layout.column + j].B = 0;
        }
    }
}

void RenderRect(IMG ppm_layout, int cx, int cy, int width, int height, PIX pix)
{
    int init_posx = cx - width / 2;
    int init_posy = cy - height / 2;

    for (int i = init_posy; i < init_posy + height; i++)
    {
        for (int j = init_posx; j < init_posx + width; j++)
        {
            if (i >= 0 && i < ppm_layout.row && j >= 0 && j < ppm_layout.column) // to prevent access violation
                ppm_layout.pixel[i * ppm_layout.column + j] = pix;
        }
    }
}

void RenderCircle(IMG ppm_layout, int cx, int cy, int radius, PIX pix)
{
    for (int i = 0; i < ppm_layout.row; i++)
    {
        for (int j = 0; j < ppm_layout.column; j++)
        {
            if ((j - cx) * (j - cx) + (i - cy) * (i - cy) <= radius * radius)
                ppm_layout.pixel[i * ppm_layout.column + j] = pix;
        }
    }
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
            fprintf(fp2, "%c%c%c", ppm_layout.pixel[i * ppm_layout.column + j].R, ppm_layout.pixel[i * ppm_layout.column + j].G, ppm_layout.pixel[i * ppm_layout.column + j].B);
    }
    free(ppm_layout.pixel);
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

    BlackFill(ppm_pic);

    shape = getc(fp);
    
    while (shape == 'c' || shape == 'r')
    {
        if (shape == 'c')
        {
            fscanf(fp, " %u %u %u %hhu %hhu %hhu\n", &circle.cx, &circle.cy, &circle.r, &pix.R, &pix.G, &pix.B);
            RenderCircle(ppm_pic, circle.cx, circle.cy, circle.r, pix);
            //printf("%u %u %u %d %d %d\n", circle.cx, circle.cy, circle.r, pix.R, pix.G, pix.B);
        }
        else if (shape == 'r')
        {
            fscanf(fp, "%u %u %u %u %hhu %hhu %hhu\n", &rect.cx, &rect.cy, &rect.width, &rect.height, &pix.R, &pix.G, &pix.B);
            RenderRect(ppm_pic, rect.cx, rect.cy, rect.width, rect.height, pix);
            //printf("%u %u %u %u %hhu %hhu %hhu\n", rect.cx, rect.cy, rect.width, rect.height, pix.R, pix.G, pix.B);
        }
        shape = getc(fp);
    }

    createPPM(ppm_pic);

    fclose(fp);

    return 0;
}


