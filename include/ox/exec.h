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
#ifndef _EXEC_H
#define _EXEC_H

#ifdef _TEST_EXEC
#define LINE() printk("line=%d\tfile=%s\n",__LINE__,__FILE__);
#else
#define LINE()
#endif

typedef int (*start_t)();
typedef int (*main_t)(int argc, char **argv);
typedef int (*main2_t)(int argc, char **argv, char **envp);

// If p_first_exec == 1, run this method first.
void kstart();

#endif // _EXEC_H
