#ifndef _U_TYPES_H_
#define _U_TYPES_H_

#ifndef U_TYPE
#define U_TYPE
typedef int	          bool;
typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef signed   short	SWORD;
typedef unsigned long	DWORD;
typedef signed   long	SDWORD;
typedef unsigned int	UINT;
#endif

typedef unsigned int  	WPARAM;
typedef unsigned long		LPARAM;

//typedef unsigned long	UINT32;
//typedef unsigned short	UINT16;
//typedef unsigned char	UINT8;

#define MAKEWORD(low, high) ((WORD)(((BYTE)(low))|(((WORD)((BYTE)(high)))<<8)))
#define LOWORD(l)           ((WORD)(DWORD)(l))
#define HIWORD(l)           ((WORD)((((DWORD)(l))>>16)&0xFFFF))
#define LOSWORD(l)          ((SWORD)(DWORD)(l))
#define HISWORD(l)          ((SWORD)((((DWORD)(l))>>16)&0xFFFF))

#define MAKELONG(low, high) ((long)(((WORD)(low))|(((DWORD)((WORD)(high)))<<16)))

#define Pi	3.1415926
#define DEG2RAD(x) Pi*(x)/180.000000
#define RAD2DEG(x) 180.000000*(x)/Pi

typedef struct _Point
{
	int x;
	int y;
} UPoint;

typedef UPoint* PUPoint;

typedef struct _Rect
{
	int left;
 	int top;
 	int right;
 	int bottom;
} URect;
typedef URect* PURect;

typedef struct _Size
{
	int cx;
	int cy;
} USize;
typedef USize* PUSize;

typedef struct _UImgInf {
	int width;
	int height;
	int rowbytes;
} UImgInf;
typedef UImgInf* PUImgInf;

typedef struct _Uimage {
	unsigned char *img;
	UImgInf        inf;
} UImage;
typedef UImage* PUImage;

#define UIMG_W       inf.width
#define UIMG_H       inf.height
#define UIMG_RBYTES  inf.rowbytes

#endif

