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

/*
Overall structure:
------------------

Raster image filters implemented:
---------------------------------
filter_smooth_3x3_block
filter_smooth_3x3_gauss
filter_sharpen_3x3
filter_edge_detection_3x3_1
filter_edge_detection_3x3_2
filter_edge_detection_3x3_3
filter_horizontal_edge_detection_3x3
filter_vertical_edge_detection_3x3
filter_horizontal_sobel_operator_3x3
filter_vertical_sobel_operator_3x3
*/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* Image types */
#define GRAYSCALE 1
#define RGB 3
#define RGBA 4

/* Maximum image resolution */
#define MAX_WIDTH 8192
#define MAX_HEIGHT 8192

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

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

/**
 * Clear all pixel data in an image by setting every byte in the pixel buffer
 * to zero (regardless of image type).
 *
 * @param image Image whose pixel buffer will be cleared; must have a valid
 *              pixel buffer.
 *
 * @returns none
 */
void image_clear(image_t *image) {
    if (image == NULL || image->pixels == NULL) {
        return;
    }
    memset(image->pixels, 0x00, image_size(image));
}

/**
 * Set the RGBA color of the pixel at the specified (x, y) coordinates in the image.
 *
 * If (x, y) lies outside the image bounds the function has no effect.
 *
 * @param image Pointer to the image whose pixel will be updated.
 * @param x Horizontal pixel coordinate (0 is left).
 * @param y Vertical pixel coordinate (0 is top).
 * @param r Red component (0–255).
 * @param g Green component (0–255).
 * @param b Blue component (0–255).
 * @param a Alpha component (0–255).
 *
 * @returns none
 */
void image_putpixel(image_t *image, int x, int y, unsigned char r,
                    unsigned char g, unsigned char b, unsigned char a) {
    unsigned char *p;
    if (image == NULL || image->pixels == NULL) {
        return;
    }
    if (x < 0 || y < 0 || x >= (int)image->width || y >= (int)image->height) {
        return;
    }
    p = image->pixels + (x + y * image->width) * image->bpp;
    if (image->bpp == GRAYSCALE) {
        /* convert to grayscale using integer approximation of standard weights */
        /* uses integer arithmetic with coefficients scaled by 256 (77≈0.299×256, 150≈0.587×256, 29≈0.114×256) */
        *p = (unsigned char)((77 * r + 150 * g + 29 * b) >> 8);
    } else {
        *p++ = r;
        *p++ = g;
        *p++ = b;
        if (image->bpp == RGBA) {
            *p = a;
        }
    }
}

/**
 * Update the pixel at (x, y) by replacing each color channel with the greater of
 * the existing channel and the provided value; the alpha channel is written
 * unconditionally.
 *
 * If (x, y) is outside the image bounds, the function does nothing.
 *
 * @param image Target image.
 * @param x X coordinate of the pixel.
 * @param y Y coordinate of the pixel.
 * @param r Red component candidate; pixel's red becomes `max(current, r)`.
 * @param g Green component candidate; pixel's green becomes `max(current, g)`.
 * @param b Blue component candidate; pixel's blue becomes `max(current, b)`.
 * @param a Alpha component to write (overwrites existing alpha).
 *
 * @returns none
 */
void image_putpixel_max(image_t *image, int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    unsigned char *p;
    if (image == NULL || image->pixels == NULL) {
        return;
    }
    if (x < 0 || y < 0 || x >= (int)image->width || y >= (int)image->height) {
        return;
    }
    p = image->pixels + (x + y * image->width) * image->bpp;
    if (image->bpp == GRAYSCALE) {
        /* convert to grayscale using integer approximation of standard weights */
        /* uses integer arithmetic with coefficients scaled by 256 (77≈0.299×256, 150≈0.587×256, 29≈0.114×256) */
        unsigned char gray = (unsigned char)((77 * r + 150 * g + 29 * b) >> 8);
        if (*p < gray) {
            *p = gray;
        }
        return;
    }
    /* image type is RGB or RGBA */
    if (*p < r) {
        *p = r;
    }
    p++;
    if (*p < g) {
        *p = g;
    }
    p++;
    if (*p < b) {
        *p = b;
    }
    if (image->bpp == RGBA) {
        p++;
        *p = a;
    }
}

