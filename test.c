#include <assert.h>

#include "svitava.c"

#define TEST_BEGIN \
    puts(__FUNCTION__); \
    {

#define TEST_END \
    }

void test_image_size_null_image(void) {
    TEST_BEGIN
    size_t size = image_size(NULL);
    assert(size == 0);
    TEST_END

}

void test_image_create_zero_width(void) {
    TEST_BEGIN
    image_t image = image_create(0, 100, 4);
    assert(image.width == 0);
    assert(image.height == 0);
    assert(image.bpp == 0);
    assert(image.pixels == NULL);
    TEST_END
}

void test_image_create_too_wide(void) {
    TEST_BEGIN
    image_t image = image_create(MAX_WIDTH+1, 100, 4);
    assert(image.width == 0);
    assert(image.height == 0);
    assert(image.bpp == 0);
    assert(image.pixels == NULL);
    TEST_END
}

void test_image_create_zero_height(void) {
    TEST_BEGIN
    image_t image = image_create(100, 0, 4);
    assert(image.width == 0);
    assert(image.height == 0);
    assert(image.bpp == 0);
    assert(image.pixels == NULL);
    TEST_END
}

void test_image_create_too_high(void) {
    TEST_BEGIN
    image_t image = image_create(100, MAX_HEIGHT+1, 4);
    assert(image.width == 0);
    assert(image.height == 0);
    assert(image.bpp == 0);
    assert(image.pixels == NULL);
    TEST_END
}

void test_image_create_wrong_image_type(void) {
    TEST_BEGIN
    image_t image = image_create(100, 100, 0);
    assert(image.width == 0);
    assert(image.height == 0);
    assert(image.bpp == 0);
    assert(image.pixels == NULL);
    TEST_END
}

void test_image_create_grayscale(void) {
    TEST_BEGIN
    image_t image = image_create(100, 100, GRAYSCALE);
    assert(image.pixels != NULL);
    free(image.pixels);
    TEST_END
}

void test_image_create_rgb(void) {
    TEST_BEGIN
    image_t image = image_create(100, 100, RGB);
    assert(image.pixels != NULL);
    free(image.pixels);
    TEST_END
}

void test_image_create_rgba(void) {
    TEST_BEGIN
    image_t image = image_create(100, 100, RGBA);
    assert(image.pixels != NULL);
    free(image.pixels);
    TEST_END
}

void test_image_clone_null_image(void) {
    TEST_BEGIN
    image_t cloned = image_clone(NULL);
    assert(cloned.width == 0);
    assert(cloned.height == 0);
    assert(cloned.bpp == 0);
    assert(cloned.pixels == NULL);
    TEST_END
}

void test_image_clone_image_without_pixels(void) {
    TEST_BEGIN
    image_t image, cloned;

    image.width = 100;
    image.height = 100;
    image.bpp = 1;
    image.pixels = NULL;

    cloned = image_clone(&image);
    assert(cloned.width == 0);
    assert(cloned.height == 0);
    assert(cloned.bpp == 0);
    assert(cloned.pixels == NULL);
    TEST_END
}

void test_image_clone_proper_image(void) {
    TEST_BEGIN
    image_t image, cloned;

    image = image_create(100, 100, RGB);
    assert(image.pixels != NULL);

    cloned = image_clone(&image);
    assert(cloned.width == 100);
    assert(cloned.height == 100);
    assert(cloned.bpp == RGB);
    assert(cloned.pixels != NULL);

    free(image.pixels);
    free(cloned.pixels);
    TEST_END
}

void test_image_clone_large_image(void) {
    TEST_BEGIN
    image_t image, cloned;

    image = image_create(100, 100, RGB);
    image.width = MAX_WIDTH+1;
    image.height = MAX_HEIGHT+1;
    assert(image.pixels != NULL);

    cloned = image_clone(&image);
    assert(cloned.width == 0);
    assert(cloned.height == 0);
    assert(cloned.bpp == 0);
    assert(cloned.pixels == NULL);

    free(image.pixels);
    TEST_END
}

void test_image_clear_null_image(void) {
    TEST_BEGIN
    int result = image_clear(NULL);
    assert(result == NULL_IMAGE_POINTER);
    TEST_END
}

