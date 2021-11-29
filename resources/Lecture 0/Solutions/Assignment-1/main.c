#include <stdio.h>
#include <stdlib.h>

typedef struct {
	unsigned int* data;
	unsigned int width;
	unsigned int height;
} Image;

void save_to_ppm(Image* image, const char* path) {
	FILE* file = fopen(path, "wb");
	if(file) {
		fprintf(file, "P6\n%d %d 255\n", image->width, image->height);
		for(int y = 0; y < image->height; ++y) {
			for(int x = 0; x < image->width; ++x) {
				unsigned char r = (image->data[y * image->width + x] >> (8 * 3)) & 0xff;
				unsigned char g = (image->data[y * image->width + x] >> (8 * 2)) & 0xff;
				unsigned char b = (image->data[y * image->width + x] >> (8 * 1)) & 0xff;
				fprintf(file, "%c%c%c", r, g, b);
			}
		}
		fclose(file);
	}
	
}

void draw_rect(Image* image, int x0, int y0, int w, int h, unsigned int color) {
	for(int y = y0 - h / 2; y <= y0 + h / 2; ++y) {
		for(int x = x0 - w / 2; x <= x0 + w / 2; ++x) {
			if(x >= 0 && x< image->width && y >= 0 && y < image->height)
				image->data[y * image->width + x] = color;
		}
	}
}

void draw_circle(Image* image, int cx, int cy, int r, unsigned int color) {
	for(int y = cy - r; y <= cy + r; ++y) {
		for(int x = cx - r; x <= cx + r; ++x) {
			if((x - cx)* (x - cx) + (y - cy) * (y - cy) <= r * r) {
				if(x >= 0 && x< image->width && y >= 0 && y < image->height)
					image->data[y * image->width + x] = color;
			}
		}
	}
}

void clear_screen(Image* image, unsigned int color) {
	for(int y = 0; y < image->height; ++y) {
		for(int x = 0; x < image->width; ++x) {
			image->data[y * image->width + x] = color;
		}
	}
	
}

int is_digit(char c) {
	return c >= '0' && c <= '9';
}

int parse_int(char** ptr) {
	while(!is_digit(**ptr)) (*ptr)++;
	int result = atoi(*ptr);
	while(is_digit(**ptr)) (*ptr)++;
	return result;
}

//caller's responsibility to free after using
char* read_file(const char* path) {
	FILE* file = fopen(path, "rb");
	char* result = 0;
	if(file) {
		unsigned int size = 0;
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		result = malloc(size + 1);
		fseek(file, 0, SEEK_SET);
		if(result) {
			fread(result, size, 1, file);
			result[size] = 0;
		}
		fclose(file);
	}
	return result;
}

int main() {
	Image image = {0};
	clear_screen(&image, 0x181818ff);
	
	char* content = read_file("render.txt");
	if(!content) return -1;
	char* row_start = content;
	unsigned int offset = 0;
	
	image.width = parse_int(&row_start);
	image.height = parse_int(&row_start);
	
	while(*row_start++ != '\n');
	
	image.data = malloc(image.width * image.height * sizeof(*image.data));
	
	while(*row_start) {
		char* row_ptr = row_start;
		while(*row_ptr != '\n' && *row_ptr != 0) row_ptr++;
		if(*row_ptr != 0) *row_ptr++ = 0;
		offset = row_ptr - row_start;
		
		row_ptr = row_start;
		
		switch(row_start[0]) {
			case 'r': {
				int cx = parse_int(&row_ptr);
				int cy = parse_int(&row_ptr);
				int w = parse_int(&row_ptr);
				int h = parse_int(&row_ptr);
				unsigned char r = parse_int(&row_ptr);
				unsigned char g = parse_int(&row_ptr);
				unsigned char b = parse_int(&row_ptr);
				
				draw_rect(&image, cx, cy, w, h, (r << 24 | g << 16 | b << 8 | 0xff));
			} break;
			
			case 'c': {
				int cx = parse_int(&row_ptr);
				int cy = parse_int(&row_ptr);
				int rad = parse_int(&row_ptr);
				unsigned char r = parse_int(&row_ptr);
				unsigned char g = parse_int(&row_ptr);
				unsigned char b = parse_int(&row_ptr);
				
				draw_circle(&image, cx, cy, rad, (r << 24 | g << 16 | b << 8 | 0xff));
			} break;
			
		}
		row_start += offset;
	}
	
	save_to_ppm(&image, "output.ppm");
	free(image.data);
	free(content);
	return 0;
}