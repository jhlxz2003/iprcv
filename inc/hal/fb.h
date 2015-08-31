#ifndef _DAV_FB_H
#define _DAV_FB_H

#include "utypes.h"
#include "hal/rgb.h"

#ifdef __cplusplus
extern "C" {
#endif

/* called by dav_init() in dav_init.c */
int      dav_fb_init         ();

void    dav_fb_trans_on     (int mode);
void    dav_fb_trans_off    ();

void    dav_fb_trans_op_draw(int x, int y, int w, int h, int bx, int by, UImage* bg, UImage* fg);
void    dav_fb_trans_op_map(int x, int y, int w, int h, UImage* bg, UImage* fg);
void    dav_fb_trans_op_color(int x, int y, int w, int h, COLOR color, UImage* fg);
void    dav_fb_trans_draw_bf(int x, int y, PUImage image);
void    dav_fb_trans_draw(int x, int y, PUImage image);
void    dav_fb_get_scr(int x, int y, int w, int h, PUImage d);
void    dav_fb_put_pixel(int x, int y, COLOR color);
COLOR   dav_fb_get_pixel(int x, int y);
void     dav_fb_fill_rect(int x, int y, int w, int h, COLOR col);
void     dav_fb_draw_partial(int dstx, int dsty, int w, int h, int srcx, int srcy, PUImage image);
void     dav_fb_draw(int x, int y, PUImage image);
void     dav_fb_scr_clr(int x, int y, int w, int h);
void     dav_fb_bf_clr(int x, int y, int w, int h);
void     dav_fb_map_partial(int x, int y, int w, int h, PUImage image);
void     dav_fb_backup_to_bf();
void     dav_fb_backup_area(int x, int y, int w, int h);
void     dav_fb_backup_bf(UImage *d);
void     dav_fb_put_pixel_bf(int x, int y, COLOR color);
void     dav_fb_fill_rect_bf(int x, int y, int w, int h, COLOR color);
void     dav_fb_draw_partial_bf(int dstx, int dsty, int w, int h, int srcx, int srcy, PUImage image);
void     dav_fb_draw_bf(int x, int y, PUImage image);
void     dav_fb_show();
void     dav_fb_lock();
void     dav_fb_unlock();

#ifdef __cplusplus
}
#endif

#endif