void test_image_clear_image_without_pixels(void) {
    TEST_BEGIN
    image_t image;
    int result;

    image.width = 100;
    image.height = 100;
    image.bpp = 1;
    image.pixels = NULL;

    result = image_clear(&image);
    assert(result==NULL_PIXELS_POINTER);
    TEST_END
}

void test_image_clear_proper_image(void) {
    TEST_BEGIN
    image_t image;
    int result;
    int i;

    image = image_create(100, 100, GRAYSCALE);
    assert(image.pixels != NULL);

    result = image_clear(&image);
    assert(result==OK);

    for (i=0; i<100*100; i++) {
        assert(image.pixels[i] == 0);
    }

    free(image.pixels);
    TEST_END
}

void test_image_putpixel_null_image(void) {
    TEST_BEGIN
    int result = image_putpixel(NULL, 0, 0, 0, 0, 0, 0);
    assert(result == NULL_IMAGE_POINTER);
    TEST_END
}

void test_image_putpixel_image_without_pixels(void) {
    TEST_BEGIN
    image_t image;
    int result;

    image.width = 100;
    image.height = 100;
    image.bpp = 1;
    image.pixels = NULL;

    result = image_putpixel(&image, 0, 0, 0, 0, 0, 0);
    assert(result==NULL_PIXELS_POINTER);
    TEST_END
}

void test_image_putpixel_negative_coordinates(void) {
    TEST_BEGIN
    image_t image;
    int result;

    image = image_create(100, 100, GRAYSCALE);
    assert(image.pixels != NULL);

    result = image_putpixel(&image, -1, 0, 0, 0, 0, 0);
    assert(result==INVALID_COORDINATES);

    result = image_putpixel(&image, 0, -1, 0, 0, 0, 0);
    assert(result==INVALID_COORDINATES);

    free(image.pixels);
    TEST_END
}

void test_image_putpixel_coordinates_outside_range(void) {
    TEST_BEGIN
    image_t image;
    int result;

    image = image_create(100, 100, GRAYSCALE);
    assert(image.pixels != NULL);

    result = image_putpixel(&image, 100+1, 1, 0, 0, 0, 0);
    assert(result==INVALID_COORDINATES);

    result = image_putpixel(&image, 1, 100+1, 0, 0, 0, 0);
    assert(result==INVALID_COORDINATES);

    free(image.pixels);
    TEST_END
}

void test_image_putpixel_rgb_image(void) {
    TEST_BEGIN
    image_t image;
    unsigned char expected[3] = {1, 2, 3};
    int result;

    image = image_create(1, 1, RGB);
    assert(image.pixels != NULL);

    result = image_putpixel(&image, 0, 0, 1, 2, 3, 4);
    assert(result==OK);
    assert(memcmp((void*)expected, (void*)image.pixels, 3)==0);

    free(image.pixels);
    TEST_END
}

void test_image_putpixel_rgba_image(void) {
    TEST_BEGIN
    image_t image;
    unsigned char expected[4] = {1, 2, 3, 4};
    int result;

    image = image_create(1, 1, RGBA);
    assert(image.pixels != NULL);

    result = image_putpixel(&image, 0, 0, 1, 2, 3, 4);
    assert(result==OK);
    assert(memcmp((void*)expected, (void*)image.pixels, 4)==0);

    free(image.pixels);
    TEST_END
}

void test_image_putpixel_grayscale_image(void) {
    TEST_BEGIN
    image_t image;
    int result;

    image = image_create(1, 1, GRAYSCALE);
    assert(image.pixels != NULL);

    result = image_putpixel(&image, 0, 0, 0, 0, 0, 40);
    assert(result==OK);
    assert(image.pixels[0]==0);

    result = image_putpixel(&image, 0, 0, 10, 20, 30, 40);
    assert(result==OK);
    assert(image.pixels[0]==18);

    free(image.pixels);
    TEST_END
}

void test_image_putpixel_max_null_image(void) {
    TEST_BEGIN
    int result = image_putpixel_max(NULL, 0, 0, 0, 0, 0, 0);
    assert(result == NULL_IMAGE_POINTER);
    TEST_END
}

void test_image_putpixel_max_image_without_pixels(void) {
    TEST_BEGIN
    image_t image;
    int result;

    image.width = 100;
    image.height = 100;
    image.bpp = 1;
    image.pixels = NULL;

    result = image_putpixel_max(&image, 0, 0, 0, 0, 0, 0);
    assert(result==NULL_PIXELS_POINTER);
    TEST_END
}

