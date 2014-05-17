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
//     pioutil.c
//
// @description:
//     Implements C callable PIO hard disk access
//     and supporting routines. Requires pio_read/pio_write
//     low-level asm routines.
//
// @author:
//     Dr. Roger G. Doss, PhD
//
#include <ox/defs.h>
#include <ox/types.h>
#include <ox/linkage.h>
#include <ox/bool_t.h>
#include <drivers/block/pio.h>
#include <drivers/block/ide.h>
#include <drivers/block/partition.h>

#include <platform/interrupt.h>
#include <platform/interrupt_admin.h>
#include <platform/protected_mode_defs.h>
#include <platform/asm_core/interrupt.h>
#include <platform/asm_core/util.h>
#include <platform/gdt.h>
#include <platform/8259.h>

#include <asm_core/io.h>
#include <ox/error_rpt.h>
#include <ox/fs.h> // For BLOCK_START in inode.h.

#include <stddef.h>

#ifdef _TEST_PIO_UTIL
#include <stdio.h>
#define printk printf
#define panic  printf
unsigned io_outb_p(unsigned x,unsigned y){return 0;}
unsigned io_inb_p(unsigned x){return 0;}
#endif

irq_stat_t ide_handler(int irq)
{
    printk("ide_handler:: entering IDE handler\n");
    io_outb(MASTER_PIC, EOI);
    io_outb(SLAVE_PIC, EOI);
    printk("ide_handler:: done IDE handler\n");
    return IRQ_ENABLE;
}

irq_info_t ide_info()
{
    irq_info_t info;
    info.name  = "IDE";
    info.descr = "OX Kernel IDE Driver";
    info.version.major = 1;
    info.version.minor = 0;
    return info;
}

/*
 * ide_enable:
 * Initialize OS for ide i/o.
 */
void ide_enable(void)
{
    asm_disable_interrupt();
    printk("initializing IDE disk driver version 1.0\n");
    interrupt_install_handler(14,
              IRQ_EXCL,
              ide_handler,
              ide_info);
    printk("done initializing IDE disk driver version 1.0\n");
    enable_irq(14);
}/* ide_init */

/*
 * get_drive_head:
 *
 * drive can be 0 or 1 for first or second drive.
 * head can be 0-15 values of the last 4 bits.
 *
 */
unsigned char get_drive_head(unsigned char drive, 
                             unsigned char head)
{
   unsigned char val = 160; // bit 7 == 1, bit 5 == 1, all others 0.
   val |= (drive << 4); // Set the forth bit.
   val |= (head & 240); // Set bottom for bits 0-3.
   return val;
}/* get_drive_head */

/*
 * get_chs:
 *
 * Given an input representing the logical block address (lba),
 * convert it into a device useable chs address.
 * This is needed in the call to pio_read/pio_write.
 * From http://en.wikipedia.org/wiki/Logical_block_addressing.
 * HPC and SPT values are from 28 bit lba not 48 bit lba.
 */
#define HPC 16
#define SPT 63
void get_chs(unsigned int lba, unsigned char *head, unsigned char *sector, 
                               unsigned char *cyl1, unsigned char *cyl2)
{
    unsigned short cylinder = lba / (SPT * HPC);
    *head   = (lba / SPT) % HPC;
    *sector = (lba % SPT) + 1;
    *cyl1   = (unsigned char)(cylinder & 0xFF); /* low bits  */
    *cyl2   = (unsigned char)(cylinder >> 8); /* high bits */
}/* get_chs */

/*
 * get_lba:
 *
 * Given input of chs, convert to logical lba address.
 * From http://en.wikipedia.org/wiki/Logical_block_addressing.
 */
unsigned int get_lba(unsigned char head, unsigned char sector, 
                     unsigned char cyl1, unsigned char cyl2)
{
    unsigned short cylinder = ((cyl2 << 0x8) | cyl1);
    return ((cylinder * HPC) + head) * SPT + sector - 1;
}/* get_lba */

#define CMOS_READ(addr) ({ \
   io_outb_p(0x80|addr,0x70); \
   io_inb_p(0x71); \
})

/*
 * ata_nr_drives:
 *
 * Report if there are ATA drives
 * on the controller.
 *
 */
int ata_nr_drives(void)
{
    io_outb(0x70,0x12);
    register int disks = io_inb(0x70);
    if(disks & 0xF0) {
        if(disks & 0x0F) {
            return 2;
        } else {
            return 1;
        }
    }
    return 0;
}/* ata_nr_drives */

