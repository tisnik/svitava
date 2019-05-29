#include <stdio.h>
#include <stdlib.h>

unsigned char true_color_tga_header[] =
{
    0x00,       /* without ID */
    0x00,       /* without palette */
    0x03,       /* grayscale */
    0x00, 0x00, /* start of color palette */
    0x00, 0x00, /* length of color palette */
    0x00,       /* bits per palette entry */
    0x00, 0x00,
    0x00, 0x00, /* image coordinates */
    0x00, 0x00, /* image width */
    0x00, 0x00, /* image height */
    0x08,       /* bits per pixel */
    0x20        /* picture orientation */
};

int tga_write(const Bitmap *bitmap, const char *file_name)
{
    FILE *fout;

    fout = fopen(file_name, "wb");
    if (!fout)
    {
        return -1;
    }
    true_color_tga_header[12]=(bitmap->width) & 0xff;
    true_color_tga_header[13]=(bitmap->width) >>8;
    true_color_tga_header[14]=(bitmap->height) & 0xff;
    true_color_tga_header[15]=(bitmap->height) >>8;

    fwrite(true_color_tga_header, sizeof(true_color_tga_header), 1, fout);
    fwrite(bitmap->pixels, bitmap_size(bitmap), 1, fout);

    if (fclose(fout) == EOF)
    {
        return -1;
    }
    return 0;
}

