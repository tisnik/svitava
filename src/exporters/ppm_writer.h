#ifndef _PPM_WRITER_H_
#define _PPM_WRITER_H_

#include <stdio.h>
#include "pixmap.h"

/* ASCII PPM with magic number P3 */
int ppm_write_ascii(const Pixmap * pixmap, const char *file_name);
void ppm_write_ascii_to_stream(const Pixmap * pixmap, FILE * fout);

/* ASCII PPM with magic number P6 */
int ppm_write_binary(const Pixmap * pixmap, const char *file_name);
void ppm_write_binary_to_stream(const Pixmap * pixmap, FILE * fout);

#endif
