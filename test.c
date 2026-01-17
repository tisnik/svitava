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

int main(void) {
    test_image_size_null_image();
    test_image_create_zero_width();
    test_image_create_too_wide();
    test_image_create_zero_height();
    test_image_create_too_high();
    test_image_create_wrong_image_type();
    test_image_create_grayscale();
    test_image_create_rgb();
    test_image_create_rgba();
    test_image_clone_null_image();
    test_image_clone_image_without_pixels();
    test_image_clone_proper_image();
    test_image_clone_large_image();
    return 0;
}

