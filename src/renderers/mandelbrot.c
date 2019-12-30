#include <stdlib.h>
#include <math.h>

#include "pixmap.h"
#include "mandelbrot.h"

void calculate_mandelbrot(Pixmap *pixmap, long maxiter) {
    double cy = -1.5;
    int y;
    for (y=0; y<pixmap->height; y++) {
        double cx = -2.0;
        int x;
        for (x=0; x<pixmap->width; x++) {
            double zx = 0.0;
            double zy = 0.0;
            unsigned int i = 0;
            while (i < maxiter) {
                double zx2 = zx * zx;
                double zy2 = zy * zy;
                zx = -fabs(zx);
                if (zx2 + zy2 > 4.0) {
                    break;
                }
                zy = 2.0 * zx * zy + cy;
                zx = zx2 - zy2 + cx;
                i++;
            }
            {
                int r = 3*i;
                int g = 3*i;
                int b = 3*i;

                pixmap_putpixel(pixmap, x, y, r, g, b);
            }
            cx += 3.0/pixmap->width;
        }
        cy += 3.0/pixmap->height;
    }
}

void calculate_mandelbrot_(Pixmap *pixmap, long maxiter) {
    double cy = -1.5;
    int y;
    for (y=0; y<pixmap->height; y++) {
        double cx = -2.0;
        int x;
        for (x=0; x<pixmap->width; x++) {
            double zx = 0.0;
            double zy = 0.0;
            unsigned int i = 0;
            while (i < maxiter) {
                double zx2 = zx * zx;
                double zy2 = zy * zy;
                if (zx2 + zy2 > 4.0) {
                    break;
                }
                zy = 2.0 * zx * zy + cy;
                zx = zx2 - zy2 + cx;
                i++;
            }
            {
                int r = (int)(zx-cx)*10;
                int g = (int)(zy-cy)*10;
                int b = 0;

                r = r & 0xff;
                g = g & 0xff;

                pixmap_putpixel(pixmap, x, y, r, g, b);
            }
            cx += 3.0/pixmap->width;
        }
        cy += 3.0/pixmap->height;
    }
}

