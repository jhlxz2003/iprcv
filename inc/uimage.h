#ifndef U_IMAGE_H_
#define U_IMAGE_H_

#include "utypes.h"
#include "hal/rgb.h"

unsigned char *u_image_buffer_init();
void  u_image_buffer_finish();
void  u_image_buffer_clear();
void  u_rgb888_rgb565(unsigned char *dst, unsigned char *src, int w, int h);
void  u_rgba888_rgba565(unsigned char *dst, unsigned char *src, int w, int h);
void  u_rgb888_rgb565x(unsigned char *dst, unsigned char *src, int dw, int dh, int sw, int sh);
void  u_rgba888_rgba565x(unsigned char *dst, unsigned char *src, int dw, int dh, int sw, int sh);

void  u_fbmem_trans_cp(UImage *d, UImage *s, int dx, int dy, int sx, int sy, int w, int h);
void  u_fbmem_trans_op(UImage *d, int bx, int by, int fx, int fy, 
                       int w, int h, UImage *bg, UImage *fg);
void  u_fbmem_trans_op_color(UImage *d, int dx, int dy, int sx, int sy, 
                       int w, int h, COLOR color, UImage *fg);
void  u_fbmem_trans_op_draw(UImage* dst, int x, int y, int w, int h, int bx, int by, UImage* bg, UImage* fg);
void  u_fbmem_trans_draw(UImage *d, UImage *s, int dx, int dy);
void  u_fbmem_cp(PUImage d, PUImage s, int dstx, int dsty, 
             int srcx, int srcy, int w, int h);
void  u_fbmem_cp_strip_alpha(PUImage d, PUImage s, int dstx, int dsty, 
             int srcx, int srcy, int w, int h);
void  u_fbmem_draw(UImage *d, UImage *s, int dx, int dy);
void  u_fbmem_put_pixel(UImage *img, int x, int y, COLOR color);
COLOR u_fbmem_get_pixel(UImage *img, int x, int y);
void  u_fbmem_draw_hline(UImage *img, int x, int y, int w, COLOR color);
void  u_fbmem_draw_vline(UImage *img, int x, int y, int h, COLOR color);
void  u_fbmem_fill_rect(UImage *img, int x, int y, int w, int h, COLOR color);
void  u_fbmem_cp_entire(UImage *d, UImage *s);
void  u_fbmem_cp_map_area(UImage *d, UImage *s, int x, int y, int w, int h);

#endif