/**
 * Retrieve the RGBA components of the pixel at (x, y).
 *
 * If (x, y) is outside the image bounds the function returns without modifying
 * the output pointers. The output pointers must be non-NULL when (x, y) is
 * inside bounds.
 *
 * @param image Pointer to the source image.
 * @param x X coordinate of the pixel.
 * @param y Y coordinate of the pixel.
 * @param r Pointer to receive the red component (0–255).
 * @param g Pointer to receive the green component (0–255).
 * @param b Pointer to receive the blue component (0–255).
 * @param a Pointer to receive the alpha component (0–255).
 */
void image_getpixel(const image_t *image, int x, int y, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a) {
    const unsigned char *p;
    if (image == NULL || image->pixels == NULL || r == NULL || g == NULL || b == NULL || a == NULL) {
        return;
    }
    if (x < 0 || y < 0 || x >= (int)image->width || y >= (int)image->height) {
        return;
    }
    p = image->pixels + (x + y * image->width) * image->bpp;

    if (image->bpp == GRAYSCALE) {
        /* for grayscale, replicate the single value to all RGB channels */
        *r = *g = *b = *p;
        *a = 255; /* grayscale images are always opaque */
        return;
    }

    *r = *p++;
    *g = *p++;
    *b = *p++;
    if (image->bpp == RGBA) {
        *a = *p;
    } else {
        *a = 255; /* default opaque for non-RGBA images */
    }
}

/**
 * Draws a horizontal line between two x coordinates at a given y using the specified RGBA color.
 *
 * The line includes both endpoints; the order of `x1` and `x2` does not matter. Pixels that lie
 * outside the image bounds are ignored.
 *
 * @param image Target image to draw into.
 * @param x1 One end x coordinate of the line.
 * @param x2 Other end x coordinate of the line.
 * @param y Y coordinate of the line.
 * @param r Red component (0–255).
 * @param g Green component (0–255).
 * @param b Blue component (0–255).
 * @param a Alpha component (0–255).
 *
 * @returns none
 */
void image_hline(image_t *image, int x1, int x2, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    int x, fromX = MIN(x1, x2), toX = MAX(x1, x2);
    for (x = fromX; x <= toX; x++) {
        image_putpixel(image, x, y, r, g, b, a);
    }
}

/**
 * Draws a vertical line at column x between y1 and y2 inclusive using the specified RGBA color.
 *
 * @param image Target image.
 * @param x X coordinate (column) where the line is drawn.
 * @param y1 One end Y coordinate of the line.
 * @param y2 Other end Y coordinate of the line.
 * @param r Red component (0-255).
 * @param g Green component (0-255).
 * @param b Blue component (0-255).
 * @param a Alpha component (0-255).
 *
 * @returns none
 */
void image_vline(image_t *image, int x, int y1, int y2, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    int y, fromY = MIN(y1, y2), toY = MAX(y1, y2);
    for (y = fromY; y <= toY; y++) {
        image_putpixel(image, x, y, r, g, b, a);
    }
}

/**
 * Draws a straight line between two pixel coordinates using an integer rasterization algorithm.
 *
 * The line includes both endpoint pixels and writes the specified RGBA color to each covered pixel.
 * Pixels that lie outside the image bounds are ignored.
 *
 * @param image Target image to draw into.
 * @param x1 X coordinate of the start point (in pixels).
 * @param y1 Y coordinate of the start point (in pixels).
 * @param x2 X coordinate of the end point (in pixels).
 * @param y2 Y coordinate of the end point (in pixels).
 * @param r Red component of the color (0-255).
 * @param g Green component of the color (0-255).
 * @param b Blue component of the color (0-255).
 * @param a Alpha component of the color (0-255).
 *
 * @returns none
 */
