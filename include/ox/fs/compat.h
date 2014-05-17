/*

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#ifndef _COMPAT_H
#define _COMPAT_H

#ifdef _TEST_INODE_INC
#define P() printf("file=%s line=%d\n",__FILE__,__LINE__)
#else
#define P()
#endif

#define _POSIX_SOURCE 1

#include "errno.h"
#include "time.h"
#include "sys/unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/fcntl.h"
#include "sys/utime.h"
#include "sys/signal.h"
#include "dirent.h"
#include "ox/defs.h"
#include "platform/protected_mode_defs.h"
#include "platform/segment.h"
#include "platform/tss.h"
#include "ox/process.h"
#include "ox/scheduler.h"
#include "ox/ktime.h"

#define SSIZE_MAX 2147483647

#endif
