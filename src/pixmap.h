/* vim: set fileencoding=utf-8 tabstop=8 noexpandtab */

#ifndef _PIXMAP_H_
#define _PIXMAP_H_

typedef struct
{
	unsigned int width;
	unsigned int height;
	unsigned int bpp;
	unsigned char *pixels;
} Pixmap;

void pixmap_destroy(Pixmap *pixmap);
void pixmap_clear(Pixmap *pixmap);
void pixmap_putpixel(Pixmap *pixmap, int x, int y, unsigned char r, unsigned char g, unsigned char b);
void pixmap_getpixel(const Pixmap *pixmap, int x, int y, unsigned char *r, unsigned char *g, unsigned char *b);
void pixmap_putpixel_max(Pixmap *pixmap, int x, int y, unsigned char r, unsigned char g, unsigned char b);
void pixmap_hline(Pixmap *pixmap, int x1, int x2, int y, unsigned char r, unsigned char g, unsigned char b);
void pixmap_vline(Pixmap *pixmap, int x, int y1, int y2, unsigned char r, unsigned char g, unsigned char b);
void pixmap_line(Pixmap *pixmap, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b);

#endif
