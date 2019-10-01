#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pixmap.h"
#include "exporters/bmp_writer.h"
#include "exporters/tga_writer.h"
/* #include "exporters/png_writer.h" */
#include "exporters/ppm_writer.h"
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
    /*png_write(pixmap, "test.png");*/
    ppm_write_ascii(pixmap, "test.ppm");
    ppm_write_binary(pixmap, "test2.ppm");
    pixmap_destroy(pixmap);
    /*
    pixmap = pixmap_create(255, 255, 3);
    pixmap_clear(pixmap);
    {
        int x, y;
        for (y=0; y<128; y++) {
            for (x=0; x<128; x++) {
                pixmap_putpixel(pixmap, x, y, 255, 0, 0);
            }
        }
    }
    png_write(pixmap, "test1.png");
    bmp_write(pixmap, "test1.bmp");
    pixmap_destroy(pixmap);

    pixmap = pixmap_create(256, 256, 3);
    pixmap_clear(pixmap);
    {
        int x, y;
        for (y=0; y<128; y++) {
            for (x=0; x<128; x++) {
                pixmap_putpixel(pixmap, x, y, 255, 0, 0);
            }
        }
    }
    png_write(pixmap, "test2.png");
    bmp_write(pixmap, "test2.bmp");
    pixmap_destroy(pixmap);

    pixmap = pixmap_create(257, 257, 3);
    pixmap_clear(pixmap);
    {
        int x, y;
        for (y=0; y<128; y++) {
            for (x=0; x<128; x++) {
                pixmap_putpixel(pixmap, x, y, 255, 0, 0);
            }
        }
    }
    png_write(pixmap, "test3.png");
    bmp_write(pixmap, "test3.bmp");
    pixmap_destroy(pixmap);
    */
    return 0;
}

