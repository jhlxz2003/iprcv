#ifndef __U_LIB_H__
#define __U_LIB_H__

#define  PNG_LO_READ_ENTIRE

////////////////////////////////////////////////////////////////////

#include "umacros.h"
#include "alpha.h"
#include "bdl.h"
#include "daemon_init.h"
#ifdef LIBEV
#include "../ev/ev.h"
#else
#include "../event/event.h"
#endif
#include "mpg123.h"
#include "mxml.h"
#include "ntpcli.h"
#include "queue.h"
#include "threadpool.h"
#include "u_md5.h"
#include "ubase64.h"
#include "ucrypt.h"
#include "ud.h"
#include "udir.h"
#include "uflock.h"
#include "ufs.h"
#include "uftp.h"

#include "upng.h"
#include "ujpeg.h"
#include "uimage.h"

#include "umem.h"
#include "unotifier.h"
#include "utimeout.h"
#include "utime.h"
#include "urwlock.h"
#include "umq.h"
#include "utimer.h"

#include "umisc.h"
#include "urect.h"

#include "usock.h"
#include "usocku.h"

#include "urand.h"
#include "ulist.h"

#include "uio.h"
#include "ushmem.h"
#include "usleep.h"
#include "uping.h"
#include "usem.h"

#include "utypes.h"
#include "utype.h"

#include "utty.h"
#include "usignal.h"
#include "util.h"

#include "hal/rgb.h"
#include "hal/fb.h"
#include "hal/init.h"

#endif