void image_line(image_t *image, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    while (1) {
        image_putpixel(image, x1, y1, r, g, b, a);
        if (x1 == x2 && y1 == y2) {
            break;
        }
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y1 += sy;
        }
    }
}

/**
 * Draws an anti-aliased straight line between two points into an RGBA image.
 *
 * The line is rasterized with sub-pixel intensity distribution so adjacent pixels
 * receive proportionally scaled color components for smoothing. Color components
 * are applied using the image's per-pixel maximum blending semantics (brightest
 * component wins); alpha is written as provided.
 *
 * @param image Target image buffer (RGBA) to draw into.
 * @param x1 X coordinate of the line start.
 * @param y1 Y coordinate of the line start.
 * @param x2 X coordinate of the line end.
 * @param y2 Y coordinate of the line end.
 * @param r Red component (0–255) of the line color.
 * @param g Green component (0–255) of the line color.
 * @param b Blue component (0–255) of the line color.
 * @param a Alpha component (0–255) of the line color.
 *
 * @returns none
 */
void image_line_aa(image_t *image, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    int    dx = x2 - x1;
    int    dy = y2 - y1;
    double s, p, e = 0.0;
    int    x, y, xdelta, ydelta, xpdelta, ypdelta, xp, yp;
    int    i, imin, imax;

    /* anti-aliasing requires RGBA for proper blending */
    if (image == NULL || image->pixels == NULL || image->bpp != RGBA) {
        return;
    }

    /* strict vertical line does not have to be anti-aliased */
    if (x1 == x2) {
        image_vline(image, x1, y1, y2, r, g, b, a);
        return;
    }

    /* strict horizontal line does not have to be anti-aliased */
    if (y1 == y2) {
        image_hline(image, x1, x2, y1, r, g, b, a);
        return;
    }

    if (x1 > x2) {
        x1 = x1 ^ x2;
        x2 = x1 ^ x2;
        x1 = x1 ^ x2;
        y1 = y1 ^ y2;
        y2 = y1 ^ y2;
        y1 = y1 ^ y2;
    }

    /* iterate along the dominant (longer) axis */
    if (abs(dx) > abs(dy)) {
        s = (double)dy / (double)dx;  /* slope: rise over run */
        imin = x1;
        imax = x2;
        x = x1;
        y = y1;
        xdelta = 1;
        ydelta = 0;
        xpdelta = 0;
        xp = 0;
        if (y2 > y1) {
            ypdelta = 1;
            yp = 1;
        } else {
            s = -s;
            ypdelta = -1;
            yp = -1;
        }
    } else {
        s = (double)dx / (double)dy;  /* slope: run over rise */
        xdelta = 0;
        ydelta = 1;
        ypdelta = 0;
        yp = 0;
        if (y2 > y1) {
            imin = y1;
            imax = y2;
            x = x1;
            y = y1;
            xpdelta = 1;
            xp = 1;
        } else {
            s = -s;
            imin = y2;
            imax = y1;
            x = x2;
            y = y2;
            xpdelta = -1;
            xp = -1;
        }
    }
    /* p: sub-pixel step scaled to [0, 256) range for intensity calculation */
    p = s * 256.0;
    for (i = imin; i <= imax; i++) {
        int c1 = (int)e;
        if (c1 > 255) c1 = 255;
        int c2 = 255 - c1;
        image_putpixel_max(image, x + xp, y + yp, (r * c1) / 255, (g * c1) / 255, (b * c1) / 255, a);
        image_putpixel_max(image, x, y, (r * c2) / 255, (g * c2) / 255, (b * c2) / 255, a);
        e = e + p;
        x += xdelta;
        y += ydelta;
        if (e >= 256.0) {
            e -= 256.0;
            x += xpdelta;
            y += ypdelta;
        }
    }
}

