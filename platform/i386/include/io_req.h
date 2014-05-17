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
 *
 * 	io_req.h
 *
 * @description:
 *
 * 	Utility routines to manage IO regions as requested
 * 	by device drivers.
 * 	
 * 	NOTE: These are advisory.  It is still possible to
 * 	attempt to use a region already in use by another
 * 	driver; however, the behavior is undefined and
 * 	will most likely crash the running system.
 *
 * @author:
 * 	Roger G. Doss
 */
#ifndef _IO_REQ_H
#define _IO_REQ_H

#define NR_IO_DESCRIP_NAME_MAX  128
#define NR_IO_REGIONS	        32

typedef int io_region_id_t;

typedef struct io_region {

	io_region_id_t io_id;

	unsigned long io_start, 
		          io_finish;

	char 	      io_descr[NR_IO_DESCRIP_NAME_MAX];

}io_region_t;

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
io_request_region( unsigned long start, unsigned long finished, const char *desc );

/*
 * io_release_region:
 * 
 * Provided with the IO region id, this routine
 * releases the IO region.
 */ 
void
io_release_region( io_region_id_t id );

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
io_region_info( io_region_t **region, size_t *len );

#endif
