#include "mdlbrt_cmn.h"
#include "threading.h"

static float progress;

int draw(void *pxl_arr) {
    uint32_t *canvas = (uint32_t *)pxl_arr;
    for (int Py = 0; Py < IM_HEIGHT / 2; Py ++) {
        float y0 = map_range(0, IM_HEIGHT, MN_Y_SCALE_MIN, MN_Y_SCALE_MAX, Py);
        for(int Px = 0; Px < IM_WIDTH; Px ++) {
            float x0 = map_range(0, IM_WIDTH, MN_X_SCALE_MIN, MN_X_SCALE_MAX, Px);
            float x = 0, y = 0, x2 = 0, y2 = 0;
            int iteration = 0;
            while (x2 + y2 <= 4 && iteration < MAX_ITER) {
                y = 2 * x * y + y0;
                x = x2 - y2 + x0;
                x2 = x * x;
                y2 = y * y;
                iteration += 1;
            }
            uint32_t color = get_color(iteration);
            canvas[Py * IM_WIDTH + Px] = color;
            canvas[(IM_HEIGHT - Py - 1) * IM_WIDTH + Px] = color;
            progress = (float)(Py * IM_WIDTH + Px) / ((IM_HEIGHT / 2.0 - 1) * (IM_WIDTH) + IM_WIDTH - 1);
        }
    }
    return 0;
}

int main() {
    init_bump_context(megabytes(512));
    BMP image = create_bmp(IM_WIDTH, IM_HEIGHT);

    thrd_t renderer_thread;
    thrd_create(&renderer_thread, draw, (void *)image.pdata);

    while (progress < 1) {
        thrd_sleep_millisecs(20);
        printf("Progress: %10f%%\r", progress * 100);
    }
    printf("\n");

    thrd_join(renderer_thread, NULL);
    save_bmp("mandelbrot.bmp", image);
    end_bump_context();
}