/**
 * Apply a convolution kernel to the image, producing a filtered version in-place.
 *
 * Applies the provided size×size integer kernel to each pixel inside the image
 * (excluding a border of floor(size/2) pixels). For each processed pixel the
 * weighted sums of the R, G, B channels are computed, divided by `divisor`, and
 * written back into the image buffer; the alpha channel of written pixels is
 * set to 255 (fully opaque). Border pixels that cannot be fully covered by the
 * kernel are left unchanged.
 *
 * @param image   Image to be filtered; its pixel buffer is updated with the result.
 * @param size    Kernel dimension; must match both kernel array dimensions and be an odd positive integer.
 * @param kernel  2D integer kernel of dimensions [size][size]; kernel[row][col] is applied around each pixel.
 * @param divisor Value used to normalize the accumulated channel sums; must be non-zero.
 */
void apply_kernel(image_t *image, int size, int kernel[size][size], int divisor) {
    int x, y;
    image_t tmp;
    int limit = size/2;

    if (image == NULL || image->pixels == NULL) {
        return;
    }
    /* size must be odd positive number */
    if (size <= 0 || size % 2 == 0 || divisor == 0) {
        return;
    }

    tmp = image_clone(image);
    if (tmp.pixels == NULL) {
        return; /* allocation failed */
    }

    for (y=limit; y<(int)tmp.height-limit; y++) {
        for (x=limit; x<(int)tmp.width-limit; x++) {
            int r=0, g=0, b=0;
            int dx, dy;
            for (dy=-limit; dy<=limit; dy++) {
                for (dx=-limit; dx<=limit; dx++) {
                    unsigned char rr, gg, bb, aa;
                    image_getpixel(image, x+dx, y+dy, &rr, &gg, &bb, &aa);
                    r+=rr*kernel[dy+limit][dx+limit];
                    g+=gg*kernel[dy+limit][dx+limit];
                    b+=bb*kernel[dy+limit][dx+limit];
                }
            }
            r/=divisor;
            g/=divisor;
            b/=divisor;
            /* clamp to valid unsigned char range */
            r = (r < 0) ? 0 : (r > 255 ? 255 : r);
            g = (g < 0) ? 0 : (g > 255 ? 255 : g);
            b = (b < 0) ? 0 : (b > 255 ? 255 : b);
            image_putpixel(&tmp, x, y, r, g, b, 255);
        }
    }
    memcpy(image->pixels, tmp.pixels, image_size(image));
    free(tmp.pixels);
}

/**
 * Apply a 3×3 weighted smoothing filter to the given image in-place.
 *
 * Uses a 3×3 kernel with weights:
 *   [ 1, 1, 1 ]
 *   [ 1, 1, 1 ]
 *   [ 1, 1, 1 ]
 * and a divisor of 9 to perform a weighted average of each pixel's neighbourhood.
 *
 * @param image Image to be filtered; the pixel buffer is modified in-place. If `image` or its pixel buffer is NULL, no action is taken.
 */
void filter_smooth_3x3_block(image_t *image) {
    static int kernel[3][3] = {
        {1,1,1},
        {1,1,1},
        {1,1,1},
    };

    apply_kernel(image, 3, kernel, 9);
}

/**
 * Apply a 3×3 Gaussian-like smoothing filter to the provided image in-place.
 *
 * Uses the 3×3 kernel with weights:
 *   [ 1, 2, 1 ]
 *   [ 2, 4, 2 ]
 *   [ 1, 2, 1 ]
 * and a divisor of 16 to perform smoothing.
 *
 * @param image Image to be filtered; the pixel buffer is modified in-place. If `image` or its pixel buffer is NULL, no action is taken.
 */
