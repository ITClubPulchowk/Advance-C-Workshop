
#include <stdio.h>

void platform_init() {

}

void paltform_render_loop(int framebuffer_w, int framebuffer_h, uint32_t *pixels) {
	printf("\n");

	while (ChunkIndex < MaxChunkCount) {
		float progress = (float)ChunkIndex / (float)MaxChunkCount;
		printf("Progress: %f\r", progress);
		thrd_sleep_millisecs(10);
	}

	printf("\n");

	float time = clock_time(TimeCounts);
	printf("Render Time: %f ms\n", time);
}
