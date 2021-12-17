#ifndef BMPLEKHAK
#define BMPLEKHAK

#ifndef BMPMALLOC
#include <stdlib.h>
#define BMPMALLOC(size) malloc(size)
#endif

#include <stdint.h>
#include <stdio.h>

#define BM 0x4d42

#pragma pack(push, headers, 1)

typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFOHEADER;

#pragma pack(pop, headers)

typedef struct {
    BITMAPFILEHEADER fheader;
    BITMAPINFOHEADER iheader;
    uint32_t *pdata;
} BMP;

#define TOTALHEADERSIZE (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER))

static uint32_t get_img_size(BMP t) {
    return t.fheader.bfSize - TOTALHEADERSIZE;
}

static BMP create_bmp(uint32_t width, uint32_t height) {
    BMP ret;
    ret.fheader.bfType      = BM;
    ret.fheader.bfSize      = TOTALHEADERSIZE + (width * height * 4);
    ret.fheader.bfReserved1 = 0;
    ret.fheader.bfReserved2 = 0;
    ret.fheader.bfOffBits   = TOTALHEADERSIZE;

    ret.iheader.biSize   = sizeof(BITMAPINFOHEADER);
    ret.iheader.biWidth  =   width ;
    ret.iheader.biHeight = - height;
    ret.iheader.biPlanes = 1;
    ret.iheader.biBitCount = 32;
    ret.iheader.biCompression = 0;
    ret.iheader.biSizeImage   = 0;
    ret.iheader.biXPelsPerMeter = 0;
    ret.iheader.biYPelsPerMeter = 0;
    ret.iheader.biClrUsed       = 0;
    ret.iheader.biClrImportant  = 0;

    ret.pdata = (uint32_t *)BMPMALLOC(width * height * 4);

    return ret;
}

static void save_bmp(const char *filename, BMP bmp) {
    FILE *fout = fopen(filename, "wb");
    fwrite(&bmp.fheader, sizeof(BITMAPFILEHEADER), 1, fout);
    fwrite(&bmp.iheader, sizeof(BITMAPINFOHEADER), 1, fout);
    fwrite(bmp.pdata   , get_img_size(bmp)       , 1, fout);
    fclose(fout);
}

#endif
