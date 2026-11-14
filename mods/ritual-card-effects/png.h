#ifndef YOUR_TURN_PNG_H
#define YOUR_TURN_PNG_H
#include <stddef.h>
#include <stdint.h>

/* Pixels are 0xAARRGGBB, row by row; free(pixels) when done. */
typedef struct { int width, height; uint32_t *pixels; } PngImage;

/* Nonzero when `data` was a PNG this reader handles. */
int Png_Decode(const uint8_t *data, size_t size, PngImage *image);

#endif
