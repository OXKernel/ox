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
//      bitmap.c
//
// @description:
//      Get and Set bits in a char stream of data.
//      The length of the data is not relevant and
//      can be arbitrary large.
//
// @author:
//      Dr. Roger G. Doss, PhD
//
#include "bitmap.h"

int bit_get (char *data, int bit)
{
   int pos = bit / 8;
   int b   = bit % 8;
   if(data[pos] & (1 << b)) {
      return 1;
   } else {
      return 0;
   } 
}

void bit_set(char *data, int bit, int value)
{
   int pos = bit / 8;
   int b   = bit % 8;

   if(value == 0) {
      data[pos] &= ~(1 << b);
   } else if(value == 1) {
      data[pos] |= (1 << b);
   }
}


#ifdef _TEST_BITMAP
int 
main(int argc, char **argv)
{
   char data[4]={0};
   int  i;
   unsigned d = 0;
   printf("%d\n",bit_get(data,0));
   bit_set(data,0,1);
   printf("%d\n",bit_get(data,0));

   bit_set(data,0,0);
   printf("%d\n",bit_get(data,0));

   bit_set(data,11,1);
   bit_set(data,10,1);
   printf("%d\n",bit_get(data,10));

   bit_set(data,10,0);
   printf("%d\n",bit_get(data,10));
   printf("%d\n",bit_get(data,11));

   for(i = 0; i < 32; i++) {
      bit_set(data,i,1);
   }
   memcpy(&d,data,4);
   printf("%u\n",d);
   for(i = 0; i < 32; i++) {
      bit_set(data,i,0);
   }
   memcpy(&d,data,4);
   printf("%u\n",d);
   return 0;
}
#endif
