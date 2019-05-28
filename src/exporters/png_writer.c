#include <stdio.h>
#include <stdlib.h>
#include <png.h>

#include "bitmap.h"

int png_write(const Bitmap *bitmap, const char *file_name)
{
    int code = 0;
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep row;
    char *title=NULL;

    /* Open file for writing (binary mode) */
    fp = fopen(file_name, "wb");
    if (fp == NULL)
    {
        fprintf(stderr, "Could not open file %s for writing\n", file_name);
        code = 1;
        goto FINALISE;
    }

    /* Initialize write structure */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
    {
        fprintf(stderr, "Could not allocate write struct\n");
        code = 1;
        goto FINALISE;
    }

    /* Initialize info structure */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fprintf(stderr, "Could not allocate info struct\n");
        code = 1;
        goto FINALISE;
    }

    /* Setup Exception handling */
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        fprintf(stderr, "Error during png creation\n");
        code = 1;
        goto FINALISE;
    }

    png_init_io(png_ptr, fp);

    /* Write header (8 bit colour depth) */
    png_set_IHDR(png_ptr, info_ptr, bitmap->width, bitmap->height,
            8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    /* Set title */
    if (title != NULL)
    {
        png_text title_text;
        title_text.compression = PNG_TEXT_COMPRESSION_NONE;
        title_text.key = "Title";
        title_text.text = title;
        png_set_text(png_ptr, info_ptr, &title_text, 1);
    }

    png_write_info(png_ptr, info_ptr);

    /* Write image data */
    int scanline;
    row = bitmap->pixels;
    for (scanline=0 ; scanline<bitmap->height; scanline++)
    {
        png_write_row(png_ptr, row);
        row += bitmap->bpp;
    }

    /* End write */
    png_write_end(png_ptr, NULL);

FINALISE:
    if (fp != NULL)
    {
        fclose(fp);
    }
    if (info_ptr != NULL)
    {
        png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    }
    if (png_ptr != NULL)
    {
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    }

    return code;
}

