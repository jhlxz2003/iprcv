
#ifndef _URECT_H_
#define _URECT_H_

#include "umacros.h"

U_BEGIN_DECLS

int  u_rect_set         (PURect lprc, int xLeft, int yTop,
                         int xRight, int yBottom);
int  u_rect_empty       (PURect lprc);
int  u_rect_is_empty    (const PURect lprc);
int  u_rect_cp          (PURect lprcDst, const PURect lprcSrc);
int  u_rect_is_equal    (const URect *lprc1, const URect *lprc2);
int  u_rect_normalize   (PURect lprc);
int  u_rect_intersect   (PURect lprcDst, const URect *lprcSrc1,
                         const URect *lprcSrc2);
int  u_rect_reset       (PURect lprc);
int  u_rect_is_intersect(const URect *lprc1, const URect *lprc2);
int  u_rect_shift       (PURect lprc, int dx, int dy);
int  u_rect_inflate     (PURect lprc, int dx, int dy);
int  u_pt_in_rect       (const URect *lprc, int x, int y);

U_END_DECLS

#endif

