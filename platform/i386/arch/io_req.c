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
 * @module:
 * 	io_req.c
 * @description:
 * 	Implementation of routines to manage IO regions as
 * 	requested by device drivers.
 * @author:
 * 	Roger G. Doss
 */
#include <stddef.h>
#include <platform/io_req.h>
#include <sys/types.h>
#include <ox/error_rpt.h>

#define IO_INVALID -1

static io_region_t io_tab[NR_IO_REGIONS];
static size_t curr;
static size_t id;

/*
 * io_region_id_t:
 * 
 * Provided with a region, this routine returns a region id,
 * an integer value >= 0.  On error, the routine
 * returns -1.
 *
 * Callers of the routine should store the return value,
 * and check it to see if the region has been allocated
 * to them.
 *
 */
io_region_id_t
io_request_region( unsigned long start, unsigned long finished, const char *desc )
{
	size_t i, len;

	if(!desc ) {
	 	PRINTK("IO_REGION: NULL Descriptor\n");      
		return (IO_INVALID);
	}	
	if (start >= finished) {
		PRINTK("IO_REGION: start address less than or equal to finish address\n");
		return (IO_INVALID);	
	}
	if((long)start < 0 ) {
		PRINTK("IO_REGION: start address is negative\n");
		return (IO_INVALID);
	}
	if ((long)finished < 0) {
		PRINTK("IO_REGION: finish address is negative\n");
		return (IO_INVALID);
	}
	if((curr+1) == NR_IO_REGIONS) {
		PRINTK("IO_REGION: slot full\n");
		return (IO_INVALID);
	}
	if((len=strlen(desc)) >= NR_IO_DESCRIP_NAME_MAX) {
		PRINTK("IO_REGION: Description too long\n");
		return (IO_INVALID);
	}
	/* Check to see if the region has already been assigned. */
	for( i =0; i < curr; i++ ) {
		if( start >= io_tab[i].io_start && start <= io_tab[i].io_finish ) {
			printk("IO_REGION: start address overlap:\n%s\n",io_tab[i].io_descr);
			return (IO_INVALID);
		}
		if( finished >= io_tab[i].io_start && finished <= io_tab[i].io_finish ) {
			printk("IO_REGION: finish address overlap:\n%s\n",io_tab[i].io_descr);
			return (IO_INVALID);
		}
	}

	/* Assign region to the caller. */
	++curr;
	++id;
	io_tab[curr].io_id       = id;
	io_tab[curr].io_start    = start;
	io_tab[curr].io_finish   = finished;
	strncpy (io_tab[curr].io_descr,desc,NR_IO_DESCRIP_NAME_MAX);
	io_tab[curr].io_descr[len]= '\0';
	return ( curr );

}/* io_request_region */

/*
 * io_release_region:
 * 
 * Provided with the IO region id, this routine
 * releases the IO region.
 */ 
void
io_release_region( io_region_id_t id )
{
	int i, j;
	for ( i =0; i < curr; i++ ) {
		if(io_tab[i].io_id == id) {
			for(j =i+1; j < curr; j++, i++) {
				io_tab[i] = io_tab[j];	
			}
			--curr;
			return;
		}
	}

	PRINTK("IO_REGION: io_release of undefined region\n");

}/* io_release_region */

/* 
 * io_region_info:
 * 
 * Provided with an address of a pointer, and a pointer to 
 * a length, this routine returns the information
 * regarding IO regions, and the length of the table.
 * IO id's are equivalent to slots in this table.
 *
 */
void
io_region_info( io_region_t **region, size_t *len )
{
	if(!region || !len) {
		PRINTK("IO_REGION: invalid parameters\n");
		return;
	}

	*region = io_tab;
	*len    = curr;

}/* io_region_info */

/*
 * EOF
 */
