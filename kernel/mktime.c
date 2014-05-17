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
/********************************************************
 * Copyright (C)  Roger George Doss. All Rights Reserved.
 ********************************************************
 *
 *	@module
 *		mktime.c
 *
 *  @description:
 *      This code is derived from linux from code originally
 *      copyright Linus Torvalds. It used under
 *      the terms and conditions of the GPL.
 *
 ********************************************************/
#include <time.h>
#include <ox/ktime.h>

#include <asm_core/io.h>
#include <ox/fs.h>
#include <ox/fs/fs_syscalls.h>
#include <ox/fs/compat.h>
#include <sys/signal.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <ox/types.h>
#include <ox/defs.h>
#include <platform/protected_mode_defs.h>
#include <platform/segment.h>
#include <platform/tss.h>
#include <ox/process.h>
#include <ox/scheduler.h>

//
// References (oldlinux) include/asm/io.h
// outb_p
// inb_p
//
// We have equivalents of these ASM routines
// written in NASM. The _p refers to a pause
// introduced using a jump instruction.
//
#define CMOS_READ(addr) ({ \
    io_outb_p(0x80|addr,0x70); \
    io_inb_p(0x71); \
})

#define BCD_TO_BIN(val) ((val)=((val)&15) + ((val)>>4)*10)

time_t ktime(time_t *t)
{
	struct tm tms = {0};
    time_t tmp = 0;

	do {
		tms.tm_sec = CMOS_READ(0);
		tms.tm_min = CMOS_READ(2);
		tms.tm_hour = CMOS_READ(4);
		tms.tm_mday = CMOS_READ(7);
		tms.tm_mon = CMOS_READ(8);
		tms.tm_year = CMOS_READ(9);
	} while (tms.tm_sec != CMOS_READ(0));

	BCD_TO_BIN(tms.tm_sec);
	BCD_TO_BIN(tms.tm_min);
	BCD_TO_BIN(tms.tm_hour);
	BCD_TO_BIN(tms.tm_mday);
	BCD_TO_BIN(tms.tm_mon);
	BCD_TO_BIN(tms.tm_year);
	tms.tm_mon--;

	tmp = kmktime(&tms);

    if(t) {
        (*t) = tmp;
    }
    return tmp;
}/* ktime */

#define MINUTE 60
#define HOUR (60*MINUTE)
#define DAY (24*HOUR)
#define YEAR (365*DAY)

/* interestingly, we assume leap-years */
static int month[12] = {
	0,
	DAY*(31),
	DAY*(31+29),
	DAY*(31+29+31),
	DAY*(31+29+31+30),
	DAY*(31+29+31+30+31),
	DAY*(31+29+31+30+31+30),
	DAY*(31+29+31+30+31+30+31),
	DAY*(31+29+31+30+31+30+31+31),
	DAY*(31+29+31+30+31+30+31+31+30),
	DAY*(31+29+31+30+31+30+31+31+30+31),
	DAY*(31+29+31+30+31+30+31+31+30+31+30)
};

time_t kmktime(struct tm * tm)
{
	long res = 0;
	int year = 0;

	year = tm->tm_year - 70;
    /* magic offsets (y+1) needed to get leapyears right.*/
	res = YEAR*year + DAY*((year+1)/4);
	res += month[tm->tm_mon];
    /* and (y+2) here. If it wasn't a leap-year, we have to adjust */
	if (tm->tm_mon>1 && ((year+2)%4))
		res -= DAY;
	res += DAY*(tm->tm_mday-1);
	res += HOUR*tm->tm_hour;
	res += MINUTE*tm->tm_min;
	res += tm->tm_sec;
	return res;
}// kmktime

clock_t ktimes(struct tms *buf)
{
    if(buf) {
        current_process->p_utime = buf->tms_utime;
        current_process->p_stime = buf->tms_stime;
        current_process->p_cutime= buf->tms_cutime;
        current_process->p_cstime= buf->tms_cstime;
    }
    return (time_t)ktime(NULL);
}// ktimes

