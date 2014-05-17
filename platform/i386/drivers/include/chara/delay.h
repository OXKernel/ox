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
#ifndef DELAY_H
#define DELAY_H
/*
    @file:
        delay.h

    @description:
        Delay calibration. Determine what integer is needed
    to count to in order to delay the CPU for a specific time.

    @author:
        Roger G. Doss
        Codes originally based on

        GazOS Operating System
        Copyright (C) 1999  Gareth Owen <gaz@athene.co.uk>

        Distributed under the terms and conditions of the GPL.
*/

void delay(unsigned long milliseconds);
unsigned long calibrateDelayLoop(void);

#endif
