#include <stdint.h>
#define BMPMALLOC(size) bump(size)

#include <math.h>
#include "bump.c"
#include "bmp_lekhak.h"

#define IM_WIDTH 800
#define IM_HEIGHT 600

#define MN_X_SCALE_MIN -2.00
#define MN_X_SCALE_MAX 0.47
#define MN_Y_SCALE_MIN -1.12
#define MN_Y_SCALE_MAX 1.12
#define MAX_ITER 1000

uint32_t rgb_to_hex(uint8_t r, uint8_t g, uint8_t b) {
    return r << 16 | g << 8 | b;
}

float scale(float pmin, float pmax, float nmin, float nmax, float value) {
    float ret = (value - pmin) * (nmax - nmin) / (pmax - pmin) + nmin;
    return ret;
}

float lerp (float v0, float v1, float t) {
    return (1 - t) * v0 + t * v1;
}

uint32_t get_color(float t) {
    uint8_t r = 0, g = 0, b = 0;

    if (t >= 0.0 && t < 0.16) {
        t = (t - 0) / (0.16 - 0);
        r = lerp(0  , 32 , t);
        g = lerp(7  , 107, t);
        b = lerp(100, 203, t);
    } else if (t >= 0.16 && t < 0.42) {
        t = (t - 0.16) / (0.42 - 0.16);
        r = lerp(32 , 237, t);
        g = lerp(107, 255, t);
        b = lerp(203, 255, t);
    } else if (t >= 0.42 && t < 0.6425) {
        t = (t - 0.42) / (0.6425 - 0.42);
        r = lerp(237, 255, t);
        g = lerp(255, 170, t);
        b = lerp(255, 0  , t);
    } else if (t >= 0.6425 && t < 0.8575) {
        t = (t - 0.6425) / (0.8575 - 0.6425);
        r = lerp(255, 0  , t);
        g = lerp(170, 2  , t);
        b = lerp(0  , 0  , t);
    } else {
        g = 2;
    }

    return rgb_to_hex(r, g, b);
}

int main() {
    init_bump_context(megabytes(512));
    BMP image = create_bmp(IM_WIDTH, IM_HEIGHT);

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
            uint32_t color = get_color((float)iteration / MAX_ITER);
            image.pdata[Py * IM_WIDTH + Px] = color;
            image.pdata[(IM_HEIGHT - Py - 1) * IM_WIDTH + Px] = color;
        }
    }

    save_bmp("mandelbrot.bmp", image);
    end_bump_context();
}
