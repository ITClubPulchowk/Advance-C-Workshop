#include "mdlbrt_cmn.h"
#include "threading.h"

volatile uint32_t progress;
typedef struct {
    uint32_t *chunk;
    uint32_t start;
    uint32_t height;
} canvas;

int draw(void *pxl) {
    canvas *can = (canvas *) pxl;
    for (int Py = can->start; Py < can->start + can->height && Py <= IM_HEIGHT / 2; Py ++) {
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
            can->chunk[Py * IM_WIDTH + Px] = color;
            can->chunk[(IM_HEIGHT - Py - 1) * IM_WIDTH + Px] = color;
            interlocked_inc(&progress);
        }
    }
    return 0;
}

int main() {
    init_bump_context(megabytes(1024));
    timer tmr;
    FILE *outf = fopen("threadperf.csv", "wb");
    fprintf(outf, "Threads,Performance\n");
    for (uint32_t thread_count = 1; thread_count <= get_core_count() * 2; thread_count ++){
        progress = 0;
        printf("Thread Count: %10d\n", thread_count);
        start_timing(&tmr);
        BMP image = create_bmp(IM_WIDTH, IM_HEIGHT);

        thrd_t *rendering_threads = bump(thread_count * sizeof(*rendering_threads));
        uint32_t height_for_threads = IM_HEIGHT / 2 / thread_count;
        canvas *a = bump(thread_count * sizeof(*a));

        for (int i = 0; i < thread_count; i ++) {
            a[i].chunk  = image.pdata;
            a[i].start  = i * height_for_threads;
            a[i].height = height_for_threads;
        }

        if (a[thread_count - 1].start + a[thread_count - 1].height != IM_HEIGHT / 2) {
            a[thread_count - 1].height = IM_HEIGHT / 2 - a[thread_count - 1].start;
        }

        for (int i = 0; i < thread_count; i ++) {
            thrd_create(&rendering_threads[i], draw, &a[i]);
        }

        while (progress < IM_WIDTH * IM_HEIGHT / 2) {
            thrd_sleep_millisecs(20);
            printf("Progress: %10f%%\r", (float)progress / (IM_WIDTH * IM_HEIGHT / 2.0) * 100);
        }
        printf("\n");

        for (int i = 0; i < thread_count; i ++) {
            thrd_join(rendering_threads[i], NULL);
        }
        uint32_t diff = end_timing(tmr);
        printf("Time Elapsed: %10d ms\n", diff);
        printf("===========================\n");
        fprintf(outf, "%d,%d\n", thread_count, diff);
        reset_bump_context();
    }
    fclose(outf);
    end_bump_context();
}