void filter_smooth_3x3_gauss(image_t *image) {
    static int kernel[3][3] = {
        {1,2,1},
        {2,4,2},
        {1,2,1},
    };

    apply_kernel(image, 3, kernel, 16);
}

/**
 * Apply a 3×3 sharpening filter to the image in place.
 *
 * Uses the 3×3 kernel with weights:
 *   [ 0, -1,  0 ]
 *   [-1,  5, -1 ]
 *   [ 0, -1,  0 ]
 *
 * @param image Image to be filtered; the pixel buffer is modified in-place. If `image` or its pixel buffer is NULL, no action is taken.
 */
void filter_sharpen_3x3(image_t *image) {
    static int kernel[3][3] = {
        { 0,-1, 0},
        {-1, 5,-1},
        { 0,-1, 0},
    };

    apply_kernel(image, 3, kernel, 1);
}

/**
 * Apply a 3×3 edge-detection filter (4-neighbor Laplacian kernel) to the image in-place.
 *
 * This filter highlights regions of rapid intensity change (edges) by computing the
 * second derivative approximation. Edges appear as bright pixels; negative values
 * are clamped to zero.
 *
 * The kernel applied is:
 *   [ 0, -1,  0 ]
 *   [-1,  4, -1 ]
 *   [ 0, -1,  0 ]
 *
 * @param image Image to be filtered; the pixel buffer is modified in-place. If `image` or its pixel buffer is NULL, no action is taken.
 */
void filter_edge_detection_3x3_1(image_t *image) {
    static int kernel[3][3] = {
        { 0,-1, 0},
        {-1, 4,-1},
        { 0,-1, 0},
    };

    apply_kernel(image, 3, kernel, 1);
}

/**
 * Apply a 3×3 edge-detection filter (8-neighbor Laplacian kernel) to the image in-place.
 *
 * This filter highlights regions of rapid intensity change (edges) using all eight
 * neighboring pixels. Edges appear as bright pixels; negative values are clamped to zero.
 *
 * The kernel applied is:
 *   [-1, -1, -1 ]
 *   [-1,  8, -1 ]
 *   [-1, -1, -1 ]
 *
 * @param image Image to be filtered; the pixel buffer is modified in-place. If `image` or its pixel buffer is NULL, no action is taken.
 */
void filter_edge_detection_3x3_2(image_t *image) {
    static int kernel[3][3] = {
        {-1,-1,-1},
        {-1, 8,-1},
        {-1,-1,-1},
    };

    apply_kernel(image, 3, kernel, 1);
}

/**
 * Apply a 3×3 Laplacian-like edge-detection filter to the provided image in-place.
 *
 * This filter uses the inverted polarity of filter_edge_detection_3x3_1, highlighting
 * edges where the center pixel is darker than its neighbors. Edges appear as bright
 * pixels; negative values are clamped to zero.
 *
 * The kernel applied is:
 *   [ 0,  1,  0 ]
 *   [ 1, -4,  1 ]
 *   [ 0,  1,  0 ]
 *
 * @param image Image to be filtered; the pixel buffer is modified in-place. If `image` or its pixel buffer is NULL, no action is taken.
 */
void filter_edge_detection_3x3_3(image_t *image) {
    static int kernel[3][3] = {
        { 0, 1, 0},
        { 1,-4, 1},
        { 0, 1, 0},
    };

    apply_kernel(image, 3, kernel, 1);
}

/**
 * Apply a 3×3 horizontal edge-detection filter to an image in-place.
 *
 * The kernel applied is:
 *   [-1, -1, -1]
 *   [ 0,  0,  0]
 *   [ 1,  1,  1]
 *
 * @param image Image to be filtered; the pixel buffer is modified in-place. If `image` or its pixel buffer is NULL, no action is taken.
 */
void filter_horizontal_edge_detection_3x3(image_t *image) {
    static int kernel[3][3] = {
        {-1,-1,-1},
        { 0, 0, 0},
        { 1, 1, 1},
    };

    apply_kernel(image, 3, kernel, 1);
}

