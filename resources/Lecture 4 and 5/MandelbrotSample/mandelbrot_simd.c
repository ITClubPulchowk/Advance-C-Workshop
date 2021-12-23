#include "threading.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <immintrin.h>

#pragma pack(push, 1)
typedef struct BMP_Header {
	uint16_t type;
	uint32_t size;
	uint16_t reserved[2];
	uint32_t off_bits;

	struct {
		uint32_t size;
		int32_t  width;
		int32_t  height;
		uint16_t planes;
		uint16_t bit_count;
		uint32_t compression;
		uint32_t size_image;
		int32_t  x_pixels_per_meter;
		int32_t  y_pixels_per_meter;
		uint32_t clr_used;
		uint32_t clr_important;
	} info;
} BMP_Header;
#pragma pack(pop)

static int bmp_write_file(const char *filename, int32_t width, int32_t height, uint32_t *pixels) {
	BMP_Header header;
	memset(&header, 0, sizeof(header));

	header.type = 0x4d42;
	header.size = sizeof(header) + (sizeof(uint32_t) * width * height);
	header.off_bits = sizeof(header);

	header.info.size = sizeof(header.info);
	header.info.width = width;
	header.info.height = -height;
	header.info.planes = 1;
	header.info.bit_count = 32;

	FILE *out = fopen(filename, "wb");
	if (out) {
		fwrite(&header, sizeof(header), 1, out);
		fwrite(pixels, sizeof(uint32_t) * width * height, 1, out);
		fclose(out);
		return 1;
	}

	return 0;
}

static inline __m128 map_range(__m128 in_a, __m128 in_b, __m128 out_a, __m128 out_b, __m128 v) {
    __m128 fac = _mm_div_ps(_mm_sub_ps(out_b, out_a), _mm_sub_ps(in_b, in_a));
    __m128 mul = _mm_sub_ps(v, in_a);
    __m128 res = _mm_mul_ps(fac, mul);
    res = _mm_add_ps(res, out_a);
    return res;
}

static inline float lerp(float from, float to, float t) {
	return (1 - t) * from + t * to;
}

typedef struct {
	float r, g, b;
} Color;

uint32_t ColorToUint32(Color v) {
	uint8_t r = (uint8_t)(255.0f * v.r);
	uint8_t g = (uint8_t)(255.0f * v.g);
	uint8_t b = (uint8_t)(255.0f * v.b);
	return r << 16 | g << 8 | b;
}

Color Uint32ToColor(uint32_t c) {
	float div = 1.0f / 255.0f;
	float r = (float)((c >> 16) & 0xff) * div;
	float g = (float)((c >> 8) & 0xff) * div;
	float b = (float)(c & 0xff) * div;
	return (Color) { r, g, b };
}

static Color Palette[] = {
	{ 0.258824, 0.117647, 0.058824 },
	{ 0.098039, 0.027451, 0.101961 },
	{ 0.035294, 0.003922, 0.184314 },
	{ 0.015686, 0.015686, 0.286275 },
	{ 0.000000, 0.027451, 0.392157 },
	{ 0.047059, 0.172549, 0.541176 },
	{ 0.094118, 0.321569, 0.694118 },
	{ 0.223529, 0.490196, 0.819608 },
	{ 0.525490, 0.709804, 0.898039 },
	{ 0.827451, 0.925490, 0.972549 },
	{ 0.945098, 0.913726, 0.749020 },
	{ 0.972549, 0.788235, 0.372549 },
	{ 1.000000, 0.666667, 0.000000 },
	{ 0.800000, 0.501961, 0.000000 },
	{ 0.600000, 0.341176, 0.000000 },
	{ 0.415686, 0.203922, 0.011765 },
};

typedef struct {
    uint32_t m[4];
} MM_Color;

MM_Color mandelbrot_color(__m128i iterations, __m128i max_iterations, __m128 x, __m128 y, int max_iters) {
    int mask = _mm_movemask_epi8(_mm_cmplt_epi32(iterations, max_iterations));
    
    int masks[] = { 0x1, 0x10, 0x100, 0x1000 };

    int iters[4];
    _mm_storeu_si128((void *)iters, iterations);

    MM_Color colors;
    for (int i = 0; i < 4; ++i) {
        if (mask & masks[i]) {
            int iter = iters[i];

            Color color_a = Palette[iter & 15];
		    Color color_b = Palette[(iter + 1) & 15];
		    float t = (float)iter / (float)max_iters;
		
		    Color color;
		    color.r = lerp(color_a.r, color_b.r, t);
		    color.g = lerp(color_a.g, color_b.g, t);
		    color.b = lerp(color_a.b, color_b.b, t);

		    colors.m[i] = ColorToUint32(color);
        }
        else {
            colors.m[i] = 0x00;
        }
    }

    return colors;
}

void platform_init();
void paltform_render_loop(int framebuffer_w, int framebuffer_h, uint32_t *pixels);

typedef struct {
	uint32_t max_iterations;
	float rect_min[2];
	float rect_max[2];

	int width, height;
	uint32_t *pixels;
} Mandelbrot_Spec;

static Mandelbrot_Spec MandelbrotSpec;
static uint32_t ChunkSize;
static uint32_t MaxChunkX;
static uint32_t MaxChunkY;
static uint32_t MaxChunkCount;

static volatile uint32_t ChunkIndex;
static volatile uint32_t TimeCounts;

#define Minimum(a, b) (((a) < (b)) ? (a) : (b))

