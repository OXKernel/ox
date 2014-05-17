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
//
// @file:
//      dma.c
// @description:
//      Read/Write DMA (Used for implementing Floppy Drive driver).
//
// @author:
//      Dr. Roger G. Doss, PhD
//
//      Derived from :=
//      GazOS Operating System
//      Copyright (C) 1999  Gareth Owen <gaz@athene.co.uk>
//
#include <ox/mm/dma.h>

#include <asm_core/io.h>
#include <ox/types.h>
#include <platform/asm_core/util.h>

/* Defines for accessing the upper and lower byte of an integer. */
#define LOW_BYTE(x)         (x & 0x00FF)
#define HI_BYTE(x)          ((x & 0xFF00) >> 8)

/* Quick-access registers and ports for each DMA channel. */
u8_t MaskReg[8]   = { 0x0A, 0x0A, 0x0A, 0x0A, 0xD4, 0xD4, 0xD4, 0xD4 };
u8_t ModeReg[8]   = { 0x0B, 0x0B, 0x0B, 0x0B, 0xD6, 0xD6, 0xD6, 0xD6 };
u8_t ClearReg[8]  = { 0x0C, 0x0C, 0x0C, 0x0C, 0xD8, 0xD8, 0xD8, 0xD8 };

u8_t PagePort[8]  = { 0x87, 0x83, 0x81, 0x82, 0x8F, 0x8B, 0x89, 0x8A };
u8_t AddrPort[8]  = { 0x00, 0x02, 0x04, 0x06, 0xC0, 0xC4, 0xC8, 0xCC };
u8_t CountPort[8] = { 0x01, 0x03, 0x05, 0x07, 0xC2, 0xC6, 0xCA, 0xCE };

void _dma_io(u8_t DMA_channel, unsigned char page, unsigned int offset, unsigned int length, u8_t mode);

void dma_io(unsigned char channel, unsigned long address, unsigned int length, unsigned char read)
{
	unsigned char page=0, mode=0;
	unsigned int offset = 0;
	
	if(read)
		mode = 0x48 + channel;
	else
		mode = 0x44 + channel;
		
	page = address >> 16;
	offset = address & 0xFFFF;
	length--;
	
	_dma_io(channel, page, offset, length, mode);	
}	
		

void _dma_io(u8_t DMA_channel, unsigned char page, unsigned int offset, unsigned int length, u8_t mode)
{
    /* Don't let anyone else mess up what we're doing. */
    asm_disable_interrupt();

    /* Set up the DMA channel so we can use it.  This tells the DMA */
    /* that we're going to be using this channel.  (It's masked) */
    io_outb(MaskReg[DMA_channel], 0x04 | DMA_channel);

    /* Clear any data transfers that are currently executing. */
    io_outb(ClearReg[DMA_channel], 0x00);

    /* Send the specified mode to the DMA. */
    io_outb(ModeReg[DMA_channel], mode);

    /* Send the offset address.  The first byte is the low base offset, the */
    /* second byte is the high offset. */
    io_outb(AddrPort[DMA_channel], LOW_BYTE(offset));
    io_outb(AddrPort[DMA_channel], HI_BYTE(offset));

    /* Send the physical page that the data lies on. */
    io_outb(PagePort[DMA_channel], page);

    /* Send the length of the data.  Again, low byte first. */
    io_outb(CountPort[DMA_channel], LOW_BYTE(length));
    io_outb(CountPort[DMA_channel], HI_BYTE(length));

    /* Ok, we're done.  Enable the DMA channel (clear the mask). */
    io_outb(MaskReg[DMA_channel], DMA_channel);

    /* Re-enable interrupts before we leave. */
    asm_enable_interrupt();
}