void test_image_putpixel_max_negative_coordinates(void) {
    TEST_BEGIN
    image_t image;
    int result;

    image = image_create(100, 100, GRAYSCALE);
    assert(image.pixels != NULL);

    result = image_putpixel_max(&image, -1, 0, 0, 0, 0, 0);
    assert(result==INVALID_COORDINATES);

    result = image_putpixel_max(&image, 0, -1, 0, 0, 0, 0);
    assert(result==INVALID_COORDINATES);

    free(image.pixels);
    TEST_END
}

void test_image_putpixel_max_coordinates_outside_range(void) {
    TEST_BEGIN
    image_t image;
    int result;

    image = image_create(100, 100, GRAYSCALE);
    assert(image.pixels != NULL);

    result = image_putpixel_max(&image, 100+1, 1, 0, 0, 0, 0);
    assert(result==INVALID_COORDINATES);

    result = image_putpixel_max(&image, 1, 100+1, 0, 0, 0, 0);
    assert(result==INVALID_COORDINATES);

    free(image.pixels);
    TEST_END
}

void test_image_putpixel_max_rgb_image(void) {
    TEST_BEGIN
    image_t image;
    unsigned char expected[3] = {1, 2, 3};
    int result;

    image = image_create(1, 1, RGB);
    assert(image.pixels != NULL);
    image_clear(&image);

    result = image_putpixel_max(&image, 0, 0, 1, 2, 3, 4);
    assert(result==OK);
    assert(memcmp((void*)expected, (void*)image.pixels, 3)==0);

    result = image_putpixel_max(&image, 0, 0, 0, 0, 0, 4);
    assert(result==OK);
    assert(memcmp((void*)expected, (void*)image.pixels, 3)==0);

    free(image.pixels);
    TEST_END
}

void test_image_putpixel_max_rgba_image(void) {
    TEST_BEGIN
    image_t image;
    unsigned char expected[4] = {1, 2, 3, 4};
    int result;

    image = image_create(1, 1, RGBA);
    assert(image.pixels != NULL);
    image_clear(&image);

    result = image_putpixel_max(&image, 0, 0, 1, 2, 3, 4);
    assert(result==OK);
    assert(memcmp((void*)expected, (void*)image.pixels, 4)==0);

    free(image.pixels);
    TEST_END
}

void test_image_putpixel_max_grayscale_image(void) {
    TEST_BEGIN
    image_t image;
    int result;

    image = image_create(1, 1, GRAYSCALE);
    assert(image.pixels != NULL);
    image_clear(&image);

    result = image_putpixel_max(&image, 0, 0, 0, 0, 0, 40);
    assert(result==OK);
    assert(image.pixels[0]==0);

    result = image_putpixel_max(&image, 0, 0, 10, 20, 30, 40);
    assert(result==OK);
    assert(image.pixels[0]==18);

    free(image.pixels);
    TEST_END
}

int main(void) {
    /* tests for function image_size() */
    test_image_size_null_image();

    /* tests for function image_create() */
    test_image_create_zero_width();
    test_image_create_too_wide();
    test_image_create_zero_height();
    test_image_create_too_high();
    test_image_create_wrong_image_type();
    test_image_create_grayscale();
    test_image_create_rgb();
    test_image_create_rgba();

    /* tests for function image_clone() */
    test_image_clone_null_image();
    test_image_clone_image_without_pixels();
    test_image_clone_proper_image();
    test_image_clone_large_image();

    /* tests for function image_clear() */
    test_image_clear_null_image();
    test_image_clear_image_without_pixels();
    test_image_clear_proper_image();

    /* tests for function image_putpixel */
    test_image_putpixel_null_image();
    test_image_putpixel_image_without_pixels();
    test_image_putpixel_negative_coordinates();
    test_image_putpixel_coordinates_outside_range();
    test_image_putpixel_rgb_image();
    test_image_putpixel_rgba_image();
    test_image_putpixel_grayscale_image();

    /* tests for function image_putpixel_max */
    test_image_putpixel_max_null_image();
    test_image_putpixel_max_image_without_pixels();
    test_image_putpixel_max_negative_coordinates();
    test_image_putpixel_max_coordinates_outside_range();
    test_image_putpixel_max_rgb_image();
    test_image_putpixel_max_rgba_image();
    test_image_putpixel_max_grayscale_image();
    return 0;
}