int mandelbrot_draw(void *arg) {
	const uint32_t MaxIters = MandelbrotSpec.max_iterations;

	float RectMin[2];
	float RectMax[2];

	RectMin[0] = MandelbrotSpec.rect_min[0];
	RectMin[1] = MandelbrotSpec.rect_min[1];
	RectMax[0] = MandelbrotSpec.rect_max[0];
	RectMax[1] = MandelbrotSpec.rect_max[1];

	const uint32_t Width  = (uint32_t)MandelbrotSpec.width;
	const uint32_t Height = (uint32_t)MandelbrotSpec.height;
	uint32_t *Pixels = MandelbrotSpec.pixels;

	uint32_t chunk_index = 0;

	uint64_t start = clock_counts_now();

    const __m128i MaxItersVec = _mm_set1_epi32((int)MaxIters);

    const __m128 ZeroVec   = _mm_set_ps1(0.0f);
    const __m128 WidthVec  = _mm_set_ps1((float)Width);
    const __m128 HeightVec = _mm_set_ps1((float)Height);

    __m128 RectMinVec[2];
    __m128 RectMaxVec[2];

    RectMinVec[0] = _mm_set_ps1(RectMin[0]);
    RectMinVec[1] = _mm_set_ps1(RectMin[1]);
    RectMaxVec[0] = _mm_set_ps1(RectMax[0]);
    RectMaxVec[1] = _mm_set_ps1(RectMax[1]);

	while (1) {
		chunk_index = interlocked_inc(&ChunkIndex) - 1;

		if (chunk_index < MaxChunkCount) {
			uint32_t x_chunk_index = chunk_index % MaxChunkX;
			uint32_t y_chunk_index = chunk_index / MaxChunkX;

			uint32_t render_px = x_chunk_index * ChunkSize;
			uint32_t render_py = y_chunk_index * ChunkSize;

			uint32_t render_max_px = Minimum(render_px + ChunkSize, Width);
			uint32_t render_max_py = Minimum(render_py + ChunkSize, Height);

			for (uint32_t py = render_py; py < render_max_py; ++py) {
                __m128 py_vec = _mm_set_ps1((float)py);
                __m128 oy = map_range(ZeroVec, HeightVec, RectMinVec[1], RectMaxVec[1], py_vec);

				for (uint32_t px = render_px; px < render_max_px; px += 4) {
                    __m128 px_vec = _mm_set_ps((float)px + 3, (float)px + 2, (float)px + 1, (float)px + 0);
                    __m128 ox = map_range(ZeroVec, WidthVec, RectMinVec[0], RectMaxVec[0], px_vec);

                    __m128 x = ZeroVec;
                    __m128 y = ZeroVec;
                    __m128 x2 = ZeroVec;
                    __m128 y2 = ZeroVec;

                    __m128 radius = _mm_set_ps1(4.0f);

                    __m128i iteration = _mm_set1_epi32(0);

                    int mask = 0xf;

                    for (int iter = 0; iter < MaxIters; ++iter) {
                        mask = mask & _mm_movemask_ps(_mm_sub_ps(_mm_add_ps(x2, y2), radius));
                        if (mask == 0) break;

                        y = _mm_add_ps(_mm_mul_ps(_mm_set_ps1(2.0f), _mm_mul_ps(x, y)), oy);
                        x = _mm_add_ps(_mm_sub_ps(x2, y2), ox);
                        x2 = _mm_mul_ps(x, x);
                        y2 = _mm_mul_ps(y, y);

                        int a = (mask & 0x8) >> 3;
                        int b = (mask & 0x4) >> 2;
                        int c = (mask & 0x2) >> 1;
                        int d = mask & 0x1;

                        __m128i increment = _mm_set_epi32(a, b, c, d);

                        iteration = _mm_add_epi32(iteration, increment);
                    }

					MM_Color color = mandelbrot_color(iteration, MaxItersVec, x2, y2, MaxIters);

					Pixels[py * Width + px + 0] = color.m[0];
                    Pixels[py * Width + px + 1] = color.m[1];
                    Pixels[py * Width + px + 2] = color.m[2];
                    Pixels[py * Width + px + 3] = color.m[3];
				}
			}
		}
		else {
			break;
		}
	}

	uint64_t end = clock_counts_now();

	interlocked_add(&TimeCounts, end - start);

	return 0;
}

int main(int argc, char *argv[]) {
	platform_init();

	MandelbrotSpec.max_iterations = 10000;
	MandelbrotSpec.rect_min[0] = -2.5f;
	MandelbrotSpec.rect_min[1] = -2.0f;
	MandelbrotSpec.rect_max[0] = 1.0f;
	MandelbrotSpec.rect_max[1] = 2.0f;

	MandelbrotSpec.width  = 1280;
	MandelbrotSpec.height = 720;
	MandelbrotSpec.pixels = malloc(sizeof(uint32_t) * MandelbrotSpec.width * MandelbrotSpec.height);

	ChunkSize = 16;
	MaxChunkX = MandelbrotSpec.width / ChunkSize;
	MaxChunkY = MandelbrotSpec.height / ChunkSize;
	MaxChunkCount = MaxChunkX * MaxChunkY;

	int thread_count = 0;
	if (argc == 2) {
		thread_count = atoi(argv[1]);
	}

	if (thread_count <= 0) {
		thread_count = 16;
	}

	printf("Running with %d threads.\n", thread_count);

	for (uint32_t index = 0; index < thread_count; ++index) {
		thrd_t thread;
		thrd_create(&thread, mandelbrot_draw, NULL);
	}

	paltform_render_loop(MandelbrotSpec.width, MandelbrotSpec.height, MandelbrotSpec.pixels);

	bmp_write_file("output.bmp", MandelbrotSpec.width, MandelbrotSpec.height, MandelbrotSpec.pixels);

	return 0;
}

#if defined(__gnu_linux__) || defined(__linux__) || defined(linux) || defined(__linux)
#include "linux_window.c"
#elif defined(_WIN64) || defined(_WIN32)
#include "windows_window.c"
#endif
