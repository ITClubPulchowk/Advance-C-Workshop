#include "mdlbrt_cmn.h"
#include "thread_pool.h"
#include <SDL2/SDL.h>

SDL_Window *window;
SDL_Surface *bruh;
uint32_t *canvas;

float fOffsetX = 0, fOffsetY = 0;
float fScale  = 1;
void world_to_screen(float fwx, float fwy, float *nsx, float *nsy) {
    *nsx = ((fwx - fOffsetX) * fScale);
    *nsy = ((fwy - fOffsetY) * fScale);
}

void screen_to_world(float nsx, float nsy, float *fwx, float *fwy) {
    *fwx = (nsx) / fScale + fOffsetX;
    *fwy = (nsy) / fScale + fOffsetY;
}

uint32_t srq_len;
volatile uint32_t offset;
volatile uint32_t turn;

uint32_t get_iter(float x0, float y0) {
    float nx0, ny0;
    world_to_screen(x0, y0, &nx0, &ny0);

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
        uint32_t offs = interlocked_add(&offset, srq_len) - srq_len;

        float y0 = map_range(0, IM_HEIGHT, MN_Y_SCALE_MIN, MN_Y_SCALE_MAX, offs);
        float y1 = map_range(0, IM_HEIGHT, MN_Y_SCALE_MIN, MN_Y_SCALE_MAX, offs + srq_len);

        for(uint32_t Px = 0; Px < IM_WIDTH; Px += srq_len) {

            float x0 = map_range(0, IM_WIDTH , MN_X_SCALE_MIN, MN_X_SCALE_MAX, Px);
            float x1 = map_range(0, IM_WIDTH , MN_X_SCALE_MIN, MN_X_SCALE_MAX, Px + srq_len);

            uint32_t iter0 = get_iter(x0, y0);
            uint32_t iter1 = get_iter(x1, y0);
            uint32_t iter2 = get_iter(x1, y1);
            uint32_t iter3 = get_iter(x0, y1);

            if (iter0 == iter1 && iter0 == iter2 && iter0 == iter3) {
                uint32_t color = get_color(iter0);
                for (uint32_t ty = offs; ty < offs + srq_len && ty < IM_HEIGHT; ty ++) {
                    for (uint32_t tx = Px; tx < Px + srq_len && tx < IM_WIDTH; tx ++) {
                        canvas[ty * IM_WIDTH + tx] = color;
                    }
                }
            } else {
                for (uint32_t ty = offs; ty < offs + srq_len && ty < IM_HEIGHT; ty ++) {
                    float y0 = map_range(0, IM_HEIGHT, MN_Y_SCALE_MIN, MN_Y_SCALE_MAX, ty);
                    for (uint32_t tx = Px; tx < Px + srq_len && tx < IM_WIDTH; tx ++) {
                        float x0 = map_range(0, IM_WIDTH , MN_X_SCALE_MIN, MN_X_SCALE_MAX, tx);
                        canvas[ty * IM_WIDTH + tx] = get_color(get_iter(x0, y0));
                    }
                }
            }
        }
    }
    return 0;
}

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("mandelbrot", 0, 0, IM_WIDTH, IM_HEIGHT, SDL_WINDOW_SHOWN);
    bruh   = SDL_CreateRGBSurface(0, IM_WIDTH, IM_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    canvas = bruh->pixels;

    int mX, mY;
    float mouseX, mouseY;
    float startPanX, startPanY;
    float prevX, prevY;
    int bHeld = 0;
    int aHeld = 0;
    int sHeld = 0;
    float beforeZoomX, beforeZoomY;
    float afterZoomX, afterZoomY;

    float minX, minY, maxX, maxY, nmouseX, nmouseY;
    SDL_Event e;
    srq_len = init_thrd_pool(0);
    while (1) {

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_KEYDOWN: {
                    switch (e.key.keysym.scancode) {
                        case SDL_SCANCODE_Q:
                            exit(0);
                        case SDL_SCANCODE_ESCAPE:
                            exit(0);
                        case SDL_SCANCODE_A:
                    aHeld = 1;
                            break;
                        case SDL_SCANCODE_S:
                    sHeld = 1;
                            break;
                        default: break;
                    }
                } break;
                case SDL_KEYUP: {
                    switch (e.key.keysym.scancode) {
                        case SDL_SCANCODE_A:
                            aHeld = 0;
                            break;
                        case SDL_SCANCODE_S:
                            sHeld = 0;
                            break;
                        default: break;
                    }
                }break;
                case SDL_MOUSEBUTTONDOWN: {
                    if (!bHeld) {
                        startPanX = mouseX;
                        startPanY = mouseY;
                    }
                    bHeld = 1;
                } break;
                case SDL_MOUSEBUTTONUP: {
                    bHeld = 0;
                } break;
                case SDL_QUIT:
                    exit(0);
                default: break;
            }
        }

        SDL_GetMouseState(&mX, &mY);
        mouseX = map_range(0, IM_WIDTH , MN_X_SCALE_MIN, MN_X_SCALE_MAX, mX);
        mouseY = map_range(0, IM_HEIGHT, MN_Y_SCALE_MIN, MN_Y_SCALE_MAX, mY);
        world_to_screen(mouseX, mouseY, &nmouseX, &nmouseY);
        screen_to_world(nmouseX, nmouseY, &beforeZoomX, &beforeZoomY);

        if (sHeld) {
            fScale *= 0.5;
            turn = 0;
            offset = 0;
        } else if (aHeld) {
            fScale *= 2;
            turn = 0;
            offset = 0;
        }

        screen_to_world(nmouseX, nmouseY, &afterZoomX, &afterZoomY);
        fOffsetX += (beforeZoomX - afterZoomX);
        fOffsetY += (beforeZoomY - afterZoomY);

        if (bHeld) {
            float bx, by;
            world_to_screen(startPanX, startPanY, &bx, &by);
            fOffsetX += (nmouseX - bx) / fScale;
            fOffsetY += (nmouseY - by) / fScale;

            startPanX = mouseX;
            startPanY = mouseY;
            turn = 0;
            offset = 0;
        }

        SDL_BlitSurface(bruh, NULL, SDL_GetWindowSurface(window), NULL);
        SDL_UpdateWindowSurface(window);

        SDL_Delay(16);
    }
}
