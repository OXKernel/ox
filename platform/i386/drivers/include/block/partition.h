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
#ifndef _PARTITION_H
#define _PARTITION_H

/*
 * struct partition
 *
 * Loaded in lba 0 of every drive
 * at byte offset 446 (0x1BE)
 * and is 64 bytes long.
 * See http://www.tech-juice.org/2011/08/08/an-introduction-to-hard-disk-geometry/
 * We only care aboyt nr_sects (total number of sectors on
 * the volume).
 */
typedef struct partition {
    unsigned char boot_ind;     /* 0x80 - active (unused) */
    unsigned char head;         /*   */
    unsigned char sector;       /*   */
    unsigned char cyl;          /*   */
    unsigned char sys_ind;      /*   */
    unsigned char end_head;     /*   */
    unsigned char end_sector;   /*   */
    unsigned char end_cyl;      /*   */
    unsigned int start_sector;  /* starting sector counting from 0 */
    unsigned int nr_sectors;    /* nr of sectors in partition */
} partition_t;

#endif
