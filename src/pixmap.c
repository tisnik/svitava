#include <stdlib.h>
#include <string.h>

#include "macros.h"
#include "pixmap.h"

void pixmap_destroy(Pixmap *pixmap)
{
    free(pixmap->pixels);
    free(pixmap);
}

