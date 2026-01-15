#include <assert.h>

#include "svitava.c"

void test_image_create(void) {
    image_t image = image_create(100, 100, 4);
    assert(image.pixels != NULL);
}

int main(void) {
    test_image_create();
    return 0;
}

