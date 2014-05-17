;
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 2
; of the License, or (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copyright (C) Roger G. Doss. All Rights Reserved
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	@module
;		syscall.s
;	@description
;		contains system call entry handling code
;	@author
;		Roger G. Doss
;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; include files
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%include "common/macros.inc"
%include "common/proc.inc"
%include "common/sys_arg.inc"
%include "common/ctx.inc"
%include "common/cdef.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; macros and defines
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; external symbols
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

extern syscall_dispatch_tab     ; ox/syscall_dispatch_tab.h
extern INTERRUPT_COUNTER        ; kernel/def_int.c
extern DEFINT_PENDING           ; kernel/def_int.c
extern INIT_TASK                ; ox/schedule.h

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; external routines
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

extern sys_trace_syscall	; kernel/syscall/syscall0.c
extern signal_exec		; kernel/signal.c
extern defint_exec 		; kernel/defint.c
extern schedule			; kernel/scheduler.c

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; text section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
section .text

;
; syscall_handler_return_jmp
;
;	This is used as a workaround for 128byte short
;	jmp limitation.  It simply calls the intended
;	target.
;
syscall_handler_return_jmp:
	jmp syscall_handler_return

;
;	syscall_handler:-
;
;		This is the actual system call handler.
;
C_ENTRY syscall_handler
	push eax						; save call #
	CTX_SAVE
	mov  dword [__EAX__ + esp],-__ENOSYS__			; return code if there is an error
	cmp  dword [Nr_SYS_CALL],eax				; is the system call # valid ?
	jae  syscall_handler_return_jmp				; return otherwise
	mov  dword eax,[syscall_dispatch_tab + (eax * 4)]	; lookup the routine in the kernel
	test dword eax,eax					; is the address valid ?
	je   syscall_handler_return_jmp				; return otherwise
	mov  dword ebx,[current_process]			; setup current process pointer
	and  dword [__EFLAGS__ + esp],~__CF_MASK__		; set clear flag
	mov  dword ecx,dr6					; set debug register 6 value in ctx
	mov  dword [__PROC_DBGREG6__ + ebx],ecx
	mov  dword edx,[__PROC_FLAGS__ + ebx] 			; PTRACE_SYSCALL
	test dword edx,PROC_TRACE_SYSCALL     			; check process flags, should we trace
	jne  .trace						; trace the call
	SYS_ARG_SET						; setup stack parameters for syscall
	call eax	 		      			; make actual system call
	SYS_ARG_UNSET						; restore stack parameters after syscall
	mov  dword [__EAX__ + esp ],eax				; record return value
	jmp syscall_handler_return				; return

	.trace:

		;
		; call sys_trace_syscall to trace
		; system call usage for this process
		;
		call sys_trace_syscall				; trace system call
		mov  dword eax,[__TMP__ + esp]			; TMP denotes the original call #
		mov  eax,[syscall_dispatch_tab + ( eax * 4)]	; lookup the routine in the kernel
		SYS_ARG_SET					; setup stack parameters for syscall
		call eax					; make actual system call
		SYS_ARG_UNSET					; restore stack parameters after syscall
		mov  dword [__EAX__ + esp],eax			; store return value
		mov  dword  eax,[current_process]
		call sys_trace_syscall
		jmp syscall_handler_return			; return


;
;       defint:-
;
;               Called to handle any work that may have
;		been deferred on Interrupt handling.
;
C_ENTRY defint
        inc dword [INTERRUPT_COUNTER]
        call defint_exec
        dec dword [INTERRUPT_COUNTER]
	jmp handle_defint

;       reschedule:-
;               Called by syscall routines to reschedule
;       running process.
;
C_ENTRY reschedule
        push dword syscall_signal_return
        jmp schedule

;
;	syscall_handler_return:-
;		This prepares us to return from the
;	system call.
;
C_ENTRY syscall_handler_return
C_ENTRY common_handler_return
		cmp dword [INTERRUPT_COUNTER],0
		jne restore_regs

handle_defint:
		cmp dword [DEFINT_PENDING],0
		jne defint

		mov eax,[__EFLAGS__  + esp]	
		test dword eax,__VM_MASK__
		jne .reenable_interrupts

		; check if we got here from the kernel
		xor eax,eax
		mov word ax,[__CS__ + esp]
		cmp word ax,KERNEL_CS
		je restore_regs
		mov eax,[__EFLAGS__  + esp]

		.reenable_interrupts:
			sti
			or  dword eax, __IF_MASK__
			and dword eax,~__NT_MASK__
			mov dword [__EFLAGS__ + esp],eax
			xor dword eax,eax

			mov dword eax,[current_process]
			cmp dword [__PROC_RESCHEDULE__ + eax],0	; should we run another process
			jne reschedule

		.signals:
			mov dword eax,[current_process]
			; avoid signal processing for init
			cmp dword [INIT_TASK],eax
			je  restore_regs
			mov dword ecx,[__PROC_BLOCKED__ + eax]	; signals that are blocked
			mov dword ebx,ecx
			not dword ecx
			and dword ecx,[__PROC_SIGNAL__ + eax]	; signals pending
			jne syscall_signal_return

		; there are no signals to process
		; restore the stack and return to whatever we
		; were doing prior
		restore_regs:
			; are we in the kernel
			mov word ax,[__CS__ + esp]
			cmp word ax,KERNEL_CS
			je .restore_ctx

			; setup status into debug register 7 for user
			xor eax,eax
			mov dword eax,[current_process]
			mov dword ebx,[__PROC_DBGREG7__ + eax ]
			mov dword dr7,ebx

			.restore_ctx:
			CTX_RESTORE
			add esp,4
			iret

;
;	syscall_signal_return:
;		This returns us from signal.
;
C_ENTRY syscall_signal_return

	mov  dword ecx,esp		; cpu context as saved by CTX_SAVE
	push dword ecx			; second argument to signal_exec, struct cpu_ctx
	push dword ebx			; first  argument to signal_exec, unsigned long oldmask
	call signal_exec		; handle the pending signal
	pop  dword ebx			; restore the stack
	pop  dword ebx
	jmp restore_regs

;
; EOF
;
