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
#ifndef _KEYBOARD_H
#define _KEYBOARD_H
/*
 * @file:
 * 	keyboard.c
 *
 * @description:
 * 	Keyboard device driver.
 *
 * @author:
 *
 * 	Dr. Roger G. Doss, PhD
 *
 *      Some codes based on GazOS Operating System
 *      Copyright (C) 1999  Gareth Owen <gaz@athene.co.uk>
 *
 *      Distributed under GPL.
*/
#define MASTER_PIC 0x20
#define EOI        0x20

irq_stat_t keyboard_handler();
void keyboard_enable();
void keyboard_disable();
unsigned char keyboard_getch();
unsigned char keyboard_hit();

#endif // _KEYBOARD_H
