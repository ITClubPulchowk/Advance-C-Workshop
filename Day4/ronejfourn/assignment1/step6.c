#include "mdlbrt_cmn.h"
#include "threading.h"
#include <stdlib.h>

volatile uint32_t progress;
volatile uint32_t in;

int draw(void *pxl) {
    uint32_t *can = (uint32_t *) pxl;
    uint32_t Py = interlocked_inc(&in);
    for (; in <= IM_HEIGHT / 2; Py = interlocked_inc(&in)) {
        Py -= 1;
        float y0 = map_range(0, IM_HEIGHT, MN_Y_SCALE_MIN, MN_Y_SCALE_MAX, Py);
        for(uint32_t Px = 0; Px < IM_WIDTH; Px ++) {
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
            can[Py * IM_WIDTH + Px] = color;
            can[(IM_HEIGHT - Py - 1) * IM_WIDTH + Px] = color;
            interlocked_inc(&progress);
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    init_bump_context(megabytes(1024));
    BMP image = create_bmp(IM_WIDTH, IM_HEIGHT);

    uint32_t thread_count;
    if (argc > 1) {
        thread_count = atoi(argv[1]);
    } else {
        thread_count = get_core_count();
    }

    thrd_t *rendering_threads = bump(thread_count * sizeof(*rendering_threads));

    for (int i = 0; i < thread_count; i ++) {
        thrd_create(&rendering_threads[i], draw, image.pdata);
    }

    while (progress < IM_WIDTH * IM_HEIGHT / 2) {
        thrd_sleep_millisecs(20);
        printf("Progress: %10f%%\r", (float)progress / (IM_WIDTH * IM_HEIGHT / 2.0) * 100);
    }
    printf("\n");

    for (int i = 0; i < thread_count; i ++) {
        thrd_join(rendering_threads[i], NULL);
    }

    save_bmp("image.bmp", image);
    end_bump_context();
}
