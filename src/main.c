#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pixmap.h"
#include "exporters/bmp_writer.h"
#include "exporters/tga_writer.h"
#include "renderers/mandelbrot.h"


typedef struct
{
    int bitmap_width;
    int bitmap_height;
} config;

int process_arguments(int argc, char **argv)
{
    return 1;
}

int main(int argc, char **argv)
{
    Pixmap *pixmap;

    pixmap = pixmap_create(320, 240, 3);
    pixmap_clear(pixmap);
    calculate_mandelbrot(pixmap, 256);
    bmp_write(pixmap, "test.bmp");
    pixmap_destroy(pixmap);

    return 0;
}

