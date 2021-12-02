#include <stdio.h>
#include <stdlib.h>


typedef struct {
     long r;
     long g;
     long b;
} Pixel;

int main() {
  char *lineBuffer;
  long length;
  Pixel *pixelBuffer;
  long height, width;
  char checkShape;
  long field_a, field_b, field_c, field_d, field_e, field_f, field_g;
  FILE* render = fopen("render.txt", "r");
  FILE* output = fopen("output.ppm", "wb");

  fseek(render, 0, SEEK_END);
  length = ftell(render);
  fseek(render, 0, SEEK_SET);
  printf("Length : %ld\n", length);

  lineBuffer = malloc(length * sizeof(char));

  fscanf(render, "%ld %ld", &width, &height);
  fprintf(output, "P3\n%ld %ld\n255\n", height, width);
  printf("P3\n%ld %ld\n255\n", height, width);

  pixelBuffer = malloc(height * width * sizeof(Pixel));


  for(long i = 0; i < height * width; i++) {
        pixelBuffer[i].r = 0;
        pixelBuffer[i].g = 0;
        pixelBuffer[i].b = 0;
  }


	while(fgets(lineBuffer, length, render) != 0){
        sscanf(lineBuffer, " %c", &checkShape);
        if(checkShape == 'c') {
            sscanf(lineBuffer, " %c %ld %ld %ld %ld %ld %ld", &checkShape,\
                &field_a, \
                &field_b, \
                &field_c, \
                &field_d, \
                &field_e, \
                &field_f \
            );

            for(long i = -field_c; i < field_c; i++) {
                for(long j = -field_c; j < field_c; j++) {
                   if(field_a + i > 0 && field_b + j > 0) {
                   long id = (field_a + i) + (field_b + j) * width;
                   if((i*i + j*j) < field_c * field_c) {
                      pixelBuffer[id].r = field_d;
                      pixelBuffer[id].g = field_e;
                      pixelBuffer[id].b = field_f;
                        }
                    }
                }
            }
        } else if(checkShape == 'r') {
            sscanf(lineBuffer , " %c %ld %ld %ld %ld %ld %ld %ld\n", &checkShape,\
                &field_a, \
                &field_b, \
                &field_c, \
                &field_d, \
                &field_e, \
                &field_f, \
                &field_g \
            );


            for(long i = 0; i < field_c; i++) {
                for(long j = 0; j < field_d; j++) {
                   long id = (field_a + i) + (field_b + j) * width;
                   pixelBuffer[id].r = field_e;
                   pixelBuffer[id].g = field_f;
                   pixelBuffer[id].b = field_g;
                }
            }
       }
	}
    free(lineBuffer);

    for(long i = 0; i < height * width; i++) {
            fprintf(output, "%ld\n%ld\n%ld\n", pixelBuffer[i].r, pixelBuffer[i].g, pixelBuffer[i].b);
    }

    free(pixelBuffer);
    fclose(output);
    fclose(render);
	return 0;
}
