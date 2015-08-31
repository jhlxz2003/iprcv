#ifndef _HAL_RGB_H_
#define _HAL_RGB_H_

/* 24bits/32bits color */
typedef unsigned short  COLOR;

#define RGB565(r, g, b)    \
      (unsigned short)(((unsigned short)(r&0xf8)<<8)|((unsigned short)(g&0xfc)<<3)|((unsigned short)(b>>3)))

#define R(x)			((unsigned char)((((unsigned short)x)>>8)&0xf8))
#define G(x)			((unsigned char)((((unsigned short)x)>>3)&0xfc))
#define B(x)			((unsigned char)((((unsigned short)x)<<3)&0xf8))

#define RGB_WHITE			RGB565(255,255,255)
#define RGB_BLACK			RGB565(0,0,0)
#define RGB_GRAY			RGB565(0xC0,0xC0,0xC0)
#define RGB_DKGRAY		RGB565(0x80,0x80,0x80)
#define RGB_RED				RGB565(255,0,0)
#define RGB_GREEN			RGB565(0,255,0)
#define RGB_BLUE			RGB565(0,0,255)

//color style 1
#define RGB_NAVYBLUEBRIGHT      RGB565(136,192,184)
#define RGB_NAVYBLUEDARK        RGB565(72,144,136)
#define RGB_NAVYLTGRAY		      RGB565(200,224,216)
#define RGB_NAVYDKGRAY		      RGB565(44,78,71)

//color style 2
#define RGB_SYSTEMBRIGHT		    RGB_NAVYBLUEBRIGHT
#define RGB_SYSTEMDARK			    RGB_NAVYBLUEDARK
#define RGB_SYSTEMLTGRAY		    RGB_NAVYLTGRAY
#define RGB_SYSTEMDKGRAY		    RGB_NAVYDKGRAY
#define RGB_SYSTEMBLACK		      RGB_BLACK
#define RGB_SYSTEMWHITE		      RGB_WHITE
#define RGB_SYSTEMTRANS		      RGB565(255,0,0)
#define RGB_SYSTEMHIBRIGHT	    RGB565(173,221,255)

#endif

