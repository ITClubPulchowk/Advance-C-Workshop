#include "thread_pool.h"
#include <SDL2/SDL.h>

SDL_Window *window;
SDL_Surface *surface;
uint32_t *pixels;

static float translationX = 0, translationY = 0;
static float scale  = 1;
void screen_to_world(float nsx, float nsy, float *fwx, float *fwy) {
    *fwx = (nsx) / scale + translationX;
    *fwy = (nsy) / scale + translationY;
}

#define WIDTH 1280
#define HEIGHT 720

#define MN_X_SCALE_MIN -4.00
#define MN_X_SCALE_MAX 4.00
#define MN_Y_SCALE_MIN -2.25
#define MN_Y_SCALE_MAX 2.25
#define MAX_ITER 10000

typedef struct {
    float r, g, b;
} color;

static color palette[] = {
    {0.258824, 0.117647, 0.058824},
    {0.098039, 0.027451, 0.101961},
    {0.035294, 0.003922, 0.184314},
    {0.015686, 0.015686, 0.286275},
    {0.000000, 0.027451, 0.392157},
    {0.047059, 0.172549, 0.541176},
    {0.094118, 0.321569, 0.694118},
    {0.223529, 0.490196, 0.819608},
    {0.525490, 0.709804, 0.898039},
    {0.827451, 0.925490, 0.972549},
    {0.945098, 0.913726, 0.749020},
    {0.972549, 0.788235, 0.372549},
    {1.000000, 0.666667, 0.000000},
    {0.800000, 0.501961, 0.000000},
    {0.600000, 0.341176, 0.000000},
    {0.415686, 0.203922, 0.011765},
};

static inline uint32_t rgb_to_hex(uint8_t r, uint8_t g, uint8_t b) {
    return 0xFF << 24 | r << 16 | g << 8 | b;
}

static inline float map_range(float pmin, float pmax, float nmin, float nmax, float value) {
    float ret = (value - pmin) * (nmax - nmin) / (pmax - pmin) + nmin;
    return ret;
}

static inline float lerp (float v0, float v1, float t) {
    return (1 - t) * v0 + t * v1;
}

static inline uint32_t get_color(uint32_t iter) {
    uint8_t r = 0, g = 0, b = 0;
    float t = (float)iter / MAX_ITER;
    color clr1 = palette[(iter    ) & 15];
    color clr2 = palette[(iter + 1) & 15];
    r = lerp(clr1.r, clr2.r, t) * 255;
    g = lerp(clr1.g, clr2.g, t) * 255;
    b = lerp(clr1.b, clr2.b, t) * 255;
    return rgb_to_hex(r, g, b);
}

uint32_t srq_len;
volatile uint32_t offset;
volatile uint32_t turn;

uint32_t get_iter(float x0, float y0) {
    float nx0, ny0;
    screen_to_world(x0, y0, &nx0, &ny0);

    float x = 0, y = 0, x2 = 0, y2 = 0;
    int iteration = 0;

    float q = (nx0 - 0.25) * (nx0 - 0.25) + ny0 * ny0;
    if (q * (q + (nx0 - 0.25)) <= 0.25 * ny0 * ny0 || (nx0 + 1) * (nx0 + 1) + ny0 * ny0 <= 1 / 16.0) {
        iteration = MAX_ITER;
    } else {
        while (x2 + y2 <= 4 && iteration < MAX_ITER) {
            y = 2 * x * y + ny0;
            x = x2 - y2 + nx0;
            x2 = x * x;
            y2 = y * y;
            iteration += 1;
        }
    }

    return iteration;
}

