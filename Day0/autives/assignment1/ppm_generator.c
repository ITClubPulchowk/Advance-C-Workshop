#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef max
#define max(x, y) ((x) > (y) ? (x) : (y))
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

#define PPM_FORMAT "P6\n"
#define MAX_COLOR_VAL "255\n"

typedef struct Color {
    unsigned char r, g, b;
} Color;

typedef struct Pair {
    int x, y;
} Pair;

enum { Rectx, Recty, w, h, Rectr, Rectg, Rectb };
enum { Circlex, Circley, rad, Circler, Circleg, Circleb };

void DrawRect(char* data, Pair* pixelsPos, Color* pixelsColor, int pixelCount, int imageSize[2]) {
    int values[7];
    char count = 0;
    while (*data) {
        if (*data >= '0' && *data <= '9') {
            values[count] = strtoll(data, &data, 10);
            count++;
        }
        else
            data++;
    }
    for (int i = values[Recty]; i < min(values[Recty] + values[h], imageSize[1]); i++) {
        for (int j = values[Rectx]; j < min(values[Rectx] + values[w], imageSize[0]); j++) {
            pixelsColor[i * 800 + j].r = values[Rectr];
            pixelsColor[i * 800 + j].g = values[Rectg];
            pixelsColor[i * 800 + j].b = values[Rectb];
        }
    }
}

void DrawCircle(char* data, Pair* pixelsPos, Color* pixelsColor, int pixelCount, int imageSize[2]) {
    int values[6];
    char count = 0;
    while (*data) {
        if (*data >= '0' && *data <= '9') {
            values[count] = strtoll(data, &data, 10);
            count++;
        }
        else
            data++;
    }
    for (int i = max(values[Circley] - values[rad], 0); i < min(values[Circley] + values[rad], imageSize[1]); i++) {
        for (int j = max(values[Circlex] - values[rad], 0); j < min(values[Circlex] + values[rad], imageSize[0]); j++) {
            if (((pixelsPos[i*800 + j].x - values[Circlex]) * (pixelsPos[i*800 + j].x - values[Circlex]))
                + ((pixelsPos[i*800 + j].y - values[Circley]) * (pixelsPos[i*800 + j].y - values[Circley]))
                <= values[rad] * values[rad]) {
                pixelsColor[i*800 + j].r = values[Circler];
                pixelsColor[i*800 + j].g = values[Circleg];
                pixelsColor[i*800 + j].b = values[Circleb];
            }
        }
    }
}

int FileSize(FILE* ptr) {
    fseek(ptr, 0, SEEK_END);
    int size = ftell(ptr);
    fseek(ptr, 0, SEEK_SET);
    return size;
}

int main() {

    FILE* sourceFile = fopen("render.txt", "rb");
    if (!sourceFile)
        exit(2);

    int fileSize = FileSize(sourceFile);
    char* sourceTxt = malloc(fileSize);
    fread(sourceTxt, sizeof(char), fileSize, sourceFile);
    fclose(sourceFile);

    char* sizeData = malloc(10 * sizeof(char));
    char pos = 0;
    do
    {
        sizeData[pos] = sourceTxt[pos];
    } while (*(sourceTxt + pos++) != '\n');

    FILE* destFile = fopen("output.ppm", "wb");
    fwrite(PPM_FORMAT, sizeof(char), 3, destFile);
    fwrite(sizeData, sizeof(char), pos, destFile);
    fwrite(MAX_COLOR_VAL, sizeof(char), 4, destFile);

    int imageSize[2];
    char foo = 0;
    while (*sizeData) {
        if (*sizeData >= '0' && *sizeData <= '9') {
            imageSize[foo] = strtoll(sizeData, &sizeData, 10);
            foo = 1;
        }
        else
            sizeData++;
    }

    int shapeCount = 2;
    char** shapeData = malloc(shapeCount * sizeof(char*));
    char* token;
    int num = 0;
    token = strtok(sourceTxt + pos, "\n");
    while (token) {
        if (num >= shapeCount) {
            shapeCount += 5;
            shapeData = realloc(shapeData, shapeCount * sizeof(char*));
        }
        shapeData[num] = token;
        token = strtok(NULL, "\n");
        num++;
    }

    int pixelCount = imageSize[0] * imageSize[1];
    Pair* pixelsPos = malloc(pixelCount * sizeof(Pair));
    Color* pixelsColor = calloc(pixelCount, sizeof(Color));
    for (int i = 0; i < pixelCount; i++) {
        pixelsPos[i].y = i / imageSize[0];
        pixelsPos[i].x = i - pixelsPos[i].y * imageSize[0];
    }

    for (int i = 0; i < num; i++) {
        if (shapeData[i][0] == 'r') {
            DrawRect(shapeData[i] + 2, pixelsPos, pixelsColor, pixelCount, imageSize);
        }
        else if (shapeData[i][0] == 'c') {
            DrawCircle(shapeData[i] + 2, pixelsPos, pixelsColor, pixelCount, imageSize);
        }
    }

    fwrite(pixelsColor, sizeof(Color), pixelCount, destFile);
    fclose(destFile);

    return 0;
}