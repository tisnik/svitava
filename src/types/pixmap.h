/* vim: set fileencoding=utf-8 tabstop=8 noexpandtab */

#ifndef _PIXMAP_TYPE_H_
#define _PIXMAP_TYPE_H_

typedef struct
{
	unsigned int width;
	unsigned int height;
	unsigned int bpp;
	unsigned char *pixels;
} Pixmap;

#endif
