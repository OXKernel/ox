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
/*******************************************************************
 * Copyright (C) Roger George Doss. All Rights Reserved.
 *******************************************************************
 *
 * @file:
 *      pio.h
 *
 * @description:
 *      ATA Disk drive read/write of sector sequentially.
 *
 * @author:
 *      Dr. Roger G. Doss, PhD
 *
 */
#ifndef _PIO_H
#define _PIO_H

#define MASTER_PIC 0x20
#define SLAVE_PIC  0xA0
#define EOI        0x20

/*
 * ide_enable:
 * Initialize OS for ide i/o.
 */
void ide_enable(void);

/*
 * ata_nr_drives:
 *
 * Report if there are ATA drives
 * on the controller.
 *
 */
int ata_nr_drives(void);

/*
 * ata_disk_size:
 *
 * Read the partition table of the drive
 * and report the size of the disk in bytes
 * nr_sect * DEV_BLOCK_SIZE.
 *
 */
unsigned long ata_disk_size(unsigned char drive, unsigned long *start_sector);

/*
 * ata_read:
 *
 * Read from the ATA disk.
 *
 */
int ata_read(unsigned char drive, unsigned int lba, char *buffer);

/*
 * ata_write:
 *
 * Write to the ATA disk.
 *
 */
int ata_write(unsigned char drive, unsigned int lba, char *buffer);

/*
 * ata_status_check:
 *
 * Using the status and error register report on error.
 */ 
int ata_status_check(int operation, unsigned char error_reg, unsigned char status_reg);

/*
 * get_drive_head:
 *
 * Returns a device useable 8 bits representing
 * the drive (0-1) and the head (0-15).
 * This is needed in the call to pio_read/pio_write.
 *
 */
unsigned char get_drive_head(unsigned char drive, 
                             unsigned char head);

/*
 * get_chs:
 *
 * Given an input representing the logical block address (lba),
 * convert it into a device useable chs address.
 * This is needed in the call to pio_read/pio_write.
 *
 */
void get_chs(unsigned int lba, unsigned char *head, unsigned char *sector, 
                               unsigned char *cyl1, unsigned char *cyl2);

/*
 * get_lba:
 *
 * Given input of chs, convert to logical lba address.
 */
unsigned int get_lba(unsigned char head, unsigned char sector, 
                     unsigned char cyl1, unsigned char cyl2);

/*
 * pio_read:
 *
 * Read from the drive using processor I/O in sequential
 * manner.
 *
 * NOTE: This method is assembler.
 *       Return of 0 == no error.
 *                 1 == error.
 *
 */
int pio_read(unsigned char drive_head, unsigned char sector,
             unsigned char cyl1, unsigned char cyl2, char *buff);

/*
 * pio_write:
 *
 * Read from the drive using processor I/O in sequential
 * manner.
 *
 * NOTE: This method is assembler.
 *       Return of 0 == no error.
 *                 1 == error.
 */
int pio_write(unsigned char drive_head, unsigned char sector,
               unsigned char cyl1, unsigned char cyl2, char *buff);


/*
 * ata_test_rw
 *
 * Test reading/writing a set of sectors.
 */ 
void ata_test_rw(int sectors);

#endif
