#include "mdlbrt_cmn.h"

int main() {
    init_bump_context(megabytes(512));
    BMP image = create_bmp(IM_WIDTH, IM_HEIGHT);

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
            image.pdata[Py * IM_WIDTH + Px] = color;
            image.pdata[(IM_HEIGHT - Py - 1) * IM_WIDTH + Px] = color;
        }
    }

    save_bmp("image.bmp", image);
    end_bump_context();
}
