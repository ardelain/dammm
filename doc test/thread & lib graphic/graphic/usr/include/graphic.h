#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#define G_WIDTH 800
#define G_HEIGHT 600

#define G_NB_COLORS 4

void g_init(void);
void g_close(void);
void g_draw(int x, int y, int colors);
void g_clear(void);
void g_flush(void);
void g_msleep(int);
void g_srandom(void);
int g_random(void);

#endif

