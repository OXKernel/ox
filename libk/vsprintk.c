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
/**************************************************************
 * Copyright (C)  Roger George Doss. All Rights Reserved.
 **************************************************************
 *
 *	@module
 *		vsprintk.c
 *
 **************************************************************/
#include <stdarg.h>

#include <ox/error_rpt.h>
#include <ox/defs.h>
#include <ox/types.h>

#include <ox/lib/vsprintk.h>
#include <ox/lib/conversions.h>
#include <ox/lib/util.h>
#include <ox/lib/string.h>
#include <ox/lib/ctype.h>

struct vflags {
		char type;	/* h,l       */
		char sign;	/* +         */
		char ljust;	/* -	     */
		char sharp; /* #         */
		char space; /* ' '       */
		char fill;  /* O         */
		char prec;	/* precision */
		char width;	/* width     */
};

#define reset_vflags()	memset(&vflags,0,sizeof(struct vflags))

/* vsprintk:-
 * 	This routine works like the ANSI C definition,
 *	except that it handles ONLY integral values;
 *	NO floating point and it has some support for
 *	ISO C facilities.
 */
int
vsprintk( char *buf, const char *fmt, va_list ap )
{
	static char tmp[MAX_BUFFER];
	char   *bp = NULL, *p = NULL,*pp = NULL, *ppp = NULL;
	char   *prefix = NULL;
	int     n = 0,x = 0,s = 0;
	struct vflags vflags = {0};
	char   fill ='\0', pad ='\0';

	for ( bp = buf, p = fmt; p && *p; p++ ) {
		if(*p != '%') {
			*bp++ = *p;
			continue;
		}
READ_FLAGS:
		++p;
		if(*p == '0') {
			vflags.fill = '0';
			goto READ_FLAGS;
		}
		else if(*p == '.') {
			/* precision */
			if(*++p == '*') {
				vflags.prec = va_arg(ap,int);
				goto READ_FLAGS;
			}
			else {
				for ( pp = tmp; pp && p && isdigit(*p); p++,pp++)
					*pp = *p;
				*pp++ = '\0';
				vflags.prec  = atoi(tmp);
				--p;
			}
			goto READ_FLAGS;	
		}
		else if(isdigit(*p)) {
			/* width     */	
			for ( pp = tmp; pp && p && isdigit(*p); p++,pp++)
				*pp = *p;
			*pp++ = '\0';
			vflags.width = atoi(tmp);
			--p;
			goto READ_FLAGS;
		}
		else if(*p == '*') {
			/* width     */
			vflags.width = va_arg(ap,int);
			goto READ_FLAGS;
		}
		else if(*p == 'h') {
			vflags.type = 'h';
			goto READ_FLAGS;
		}
		else if(*p == 'l') {
			vflags.type = 'l';
			goto READ_FLAGS;
		}
		else if(*p == '-') {
			vflags.ljust = '-';
			goto READ_FLAGS;
		}
		else if(*p == '+') {
			vflags.sign = '+';
			goto READ_FLAGS;
		}
		else if(*p == ' ') {
			vflags.space= ' ';
			goto READ_FLAGS;
		}
		else if(*p == '#') {
			vflags.sharp= '#';
			goto READ_FLAGS;
		}
		else {
			switch(*p) {
		/* conversions */
				case '%':
					/* percent */
					*bp++ = *p;
				break;

				case 'c':
					/* char */
					pp = tmp;
					if(vflags.type == 'l')
						*pp = va_arg(ap,wint_t);
					else
						*pp = va_arg(ap,int);


					/* determine fill character */
					if(vflags.fill == '0') {
						fill = '0';
					}
					else
						fill = ' ';

					/* setup width right justify */
					if(vflags.ljust != '-' && vflags.width > 1) {
						pad = vflags.width - 1;
						while(pad--)
							*bp++ = fill;
					}
					/* copy converted integer */
					*bp++ = *pp;
					
					/* setup width left justify */
					if(vflags.ljust == '-' && vflags.width > 1) {
						pad = vflags.width - 1;
						while(pad--)
							*bp++ = ' ';
					}

				break;

				case 's':
					/* string */
					if(vflags.type == 'l') {
						pp = tmp;
						to_cp(pp,va_arg(ap,wchar_t *));
					}
					else
						pp = va_arg(ap,char *);

					/* determine fill character */
					if(vflags.fill == '0') {
						fill = '0';
					}
					else
						fill = ' ';

					/* determine how much to write */
					for(ppp =  pp; ppp && *ppp; ppp++);
					n = ppp - pp;
					if(vflags.prec > 0)
						n = MIN(n,vflags.prec);

					/* setup width right justify */
					if(vflags.ljust != '-' && vflags.width > n) {
						pad = vflags.width - n;
						while(pad--)
							*bp++ = fill;
					}

					/* copy converted string */
					for (x = 0; pp && *pp && x < n; pp++,x++ ) {
						*bp++ = *pp;
					}

					/* setup width and left justify */
					if(vflags.ljust == '-' && vflags.width > n) {
						pad = vflags.width - n;
						while(pad--)
							*bp++ = ' ';
					}

				break;

				case 'u':
					/* unsigned int */
					pp = tmp;
					if(vflags.type == 'h')
						n=ultostr(pp,va_arg(ap,unsigned short),DEC);
					else if(vflags.type == 'l')
						n=ultostr(pp,va_arg(ap,unsigned long),DEC);
					else
						n=ultostr(pp,va_arg(ap,unsigned),DEC);
					goto MAIN_FORMATTING;

				case 'o':
					/* octal */
					pp = tmp;
					if(vflags.type == 'h')
						n=ultostr(pp,va_arg(ap,unsigned short),OCT);
					else if(vflags.type == 'l')
						n=ultostr(pp,va_arg(ap,unsigned long),OCT);
					else
						n=ultostr(pp,va_arg(ap,unsigned),OCT);
					goto MAIN_FORMATTING;

				case 'x':
				case 'X':
					/* hex */
					pp = tmp;
					if(vflags.type == 'h')
						n=ultostr(pp,va_arg(ap,unsigned short),HEX);
					else if(vflags.type == 'l')
						n=ultostr(pp,va_arg(ap,unsigned long),HEX);
					else
						n=ultostr(pp,va_arg(ap,unsigned),HEX);
					if(*p == 'x') {
						for(ppp = pp; ppp && *ppp; ppp++)
							if(isalpha(*ppp))
								*ppp = tolower(*ppp);	
					}
					goto MAIN_FORMATTING;

				case 'p':
					/* pointer */
					pp = tmp;
					n=ultostr(pp,(unsigned long)va_arg(ap,void *),HEX);
					goto MAIN_FORMATTING;

				case 'd':
				case 'i':
					/* int */
					pp = tmp;
					if(vflags.type == 'h')
						n=ltostr(pp,va_arg(ap,short),DEC);
					else if(vflags.type == 'l')
						n=ltostr(pp,va_arg(ap,long),DEC);
					else
						n=ltostr(pp,va_arg(ap,int),DEC);

MAIN_FORMATTING:
					/* main formatting code for integral types */
					/* determine fill character */
					if(vflags.fill == '0')
						fill = '0';
					else
						fill = ' ';

					/* determine prefix
					 */
					if(*p == 'd' || *p == 'i') {
						if(*pp == '-') {
							pp++;
							prefix = "-";
							s = 1;
						}
						else if(vflags.sign == '+') {
							prefix = "+";
							n++;
							s = 1;
						}
						else if(vflags.space== ' ') {
							prefix = " ";
							n++;
							s = 1;
						}
					}
					else if(*p == 'o' ) {
						pp++;
						if(vflags.sharp == '#') {
							prefix = "0";
							s = 1;
						}
						else
							n--;
					}
					else if(*p == 'x' || *p == 'p') {
						pp += 2;
						if(vflags.sharp == '#') {
							prefix = "0x";
							s = 2;
						}
						else
							n  -= 2;
					}
					else if(*p == 'X') {
						pp += 2;
						if(vflags.sharp == '#') {
							prefix = "0X";
							s = 2;
						}
						else
							n -= 2;	
					}

					/* right adjusted, with a zero fill
					 * copy prefix before pad 
					 */
					if(vflags.ljust != '-' && fill == '0')
						for(ppp = prefix; ppp && *ppp; ppp++)
							*bp++ = *ppp;

					x = MAX(n,(vflags.prec + s));
					if(vflags.ljust != '-' && vflags.width > x) {
						pad = vflags.width - x;
						while(pad--)
							*bp++ = fill;
					}

					/* left adjusted or space fill
					 * copy prefix before the converted value
					 */
					if(vflags.ljust == '-' || fill == ' ')
						for(ppp = prefix; ppp && *ppp; ppp++)
							*bp++ = *ppp;

					/* setup precision */
					if(vflags.prec > (n - s)) {
						pad = vflags.prec - (n - s);
						while(pad--)
							*bp++ = '0';
					}

					/* copy converted value */
					for ( ; pp && *pp; pp++,bp++ )
						*bp = *pp;

					/* setup width left justify */
					if(vflags.ljust == '-' && vflags.width > x) {
						pad = vflags.width - x;
						while(pad--)
							*bp++ = ' ';
					}
				break;

				case 'n':
					/* ISO C, place number of bytes processed
					 * so far into integer arg
					 */
					if(vflags.type == 'h')
						(*(va_arg(ap,short *)))= bp - buf;
					else if(vflags.type == 'l')
						(*(va_arg(ap,long *))) = bp - buf;
					else
						(*(va_arg(ap,int *)))  = bp - buf;
				break;
				default:
					/* go back to start of bad format */
					while(*p != '%' && p != fmt) p--;
					/* write out from that point on   */
					for( pp = p; pp && *pp ;pp++)
						*bp++ = *pp;
					/* avoid possible disaster */
					goto EXIT;
			}/* switch */
		}/* else */
		n = s = x = 0;
		prefix = NULL;
		reset_vflags();
	}/* for */

EXIT:
	/* return the number of characters we
	 * processed
	 */
	*bp = '\0';
	return (bp - buf);

}/* vsprintk */

/*
 * EOF
 */ 
