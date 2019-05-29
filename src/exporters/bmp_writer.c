#include <stdio.h>
#include <stdlib.h>

#include "bmp_writer.h"

unsigned char bmp_header[] =
{
    0x42, 0x4d,             /* magic number */
    0x46, 0x00, 0x00, 0x00, /* size of header=70 bytes */
    0x00, 0x00,             /* unused */
    0x00, 0x00,             /* unused */
    0x36, 0x00, 0x00, 0x00, /* 54 bytes - offset to data */
    0x28, 0x00, 0x00, 0x00, /* 40 bytes - bytes in DIB header */
    0x00, 0x00, 0x00, 0x00, /* width of bitmap */
    0x00, 0x00, 0x00, 0x00, /* height of bitmap */
    0x01, 0x00,             /* 1 pixel plane */
    0x00, 0x00,             /* 24 bpp */
    0x00, 0x00, 0x00, 0x00, /* no compression */
    0x00, 0x00, 0x00, 0x00, /* size of pixel array */
    0x13, 0x0b, 0x00, 0x00, /* 2835 pixels/meter */
    0x13, 0x0b, 0x00, 0x00, /* 2835 pixels/meter */
    0x00, 0x00, 0x00, 0x00, /* color palette */
    0x00, 0x00, 0x00, 0x00, /* important colors */
};

int bmp_write_to_stream(const Pixmap *pixmap, FILE *fout)
{
    int width, height, bpp;

    width = pixmap -> width;
    height = pixmap -> height;
    bpp = pixmap -> bpp * 8;

    bmp_header[18] =  width & 0xff;
    bmp_header[19] = (width >> 8) & 0xff;
    bmp_header[20] = (width >> 16) & 0xff;
    bmp_header[21] = (width >> 24) & 0xff;
    bmp_header[22] =  height & 0xff;
    bmp_header[23] = (height >> 8) & 0xff;
    bmp_header[24] = (height >> 16) & 0xff;
    bmp_header[25] = (height >> 24) & 0xff;
    bmp_header[28] = bpp;

    fwrite(bmp_header, sizeof(bmp_header), 1, fout);
    fwrite(pixmap->pixels, pixmap_size(pixmap), 1, fout);
    return 1;
}

int bmp_write(const Pixmap *pixmap, const char *file_name)
{
    FILE *fout;

    fout = fopen(file_name, "wb");
    if (!fout)
    {
        return -1;
    }
    bmp_write_to_stream(pixmap, fout);

    if (fclose(fout) == EOF)
    {
        return -1;
    }
    return 0;
}