int draw(void *arg) {
    while (1) {
        thrd_sleep_millisecs(1);
        if (offset >= HEIGHT) {
            thrd_sleep_millisecs(32);
        } else {
        uint32_t offs = interlocked_add(&offset, srq_len) - srq_len;

        float y0 = map_range(0, HEIGHT, MN_Y_SCALE_MIN, MN_Y_SCALE_MAX, offs);
        float y1 = map_range(0, HEIGHT, MN_Y_SCALE_MIN, MN_Y_SCALE_MAX, offs + srq_len);

        for(uint32_t Px = 0; Px < WIDTH; Px += srq_len) {

            float x0 = map_range(0, WIDTH , MN_X_SCALE_MIN, MN_X_SCALE_MAX, Px);
            float x1 = map_range(0, WIDTH , MN_X_SCALE_MIN, MN_X_SCALE_MAX, Px + srq_len);

            uint32_t iter0 = get_iter(x0, y0);
            uint32_t iter1 = get_iter(x1, y0);
            uint32_t iter2 = get_iter(x1, y1);
            uint32_t iter3 = get_iter(x0, y1);

            if (iter0 == iter1 && iter0 == iter2 && iter0 == iter3) {
                uint32_t color = get_color(iter0);
                for (uint32_t ty = offs; ty < offs + srq_len && ty < HEIGHT; ty ++) {
                    for (uint32_t tx = Px; tx < Px + srq_len && tx < WIDTH; tx ++) {
                        pixels[ty * WIDTH + tx] = color;
                    }
                }
            } else {
                for (uint32_t ty = offs; ty < offs + srq_len && ty < HEIGHT; ty ++) {
                    float y0 = map_range(0, HEIGHT, MN_Y_SCALE_MIN, MN_Y_SCALE_MAX, ty);
                    for (uint32_t tx = Px; tx < Px + srq_len && tx < WIDTH; tx ++) {
                        float x0 = map_range(0, WIDTH , MN_X_SCALE_MIN, MN_X_SCALE_MAX, tx);
                        pixels[ty * WIDTH + tx] = get_color(get_iter(x0, y0));
                    }
                }
            }
        }
        }
    }
    return 0;
}

enum {
    kp_w = 1<<0,
    kp_a = 1<<1,
    kp_s = 1<<2,
    kp_d = 1<<3,
    kp_k = 1<<4,
    kp_l = 1<<5,
};

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);
    window  = SDL_CreateWindow("mandelbrot", 0, 0, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    surface = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    pixels  = surface->pixels;
    uint32_t keypress = 0;
    SDL_Event e;
    srq_len = 2;
    init_thrd_pool();
    while (1) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_KEYDOWN: {
                switch (e.key.keysym.scancode) {
                case SDL_SCANCODE_Q:
                    exit(0);
                case SDL_SCANCODE_ESCAPE:
                    exit(0);
                case SDL_SCANCODE_W:
                    keypress |= kp_w;
                    break;
                case SDL_SCANCODE_A:
                    keypress |= kp_a;
                    break;
                case SDL_SCANCODE_S:
                    keypress |= kp_s;
                    break;
                case SDL_SCANCODE_D:
                    keypress |= kp_d;
                    break;
                case SDL_SCANCODE_K:
                    keypress |= kp_k;
                    break;
                case SDL_SCANCODE_L:
                    keypress |= kp_l;
                    break;
                default: break;
                }
            } break;
            case SDL_KEYUP: {
                switch (e.key.keysym.scancode) {
                case SDL_SCANCODE_W:
                    keypress ^= kp_w;
                    break;
                case SDL_SCANCODE_A:
                    keypress ^= kp_a;
                    break;
                case SDL_SCANCODE_S:
                    keypress ^= kp_s;
                    break;
                case SDL_SCANCODE_D:
                    keypress ^= kp_d;
                    break;
                case SDL_SCANCODE_K:
                    keypress ^= kp_k;
                    break;
                case SDL_SCANCODE_L:
                    keypress ^= kp_l;
                    break;
                default: break;
                }
            }break;
            case SDL_QUIT:
                exit(0);
            default: break;
            }
        }

        if (keypress & kp_k) {
            scale *= 0.99;
        } else if (keypress & kp_l) {
            scale *= 1.01;
        } else if (keypress & kp_w) {
            translationY += 0.1 / scale;
        } else if (keypress & kp_a) {
            translationX -= 0.1 / scale;
        } else if (keypress & kp_s) {
            translationY -= 0.1 / scale;
        } else if (keypress & kp_d) {
            translationX += 0.1 / scale;
        }
        if (keypress) {
            turn = 0;
            offset = 0;
        }

        SDL_BlitSurface(surface, NULL, SDL_GetWindowSurface(window), NULL);
        SDL_UpdateWindowSurface(window);

        SDL_Delay(16);
    }
}
