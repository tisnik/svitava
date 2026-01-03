/*

   Svitava: fractal renderer

   (C) Copyright 2024, 2025, 2026  Pavel Tisnovsky

   All rights reserved. This program and the accompanying materials
   are made available under the terms of the Eclipse Public License v1.0
   which accompanies this distribution, and is available at
   http://www.eclipse.org/legal/epl-v10.html

   Contributors:
       Pavel Tisnovsky

*/

/*
build as shared library:
    gcc -shared -Wl,-soname,svitava -o svitava.so -fPIC svitava.c

build as executable:
    gcc -lm -o svitava svitava.c
*/

#include <stddef.h>
#include <stdlib.h>

/* Image types */
#define GRAYSCALE 1
#define RGB 3
#define RGBA 4

/* Maximum image resolution */
#define MAX_WIDTH 8192
#define MAX_HEIGHT 8192

/**
 * Structure that represents raster image of configurable resolution and bits
 * per pixel format.
 */
typedef struct {
    unsigned int   width;
    unsigned int   height;
    unsigned int   bpp;
    unsigned char *pixels;
} image_t;

/**
 * Compute the total size in bytes of an image's pixel buffer.
 *
 * @param image Pointer to the image whose buffer size will be computed.
 *
 * @returns Total number of bytes required for the image's pixel buffer
 *          (width * height * bpp).
 */
size_t image_size(const image_t *image) {
    if (image == NULL) {
        return 0;
    }
    /* cast to size_t before multiplication to prevent overflow */
    return (size_t)image->width * (size_t)image->height * (size_t)image->bpp;
}

/**
 * Create an image_t with the given width, height, and bytes-per-pixel,
 * allocating a pixel buffer.
 *
 * The returned image_t fields width, height, and bpp are initialized and
 * pixels points to a newly allocated buffer of size width * height * bpp. If
 * allocation fails, pixels will be NULL.
 *
 * @param width  Image width specified in pixels.
 * @param height Image height specified in pixels.
 * @param bpp    Bytes per pixel (bytes used to store a single pixel).
 *
 * @returns The initialized image_t; its `pixels` member points to the
 *          allocated buffer or NULL on allocation failure.
 */
image_t image_create(const unsigned int width, const unsigned int height, const unsigned int bpp) {
    image_t image;
    /* validate parameters */
    if (width == 0 || height == 0 || width > MAX_WIDTH || height > MAX_HEIGHT || (bpp != GRAYSCALE && bpp != RGB && bpp != RGBA)) {
        image.width = 0;
        image.height = 0;
        image.bpp = 0;
        image.pixels = NULL;
        return image;
    }
    image.width = width;
    image.height = height;
    image.bpp = bpp;

    /* callers must check that image.pixels != NULL */
    image.pixels = (unsigned char *)malloc(image_size(&image));

    /* make sure the image will be 'zero value' when pixels are not allocated */
    if (image.pixels == NULL) {
        image.width = 0;
        image.height = 0;
        image.bpp = 0;
    }
    return image;
}

/**
 * Create a new image with the same dimensions and bytes-per-pixel as the given
 * image.
 *
 * If `image` is NULL or its pixel buffer is NULL, returns an image with
 * width=0, height=0, bpp=0 and pixels=NULL.
 *
 * @param image Source image to clone.
 *
 * @returns A newly created image_t with the same width, height, and bpp as
 * `image`; the pixel buffer is separately allocated and may be NULL if
 * allocation fails.
 */
image_t image_clone(const image_t *image) {
    image_t clone;
    if (image == NULL || image->pixels == NULL) {
        clone.width = 0;
        clone.height = 0;
        clone.bpp = 0;
        clone.pixels = NULL;
        return clone;
    }
    clone = image_create(image->width, image->height, image->bpp);
    if (clone.pixels != NULL) {
        memcpy(clone.pixels, image->pixels, image_size(image));
    }
    return clone;
}

