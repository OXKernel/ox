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
 * @file:
 *      page_enable.h
 *
 * @description:
 *      Interface to NASM routines for paging.
 *
 *      page_enable - enables paging given kernel_page_dir
 *      page_flush_tlb - flushes the TLB cache for a given page
 *      page_flush_tlb_386 - flushes the TLB cache for 386 processors using
 *                           kernel_page_dir
 *
 * @author:
 *      Dr. Roger G. Doss, PhD
 */                           
#ifndef _PAGE_ENABLE_H
#define _PAGE_ENABLE_H

void page_enable(void *kernel_page_dir);
void page_flush_tlb(void *virtual_addr);
void page_flush_tlb_386(void *virtual_addr);

#endif
