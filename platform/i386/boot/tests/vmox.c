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
/*
 * Test kernel
 * for loader.
 */

void main ( void );

/* without C startup code,
 * we need to write our own _start
 * entry point
 */
void
_start( void )
{
	main();

}/* start */

char *message = "--> Executing test kernel <-- ";

/* Force the executable to be larger
 * so we can test the loader loading a maximum
 * sized kernel image.
 * RGD: Note that vmox is not 5102 its 5109 now.
 *(0x7ac12) -> (0x90000 - 0x14000 { max kernel size } ) - 
 *	       (512 bytes + { s1 } + 16384 { s2 } + 5102 { vmox })
 *
 * 0x17	    -> less 22 bytes needed for the additional code
 *
 * (0x7c000)-> is the maximum we can load with 
 * 	       end segment being 0x9000, and start segment being
 *	       0x1400, ie, 507904 in decimal.
 */
//#define D_SIZE 0x7ac12 - 0x17
#define D_SIZE 0x76a0b - 0x17
char data[D_SIZE] = {0};

void
main ( void )
{
	char *vram = (char *)0xB8000;

	while(*message) {
		*vram++	= *message++;
		*vram++	= 0x7;
	}

	for( ; ; )
		/* idle */;

}/* main */

/*
 * EOF
 */