/*
 * ata_disk_size:
 *
 * Read the partition table of the drive
 * and report the size of the disk in bytes
 * nr_sect * DEV_BLOCK_SIZE.
 *
 */
unsigned long ata_disk_size(unsigned char drive, unsigned long *start_sector)
{
    char buf[512]={0}, *ptr = buf;
    partition_t *part = NULL;
    int nr_disks = ata_nr_drives();
    if(!nr_disks) {
        return 0;
    } else {
        printk("ata_disk_size:: there are %d disks found\n",nr_disks);
    }

    if(!ata_read(drive, 0, ptr)) {
        /*
         * Do the partition read and calculate
         * the size in bytes.
         *
         */
        // Should be offset 0x1BE
        // Partition table is 64 bytes long
        // See http://www.tech-juice.org/2011/08/08/an-introduction-to-hard-disk-geometry/
        part = 0x1BE + (void *)ptr;
        if(buf[510] != 0x55 ||
           (unsigned char)buf[511] != 0xAA) {
            printk("invalid partition table on drive=%d\n",drive);
            printk("buf[510]=[%x] buf[511]=[%x]\n",buf[510],buf[511]);
            printk("buf[254]=[%x] buf[255]=[%x]\n",buf[254],buf[255]);
            printk("buf[0]=[%x]   buf[1]=[%x]\n"  ,buf[0]  ,buf[1]);
            printk("start_sector=[%d] size=[%d]\n",part->start_sector,
                part->nr_sectors * 512);
            ide_initialize(0x1F0, 0x3F4, 0x170, 0x374, 0x000);
            /* Access ide_devices to calculate disk size. */
            /* TODO: For now, just get the master drive on
             *       Primary Channel.
             */
            if(!ide_devices[0].Size) {
                printk("NO IDE device on primary channel\n");
                panic("...");
            }
            part->nr_sectors = ide_devices[0].Size;
            /* Now start sector is based on the init
             * function in the file system.
             * See BLOCK_START in the file system code.
             */
            part->start_sector = BLOCK_START;
            buf[510] = 0x55;
            buf[511] = 0xAA;
            /* Now write the partition table.
             */
            if(!ata_write(drive, 0, ptr)) {
                if(!ata_read(drive, 0, ptr)) {
                    if(buf[510] != 0x55 ||
                        (unsigned char)buf[511] != 0xAA) {
                            panic("error writing partition...\n");
                    } else {
                        printk("successfully wrote partition in MBR\n");
                    }
                } else {
                    panic("error reading partition...\n");
                }
            } else {
                panic("error writing partition...\n");
            }
            /* Return our disk size. */
            *start_sector = BLOCK_START;
            return part->nr_sectors * 512; 
        }
        *start_sector = part->start_sector;
        return part->nr_sectors * 512;
    } else {
        /* This shouldn't happen, if ata_read
         * fails we will panic...
         */
        *start_sector = 0;
        return 0;
    }
}/* ata_disk_size */

/*
 * ata_read:
 *
 * Read from the ATA disk.
 *
 */
int ata_read(unsigned char drive, unsigned int lba, char *buffer)
{
    unsigned char head       = 0x0;
    unsigned char sector     = 0x0;
    unsigned char cyl1       = 0x0;
    unsigned char cyl2       = 0x0;
    unsigned char drive_head = 0x0;
    get_chs(lba, &head, &sector, &cyl1, &cyl2);
    drive_head = get_drive_head(drive, head);
    if(pio_read(drive_head, sector, cyl1, cyl2, buffer) == 1) {
        printk("ata_read:: "
               "FAILED lba=%x drive=%x drive_head=%x sector=%x cyl1=%x cyl2=%x\n",
                lba,drive,drive_head,sector,cyl1,cyl2);
        panic("ata_read:: FAILED");
        // Coincides with dev.c; however, we will not return
        // from kernel panic...
        return -1;
    }
    local_sleep(1);
    return 0;
}/* ata_read */

/*
 * ata_write:
 *
 * Write to the ATA disk.
 *
 */
