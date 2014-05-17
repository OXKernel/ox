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
#ifndef _EXIT_H
#define _EXIT_H
/********************************************************
 * Copyright (C)  Roger George Doss. All Rights Reserved.
 ********************************************************
 *
 *  @module   
 *	exit.h
 *      contains exit system call and related calls.
 *
 ********************************************************/
#include <ox/process.h>
#include <ox/scheduler.h>
#include <sys/wait.h>

int kkill(pid_t pid, int signal);
int kexit(long exit_code);
struct process *find_init();
void free_process(struct process *proc);
int kwaitpid(pid_t pid, WAIT_STATUS status, int options);

#endif // _EXIT_H
