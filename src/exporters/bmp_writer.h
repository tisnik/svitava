#ifndef _BMP_WRITER_H_
#define _BMP_WRITER_H_

int bmp_write(const Bitmap *bitmap, const char *file_name);
int bmp_write_to_stream(const Bitmap *bitmap, FILE *fout);

#endif