int ata_write(unsigned char drive, unsigned int lba, char *buffer)
{
    unsigned char head       = 0x0;
    unsigned char sector     = 0x0;
    unsigned char cyl1       = 0x0;
    unsigned char cyl2       = 0x0;
    unsigned char drive_head = 0x0;
    get_chs(lba, &head, &sector, &cyl1, &cyl2);
    drive_head = get_drive_head(drive, head);
    if(pio_write(drive_head, sector, cyl1, cyl2, buffer)== 1) {
        printk("ata_write:: "
               "FAILED lba=%x drive=%x drive_head=%x sector=%x cyl1=%x cyl2=%x\n",
                lba,drive,drive_head,sector,cyl1,cyl2);
        panic("ata_write:: FAILED");
        // Coincides with dev.c; however, we will not return
        // from kernel panic...
        return -1;
    }
    local_sleep(1);
    return 0;
}/* ata_write */

int ata_status_check(int operation, unsigned char error_reg, unsigned char status_reg)
{
    // Bit 0 is the error bit, if it is set
    // and Bit 7 (2^7)=128=0x80=BSY is 0 then
    // there was an error.
    int rtvl = 0;
    if((status_reg & 0x1) && !(status_reg & 0x80)) {
        if(operation == 0) {
            printk("error reading drive\n");
        } else if(operation == 1) {
            printk("error writing drive\n");
        }
        rtvl = 1;
        // Look into error_reg to output
        // what error it was, and return error.
        // EIDE has bits 5,3 MC Medium changed, MCR Medium change required.
        if(error_reg & 128) {
            printk("BBK(bit 7) sector marked as bad by host\n");
        }
        if(error_reg & 64) {
            printk("UNC(bit 6) uncorrectable data error\n");
        }
        if(error_reg & 16) {
            printk("NID(bit 4) ID mark not found\n");
        }
        if(error_reg & 4) {
            printk("ABT(bit 2) command abort\n"); 
        }
        if(error_reg & 2) {
            printk("NTO(bit 1) track 0 not found\n");
        }
        if(error_reg & 1) {
            printk("NDM(bit 0) data address mark not found\n");
        }
        // TODO:= Panic the kernel here.
        // Or, return the error to the caller and then panic
        // the kernel of writing out the CHS information.
        // - Integrate into the assembler.
    }
    return rtvl;
}/* ata_status_check */

void ata_test_rw(int sectors)
{
    static const int drive = 0;
    char buf[512]={0}, *ptr = buf;
    int i = 1;
    for(i = 1; i < sectors; ++i) {
	    printk("ata_test_rw:: testing write\n");
	    ptr[0]=i;
	    ata_write(drive,i,ptr);
	    printk("ata_test_rw:: done writing sector [%d]\n",i);
	    ata_read(drive,i,ptr);
	    if(ptr[0]==i) {
	        printk("ata_test_rw:: successfully wrote sector [%d]\n",i);
	    } else {
	        printk("failed ptr[0]=[%d] sector=[%d]\n",ptr[0],i);
	    }
    }
}/* ata_test_rw */

#ifdef _TEST_PIO_UTIL
int
main()
{
    // See the table in http://en.wikipedia.org/wiki/Logical_block_addressing.
    // Looks like everything works up to here.
    unsigned char head       = 0x0;
    unsigned char sector     = 0x0;
    unsigned char cyl1       = 0x0;
    unsigned char cyl2       = 0x0;
    unsigned char drive_head = 0x0;
    unsigned int lba = 16450559;
    unsigned int tmp = 16450559;
    get_chs(lba, &head, &sector, &cyl1, &cyl2);
    tmp = get_lba(head, sector, cyl1, cyl2);
    if(tmp != lba) {
        printk("error tmp=%d lba=%d\n", tmp,lba);
        printk("lba=%d cylinder=%d head=%d sector=%d\n",
                lba, (cyl2 << 8) | cyl1, head, sector);
    } else {
        // Expect chs= 16319, 15, 63
        printk("success lba=%d cylinder=%d head=%d sector=%d\n",
                lba, (cyl2 << 8) | cyl1, head, sector);
    }

    lba = 16514063;
    get_chs(lba, &head, &sector, &cyl1, &cyl2);
    tmp = get_lba(head, sector, cyl1, cyl2);
    if(tmp != lba) {
        printk("error tmp=%d lba=%d\n", tmp,lba);
        printk("lba=%d cylinder=%d head=%d sector=%d\n",
                lba, (cyl2 << 8) | cyl1, head, sector);
    } else {
        // Expect chs= 16319, 15, 63
        printk("success lba=%d cylinder=%d head=%d sector=%d\n",
                lba, (cyl2 << 8) | cyl1, head, sector);
    }
}
#endif
/*
 * EOF
 */