/**
 * Apply a 3×3 vertical edge-detection filter to the image in-place.
 *
 * The filter highlights vertical edges by convolving the image with a 3×3
 * vertical edge-detection kernel.
 *
 * The kernel applied is:
 *   [-1,  0,  1]
 *   [-1,  0,  1]
 *   [-1,  0,  1]
 *
 * @param image Image to be filtered; the pixel buffer is modified in-place. If `image` or its pixel buffer is NULL, no action is taken.
 */
void filter_vertical_edge_detection_3x3(image_t *image) {
    static int kernel[3][3] = {
        {-1, 0, 1},
        {-1, 0, 1},
        {-1, 0, 1},
    };

    apply_kernel(image, 3, kernel, 1);
}

/**
 * Apply the 3×3 horizontal Sobel operator to the given image, modifying pixels in-place.
 *
 * The kernel applied is:
 *   [-1,  0,  1]
 *   [-2,  0,  2]
 *   [-1,  0,  1]
 *
 * @param image Image to be filtered; the pixel buffer is modified in-place. If `image` or its pixel buffer is NULL, no action is taken.
 */
void filter_horizontal_sobel_operator_3x3(image_t *image) {
    static int kernel[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1},
    };

    apply_kernel(image, 3, kernel, 1);
}

/**
 * Apply the 3×3 vertical Sobel operator to the given image, modifying pixels in-place.
 *
 * The kernel applied is:
 *   [-1, -2, -1]
 *   [ 0,  0,  0]
 *   [ 1,  2,  1]
 *
 * @param image Image to be filtered; the pixel buffer is modified in-place. If `image` or its pixel buffer is NULL, no action is taken.
 */
void filter_vertical_sobel_operator_3x3(image_t *image) {
    static int kernel[3][3] = {
        {-1,-2,-1},
        { 0, 0, 0},
        { 1, 2, 1},
    };

    apply_kernel(image, 3, kernel, 1);
}

/**
 * Validate that three images are suitable for composition operations.
 * Returns 1 if valid, 0 otherwise.
 */
static int validate_composition_inputs(const image_t *src1, const image_t *src2, const image_t *dest) {
    /* validate inputs */
    if (!src1 || !src2 || !dest) {
        return 0;
    }

    /* pixel buffers must exist */
    if (!src1->pixels || !src2->pixels || !dest->pixels) {
        return 0;
    }

    /* validate supported formats */
    if ((src1->bpp != GRAYSCALE && src1->bpp != RGB && src1->bpp != RGBA) ||
        (src2->bpp != GRAYSCALE && src2->bpp != RGB && src2->bpp != RGBA) ||
        (dest->bpp != GRAYSCALE && dest->bpp != RGB && dest->bpp != RGBA)) {
        return 0;
    }

    /* ensure all images have the same dimensions */
    if (src1->width != src2->width || src1->height != src2->height ||
        src1->width != dest->width || src1->height != dest->height) {
        return 0;
    }

    return 1;
}

/*
 * Interleave pixels from two source images into a destination image using a horizontal pattern.
 *
 * For each pixel position (x,y), selects the pixel from `src1` when x is odd and from `src2` when x is even, then writes that RGBA pixel into `dest`.
 *
 * @param src1 Source image providing pixels for odd columns; must have the same dimensions as `src2` and `dest`.
 * @param src2 Source image providing pixels for even columns; must have the same dimensions as `src1` and `dest`.
 * @param dest Destination image receiving the interleaved pixels; must have the same dimensions as `src1` and `src2`.
 */
