#define BMPMALLOC(size) bump(size)

#include "bump.c"
#include "bmp_lekhak.h"

/* for each pixel (Px, Py) on the screen do */
/*     x0 := scaled x coordinate of pixel (scaled to lie in the Mandelbrot X scale (-2.00, 0.47)) */
/*     y0 := scaled y coordinate of pixel (scaled to lie in the Mandelbrot Y scale (-1.12, 1.12)) */
/*     x := 0.0 */
/*     y := 0.0 */
/*     iteration := 0 */
/*     max_iteration := 1000 */
/*     x2 := 0 */
/*     y2 := 0 */
/*  */
/*     while (x2 + y2 ≤ 4 and iteration < max_iteration) do */
/*         y := 2 × x × y + y0 */
/*         x := x2 - y2 + x0 */
/*         x2 := x × x */
/*         y2 := y × y */
/*         iteration := iteration + 1 */
/*  */
/*     color := palette[iteration] */
/*     plot(Px, Py, color) */

#define IM_WIDTH 800
#define IM_HEIGHT 600

#define MN_X_SCALE_MIN -2.00
#define MN_X_SCALE_MAX 0.47
#define MN_Y_SCALE_MIN -1.12
#define MN_Y_SCALE_MAX 1.12
#define MAX_ITER 1000

uint32_t rgb_to_hex(uint8_t r, uint8_t g, uint8_t b) {
    return r << 24 | g << 16 | b << 8;
}

float scale(float pmin, float pmax, float nmin, float nmax, float value) {
    float ret = (value - pmin) * (nmax - nmin) / (pmax - pmin) + nmin;
    return ret;
}

int main() {
    init_bump_context(megabytes(512));
    BMP image = create_bmp(IM_WIDTH, IM_HEIGHT);

    uint32_t palette[] = {
        rgb_to_hex(  0,   2,   0),
        rgb_to_hex(255, 170,   0),
        rgb_to_hex(237, 255, 255),
        rgb_to_hex( 32, 107, 203),
        rgb_to_hex(  0,   7, 100)
    };

    for (int Py = 0; Py < IM_HEIGHT / 2; Py ++) {
        float y0 = scale(0, IM_HEIGHT, MN_Y_SCALE_MIN, MN_Y_SCALE_MAX, Py);
        for(int Px = 0; Px < IM_WIDTH; Px ++) {
            float x0 = scale(0, IM_WIDTH, MN_X_SCALE_MIN, MN_X_SCALE_MAX, Px);
            float x = 0, y = 0, x2 = 0, y2 = 0;
            int iteration = 0;
            while (x2 + y2 <= 4 && iteration < MAX_ITER) {
                y = 2 * x * y + y0;
                x = x2 - y2 + x0;
                x2 = x * x;
                y2 = y * y;
                iteration += 1;
            }
            image.pdata[Py * IM_WIDTH + Px] = palette[iteration % 5];
            image.pdata[(IM_HEIGHT - Py - 1) * IM_WIDTH + Px] = palette[iteration % 5];
        }
    }

    save_bmp("mandelbrot.bmp", image);
    end_bump_context();
}
