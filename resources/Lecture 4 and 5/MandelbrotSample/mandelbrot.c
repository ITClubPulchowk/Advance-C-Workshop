#include "threading.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

#include <math.h>

static inline float map_range(float in_a, float in_b, float out_a, float out_b, float v) {
	return (out_b - out_a) / (in_b - in_a) * (v - in_a) + out_a;
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

uint32_t mandelbrot_color(uint32_t iterations, uint32_t max_iterations, float x, float y) {
	if (iterations < max_iterations) {
		Color color_a = Palette[iterations & 15];
		Color color_b = Palette[(iterations + 1) & 15];
		float t = (float)iterations / (float)max_iterations;
		
		Color color;
		color.r = lerp(color_a.r, color_b.r, t);
		color.g = lerp(color_a.g, color_b.g, t);
		color.b = lerp(color_a.b, color_b.b, t);

		return ColorToUint32(color);
	}
	return 0;
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
				float oy = map_range(0, Height, RectMin[1], RectMax[1], py);

				for (uint32_t px = render_px; px < render_max_px; ++px) {
					float ox = map_range(0, Width, RectMin[0], RectMax[0], px);

					float x = 0, y = 0, x2 = 0, y2 = 0;
					int iteration = 0;
					while (x2 + y2 <= 4.0f && iteration < MaxIters) {
						y = 2 * x * y + oy;
						x = x2 - y2 + ox;
						x2 = x * x;
						y2 = y * y;
						iteration += 1;
					}

					uint32_t color = mandelbrot_color(iteration, MaxIters, x2, y2);
					Pixels[py * Width + px] = color;
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
