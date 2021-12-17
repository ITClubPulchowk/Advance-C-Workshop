#define BMPMALLOC(size) bump(size)
#include "bump.c"
#include "bmp_lekhak.h"

uint32_t rgb_to_hex(uint8_t r, uint8_t g, uint8_t b) {
    return r << 24 | g << 16 | b << 8;
}

int main() {
    init_bump_context(megabytes(2));
    BMP a = create_bmp(800, 600);
    for (int pos = 0; pos < 600 * 800; pos ++)
        a.pdata[pos] = rgb_to_hex(0, 0, 255);
    save_bmp("bruh.bmp", a);
    end_bump_context();
}
