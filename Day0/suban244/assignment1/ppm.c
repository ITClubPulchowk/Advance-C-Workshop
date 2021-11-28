#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct Pixel {
  unsigned char r, g, b;
};

void writeCircle(FILE *input, FILE *output, struct Pixel *pixels,
                 int imageWidth, int imageHeight) {
  struct Pixel p;
  int centerX, centerY, radius;
  int loopStartW, loopStartH, loopEndW, loopEndH;
  short r, g, b;

  fscanf(input, "%d %d %d %hu %hu %hu\n", &centerX, &centerY, &radius, &r, &g,
         &b);
  p.r = r;
  p.g = g;
  p.b = b;
  printf("%hu %hu %hu", p.r, p.g, p.b);

  // Capping the for loops
  loopStartW = (centerX - radius < 0) ? 0 : centerX - radius;
  loopStartH = (centerY - radius < 0) ? 0 : centerY - radius;
  loopEndW =
      (centerX + radius - 1 < imageWidth) ? centerX + radius : imageWidth;
  loopEndH =
      (centerY + radius - 1 < imageHeight) ? centerY + radius : imageHeight;

  for (int w = loopStartW; w <= loopEndW; w++) {
    for (int h = loopStartH; h <= loopEndH; h++) {
      if (sqrt(pow((w - centerX), 2) + pow((h - centerY), 2)) <= radius) {
        pixels[imageWidth * h + w] = p;
      }
    }
  }
}

void writeRect(FILE *input, FILE *output, struct Pixel *pixels, int imageWidth,
               int imageHeight) {

  struct Pixel p;
  int centerX, centerY, width, height;
  int loopStartW, loopStartH, loopEndW, loopEndH;
  short r, g, b;
  fscanf(input, "%d %d %d %d %hu %hu %hu\n", &centerX, &centerY, &width,
         &height, &r, &g, &b);
  p.r = r;
  p.g = g;
  p.b = b;
  printf("%hu %hu %hu", p.r, p.g, p.b);
  // Capping the for loops
  loopStartW = (centerX - width / 2 < 0) ? 0 : centerX - width / 2;
  loopStartH = (centerY - height / 2 < 0) ? 0 : centerY - height / 2;
  loopEndW =
      (centerX + width / 2 - 1 < imageWidth) ? centerX + width / 2 : imageWidth;
  loopEndH = (centerY + height / 2 - 1 < imageHeight) ? centerY + height / 2
                                                      : imageHeight;

  for (int w = loopStartW; w < loopEndW; w++) {
    for (int h = loopStartH; h < loopEndH; h++) {
      pixels[imageWidth * h + w] = p;
    }
  }
}

void writeToFile(FILE *output, struct Pixel *pixels, int imageWidth,
                 int imageHeight) {
  for (int i = 0; i < imageHeight * imageWidth; i++) {
    fprintf(output, "%hu %hu %hu", pixels[i].r, pixels[i].g, pixels[i].b);
    if (i % imageWidth == imageWidth - 1) {
      fprintf(output, "\n");
    } else {
      fprintf(output, " ");
    }
  }
}

int main() {
  int imageWidth, imageHeight;
  char character;
  struct Pixel *pixels;

  FILE *input = fopen("render.txt", "rb");
  FILE *output = fopen("output.ppm", "w");

  if (input == NULL) {
    printf("Error");
    exit(1);
  }

  fscanf(input, "%d %d\n", &imageWidth, &imageHeight);

  // Write basic info to file
  fprintf(output, "P3\n");
  fprintf(output, "%d %d\n", imageWidth, imageHeight);
  fprintf(output, "255\n");
  pixels =
      (struct Pixel *)calloc(imageHeight * imageHeight, (sizeof(struct Pixel)));

  while ((character = fgetc(input)) != EOF) {
    fgetc(input);
    switch (character) {
    case 'c':
      writeCircle(input, output, pixels, imageWidth, imageHeight);
      break;
    case 'r':
      writeRect(input, output, pixels, imageWidth, imageHeight);
      break;
    default:
      break;
    }
  }

  writeToFile(output, pixels, imageWidth, imageHeight);
  fclose(input);
  fclose(output);
}
