/* vim: set fileencoding=utf-8 tabstop=8 noexpandtab */

#ifndef _pixmap_type_h_
#define _pixmap_type_h_

typedef struct
{
	unsigned int width;
	unsigned int height;
	unsigned int bpp;
	unsigned char *pixels;
} Pixmap;

#endif
