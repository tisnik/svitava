#include <stdio.h>
#include <assert.h>

#include "pixmap.h"
#include "ppm_writer.h"

void ppm_write_ascii_to_stream(const Pixmap *pixmap, FILE *fout)
{
    int x, y;
    unsigned char r, g, b;

    /* header */
    fprintf(fout, "P3 %d %d 255\n", pixmap->width, pixmap->height);

    /* pixel array */
    for (y=pixmap->height-1; y>=0; y--) {
        for (x=0; x<pixmap->width; x++) {
            pixmap_getpixel(pixmap, x, y, &r, &g, &b);
            fprintf(fout, "%d %d %d\n", r, g, b);
        }
    }
}

void ppm_write_binary_to_stream(const Pixmap *pixmap, FILE *fout)
{
    int x, y;
    unsigned char r, g, b;

    /* header */
    fprintf(fout, "P6 %d %d 255\n", pixmap->width, pixmap->height);

    /* pixel array */
    for (y=pixmap->height-1; y>=0; y--) {
        for (x=0; x<pixmap->width; x++) {
            pixmap_getpixel(pixmap, x, y, &r, &g, &b);
            fputc(r, fout);
            fputc(g, fout);
            fputc(b, fout);
        }
    }
}

int ppm_write_ascii(const Pixmap *pixmap, const char *file_name)
{
    FILE *fout;

    fout = fopen(file_name, "wb");
    if (!fout)
    {
        return -1;
    }

    ppm_write_ascii_to_stream(pixmap, fout);

    if (fclose(fout) == EOF)
    {
        return -1;
    }
    return 0;
}

int ppm_write_binary(const Pixmap *pixmap, const char *file_name)
{
    FILE *fout;

    fout = fopen(file_name, "wb");
    if (!fout)
    {
        return -1;
    }

    ppm_write_binary_to_stream(pixmap, fout);

    if (fclose(fout) == EOF)
    {
        return -1;
    }
    return 0;
}
