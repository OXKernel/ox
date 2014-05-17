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
 * <sys/termios.h>
 *
 ********************************************************/
#ifndef _TERMIOS_H
#define _TERMIOS_H  1
#ifdef __cplusplus
 extern "C" {
#endif

typedef unsigned short tcflag_t;  /* terminal flags */
typedef unsigned short     cc_t;  /* control characters */
typedef unsigned int   speed_t;   /* speed */
#define NCCS   12                 /* number of control characters */

/* POSIX Table 7-1 */
struct termios {
  tcflag_t c_iflag;     /* input   mode flag  */
  tcflag_t c_oflag;     /* output  mode flag  */
  tcflag_t c_cflag;     /* control mode flag  */
  tcflag_t c_lflag;     /* local   mode flag  */
  cc_t     c_line;      /* line    dicipline  */
  cc_t     c_cc[NCCS];  /* control characters */
};

struct winsize {
  unsigned short ws_row;    /* rows    */ 
  unsigned short ws_col;    /* columns */
  unsigned short ws_xpixel; /* horizontal pixel size */
  unsigned short ws_ypixel; /* vertical   pixel size */
};

/* POSIX Table 7-2 termios.c_iflag 's */
#define BRKINT      0x0001  /* signal interupt on break */
#define ICRNL       0x0002  /* input carriage return -> new line */
#define IGNBRK      0x0004  /* ignore break */
#define IGNCR       0x0008  /* ignore carriage return */
#define IGNPAR      0x0010  /* ignroe characters with parity errors */
#define INLCR       0x0020  /* input new line -> carriage return */
#define INPCK       0x0040  /* input parity check */
#define ISTRIP      0x0080  /* mask 8th bi */
#define IXOFF       0x0100  /* start/stop input  control */
#define IXON        0x0200  /* start/stop output control */ 
#define PARMRK      0x0400  /* parity mark errors in queue */

/* POSIX Table 7-3 termios.c_oflag 's */
#define OPOST       0x0001  /* process output */

/* POSIX Table 7-4 termios.c_cflag 's */
#define CLOCAL      0x0001  /* ignore modem status lines */
#define CREAD       0x0002  /* enable reciever */
#define CSIZE       0x000C  /* number of bits per character */
#define CS5         0x0000  /* 5 bit characters */
#define CS6         0x0004  /* 6 bit characters */
#define CS7         0x0008  /* 7 bit characters */
#define CS8         0x000C  /* 8 bit characters */
#define CSTOPB      0x0010  /* 2 stop bits, default 1 if not set */
#define HUPCL       0x0020  /* hang-up last close */
#define PARENB      0x0040  /* parity enabled on output */
#define PARODD      0x0080  /* odd parity, default even if not set */

/* POSIX Table 7-4 termios.c_lflag 's */
#define ECHO        0x0001  /* echo input characters */
#define ECHOE       0x0002  /* echo erase as backspace */
#define ECHOK       0x0004  /* echo kill */
#define ECHONL      0x0008  /* echo new line */
#define ICANON      0x0010  /* canonicial (cooked) input, erase & kill enabled */
#define IEXTEN      0x0020  /* extended functions enabled */
#define ISIG        0x0040  /* enable signals */
#define NOFLSH      0x0080  /* disable flush after quit/interrupt */
#define TOSTOP      0x0100  /* send SIGTTOU */

/* POSIX Table 7-5 termios.c_cc 's */
#define  VEOF       0       /* c_cc[VEOF]   == EOF   (^D)            */
#define  VEOL       1       /* c_cc[VEOL]   == EOL   (undefined)     */
#define  VERASE     2       /* c_cc[VERASE] == ERASE (^H)            */
#define  VINTR      3       /* c_cc[VINTR]  == INTR  (DEL)           */
#define  VKILL      4       /* c_cc[VKILL]  == KILL  (^U)            */
#define  VMIN       5       /* c_cc[VMIN]   == MIN   value for timer */
#define  VQUIT      6       /* c_cc[VQUIT]  == QUIT  (^\)            */
#define  VTIME      7       /* c_cc[VTIME]  == TIME  value for timer */
#define  VSUSP      8       /* c_cc[VSUSP]  == SUSP  (^Z)            */
#define  VSTART     9       /* c_cc[VSTART] == START (^S)            */
#define  VSTOP     10       /* c_cc[VSTOP]  == STOP  (^Q)            */

#define  _POSIX_VDISABLE ((cc_t)0xFF)

/* POSIX Table 7-6 baud speed settings */
#define  B0        0x0000  /* hang up       */
#define  B50       0x1000  /* 50    baud    */
#define  B75       0x2000  /* 75    baud    */
#define  B110      0x3000  /* 110   baud    */
#define  B134      0x4000  /* 134.5 baud    */ 
#define  B150      0x5000  /* 150   baud    */
#define  B200      0x6000  /* 200   baud    */
#define  B300      0x7000  /* 300   baud    */
#define  B600      0x8000  /* 600   baud    */
#define  B1200     0x9000  /* 1200  baud    */
#define  B1800     0xA000  /* 1800  baud    */
#define  B2400     0xB000  /* 2400  baud    */
#define  B4800     0xC000  /* 4800  baud    */
#define  B9600     0xD000  /* 9600  baud    */
#define  B19200    0xE000  /* 19200 baud    */
#define  B38400    0xF000  /* 38400 baud    */

/* POSIX Table 7.2.1.2, tcsetattr() flags */
#define TCSANOW    1       /* immediate effect for changes */
#define TCSADRAIN  2       /* changes take effect after output */
#define TCSAFLUSH  3       /* wait for output before flushing input */

/* POSIX Table 7.2.2.2, tcflush() flags  */
#define TCIFLUSH   1       /* flush input        */
#define TCOFLUSH   2       /* flush output       */
#define TCIOFLUSH  3       /* flush input/output */ 

/* POSIX Table 7.2.2.2, tcflow() flags   */
#define TCOOFF     1       /* suspend output           */
#define TCOON      2       /* restart suspended output */
#define TCIOFF     3       /* send stop  charr         */
#define TCION      4       /* send start char          */

extern int tcsendbreak(int fd, int duration);
extern int tcdrain    (int fd);
extern int tcflush    (int fd, int queue_selector);
extern speed_t cfgetispeed(const struct termios *termios_p);
extern speed_t cfgetospeed(const struct termios *termios_p);
extern int     cfsetispeed(struct termios *termios_p,speed_t speed);
extern int     cfsetospeed(struct termios *termios_p,speed_t speed);
extern int     tcgetaddr  (int fd, struct termios *termios_p);
extern int     tcsetaddr  (int fd, int opt_actions,struct termios *termios_p);

#ifdef __cplusplus
 }
#endif
#endif /* _SYS_TERMIOS_H */
