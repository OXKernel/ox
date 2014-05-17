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
   Modified for OX as print routine for testing
   the OX kernel...

   RGD --

   Portions from GazOS Operating System
   Copyright (C) 1999  Gareth Owen <gaz@athene.co.uk>

*/
#include <asm_core/io.h>
#include <drivers/chara/console.h>

unsigned char *VIDEO_MEMORY = (char *)0xB8000;

void console_write(char *string)
{
   unsigned int curchar, vidmem_off, i;

   io_outb(0x3d4, 0x0e);		/* Get cursor Y position	*/
   vidmem_off = io_inb(0x3d5);
   vidmem_off <<= 8;
   io_outb(0x3d4, 0x0f);		/* And add cursor X position	*/
   vidmem_off += io_inb(0x3d5);
   vidmem_off <<= 1;

   while((curchar=*string++))		/* Loop through the string	*/
   {
      switch(curchar)			/* Is it a special character ?  */
      {
         case '\n':			/* Newline found		*/
            vidmem_off = (vidmem_off/160)*160 + 160;
            break;

         case '\r':			/* Carriage return found	*/
            vidmem_off = (vidmem_off/160)*160;
            break;

	case '\t':
	    vidmem_off += 8;
	    break;
	 
	case 8:/* Delete */
	    vidmem_off-=2;
	    VIDEO_MEMORY[vidmem_off] = 0x20;
	    break;

         default:			/* Normal character             */
            VIDEO_MEMORY[vidmem_off++] = curchar;
            VIDEO_MEMORY[vidmem_off++] = 0x07;
            break;
      }

      if(vidmem_off >= 160*25)		/* Are we off-screen ?		*/
      {
         for(i = 0; i < 160*24; i++)	/* Scroll the screen up		*/
         {
            VIDEO_MEMORY[i] = VIDEO_MEMORY[i+160];
         }
         for(i = 0; i < 80; i++)	/* Empty the bottom row		*/
         {
            VIDEO_MEMORY[(160*24)+(i*2)] = 0x20;
            VIDEO_MEMORY[(160*24)+(i*2)+1] = 0x07;
         }
         vidmem_off -= 160;		/* We're on the bottom row	*/
      }
   }

   vidmem_off >>= 1;			/* Set the new cursor position  */
   io_outb(0x3d4, 0x0f);
   io_outb(0x3d5, vidmem_off & 0x0ff);
   io_outw(0x3d4, 0x0e);
   io_outb(0x3d5, vidmem_off >> 8);

}/* console_write */

void console_clear(void)
{
   unsigned int i;

   for(i = 0; i < (80*25); i++)         /* Fill the screen with         */
   {					/* background color		*/
      VIDEO_MEMORY[i*2] = 0x20;
      VIDEO_MEMORY[i*2+1] = 0x07;
   }

   io_outb(0x3d4, 0x0f);		/* Set the cursor to the	*/
   io_outb(0x3d5, 0);			/* upper-left corner of the	*/
   io_outw(0x3d4, 0x0e);		/* screen			*/
   io_outb(0x3d5, 0);

}/* console_clear */