void composite_horizontal_interlace(const image_t *src1, const image_t *src2, image_t *dest) {
    unsigned int i, j;

    if (!validate_composition_inputs(src1, src2, dest)) {
        return;
    }

    for (j = 0; j < src1->height; j++) {
        for (i = 0; i < src1->width; i++) {
            unsigned char r, g, b, a;
            int           which = i % 2;
            if (which) {
                image_getpixel(src1, i, j, &r, &g, &b, &a);
            } else {
                image_getpixel(src2, i, j, &r, &g, &b, &a);
            }
            image_putpixel(dest, i, j, r, g, b, a);
        }
    }
}

/**
 * Interleave pixels from two source images into a destination image using a vertical pattern.
 *
 * For each pixel position, pixels on odd-numbered rows are taken from `src1`
 * and pixels on even-numbered rows are taken from `src2`; the selected RGBA
 * values are written into `dest` at the same coordinates.
 *
 * @param src1 Source image supplying pixels for odd rows.
 * @param src2 Source image supplying pixels for even rows.
 * @param dest Destination image receiving the interleaved pixels.
 */
void composite_vertical_interlace(const image_t *src1, const image_t *src2, image_t *dest) {
    unsigned int i, j;

    if (!validate_composition_inputs(src1, src2, dest)) {
        return;
    }

    for (j = 0; j < src1->height; j++) {
        for (i = 0; i < src1->width; i++) {
            unsigned char r, g, b, a;
            int           which = j % 2;
            if (which) {
                image_getpixel(src1, i, j, &r, &g, &b, &a);
            } else {
                image_getpixel(src2, i, j, &r, &g, &b, &a);
            }
            image_putpixel(dest, i, j, r, g, b, a);
        }
    }
}

/**
 * Compose a destination image by selecting pixels from two sources in a checkerboard pattern.
 *
 * For each coordinate (i, j), the destination receives the pixel from `src1` when (i + j) is odd;
 * otherwise the pixel is taken from `src2`.
 *
 * @param src1 Source image providing pixels for one set of checkerboard positions.
 * @param src2 Source image providing pixels for the alternating checkerboard positions.
 * @param dest Destination image where the composed pixels are written. Must have the same dimensions
 *             as `src1` and `src2`.
 */
void composite_checkberboard_interlace(const image_t *src1, const image_t *src2, image_t *dest) {
    unsigned int i, j;

    if (!validate_composition_inputs(src1, src2, dest)) {
        return;
    }

    for (j = 0; j < src1->height; j++) {
        for (i = 0; i < src1->width; i++) {
            unsigned char r, g, b, a;
            int           which = (i % 2) ^ (j % 2);
            if (which) {
                image_getpixel(src1, i, j, &r, &g, &b, &a);
            } else {
                image_getpixel(src2, i, j, &r, &g, &b, &a);
            }
            image_putpixel(dest, i, j, r, g, b, a);
        }
    }
}

/**
 * Blend two source images into a destination by averaging corresponding RGBA channels.
 *
 * Each destination pixel is written with the per-channel average of the two source pixels:
 * channel = (channel_src1 + channel_src2) >> 1 (integer division by 2).
 *
 * @param src1 First source image; its width and height determine the processed area.
 * @param src2 Second source image; pixels are read at the same coordinates as src1.
 * @param dest Destination image that will be written with the blended pixels.
 */
void composite_blend(const image_t *src1, const image_t *src2, image_t *dest) {
    unsigned int i, j;

    if (!validate_composition_inputs(src1, src2, dest)) {
        return;
    }

    for (j = 0; j < src1->height; j++) {
        for (i = 0; i < src1->width; i++) {
            unsigned char r1, g1, b1, a1;
            unsigned char r2, g2, b2, a2;
            image_getpixel(src1, i, j, &r1, &g1, &b1, &a1);
            image_getpixel(src2, i, j, &r2, &g2, &b2, &a2);
            image_putpixel(dest, i, j, (r1 + r2) >> 1, (g1 + g2) >> 1, (b1 + b2) >> 1, (a1 + a2) >> 1);
        }
    }
}
