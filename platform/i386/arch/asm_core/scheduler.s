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
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copyright (C) 1999.  Roger George Doss. All Rights Reserved
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	@module 
;		asm_core.s
;
;	@description
;		Core kernel written in nasm for x86.
;		This is the main kernel assembler file
;		containing kernel low-level scheduling routines
;		including context switching and CPU idle routines.
;
;	@author
;		Roger George Doss
;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; include files
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%include "common/macros.inc"
%include "common/pqueue.inc"
%include "common/debug.inc"
%include "common/proc.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	external symbols
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

extern process_tab			; process table
extern which_queue			; 0-31 represents a queue
extern dma_active			; 1 if we are using dma
extern print_reg            ; libk/printk.c
extern enable_irq           ; arch/i386/interrupt.c

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	data section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
section .data
message2 db 'asm_switch attempted to run NULL process',10,0
proc_selector: dd 0
ctx_tmp: dd 0
mode: dd 0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	special macros
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro ASM_SWITCH_PANIC 0
	push dword message2
	push dword __LINE__
	push dword __FILE__
	push dword eformat
	call panic
	%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; text section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

section .text

;
;       asm_idle
;		This routine will idle the processor
;		NOTE: We are expecting to be called only from
;		asm_switch.
C_ENTRY asm_idle
	sti
        cmp dword [which_queue],0      ; any process available to run
        jne asm_select_queue	       ; found a process to run
        cmp dword [dma_active],1       ; if we are using dma, loop for(;;);
        jne asm_idle
        hlt                            ; hlt the processor
        jmp asm_idle


%define __PREV_PROC__ 	8	       ; offset on stack of previous process
;
; 	asm_switch
;		This is our context switching code
;	we accomplish a context switch by doing a long jmp
;	into the next process' TSS.
;
C_ENTRY asm_switch
	push  dword eax				; save eax
	xor   dword eax,eax			; set eax == 0
	xchg  dword eax,[current_process]	; current_process == 0
	
	push  dword eax				; save current process
	push  dword ebx				; save registers for tmp work
        push  dword esi

	; find highest priority queue
	; or idle
	xor dword ebx,ebx
	mov dword esi,1

	cli					; disable interrupts

asm_select_queue:
		mov dword eax,[which_queue]
		and dword eax,esi
		jnz .queue_selected		
		shl dword esi,1
		inc dword ebx
		cmp dword ebx,32
		jl asm_select_queue
		jmp asm_idle				; no queue is enabled, idle
	
	.queue_selected:
		; eax will have size of queue
		; ebx has priority
		QUEUE_SIZE

		; check size
		cmp   dword eax,0
		je   .dont_switch

		mov   dword eax,[process_tab + (ebx * 4 )] 	; eax == pointer to process

		; check if this is the same process we were running
		cmp   dword eax,[__PREV_PROC__ + esp]
		jne   .continue

		; if selected queue was empty, or, the
		; selected process was the same as current...
		.dont_switch:
			; disable the queue
			btc [which_queue],ebx
			mov dword [current_process], eax

			; restore registers
			pop  dword esi	 ; original esi
			pop  dword ebx	 ; original ebx
			add  dword esp,4 ; skip current_process
			pop  dword eax	 ; original eax
			ret

		.continue:
		; check if this is NULL
		; NOTE: This is a kernel panic, the queue size
		;	should equal zero if empty; moreover,
		;	the queue should not have been enabled
		;	in the first place!
		cmp   dword eax,0
		jne .continue1
			ASM_SWITCH_PANIC

		.continue1:
		; eax has pointer to process
		; ebx has priority
		QUEUE_READ_FRONT			; free process from queue

		mov  dword esi,eax			; esi == pointer to process

		; set the previous_process
		mov  dword eax,[__PREV_PROC__ + esp]
		mov  dword [previous_process],eax

		; set the current_process
		mov dword [current_process],esi

		; extract the TSS selector from selected process
		mov  dword eax,[__PROC_TSS_SEG__ + esi]

	 	; restore registers and stack
		pop dword esi
		pop dword ebx
		add dword esp,8

		; long jump into the TSS selector, causing CONTEXT-SWITCH
		jmp dword [eax]
		ret

;
;	asm_qswitch
;		The purpose of this routine is to insert the current
;		process onto a queue, enable its queue, and to then to schedule
;		the current process.
;
C_ENTRY asm_qswitch
	push  dword eax                         ; save eax
        xor   dword eax,eax                     ; set eax == 0
        xchg  dword eax,[current_process]       ; current_process == 0

        push  dword eax                         ; save current process
        push  dword ebx                         ; save registers for tmp work
        push  dword esi

	xor dword ebx,ebx
	cli 					; disable interrupts

	; - clear rescheduling and signal flags
	;   of machine dependent process
	; - extract priority of process
	; - find approperiate run queue
	; - set flag for the run queue
	;   enabling the queue
	; - queue the process
	; - call asm_select_queue

	and   dword [__PROC_FLAGS__ + eax],~__PROC_RESCHEDULE__
	mov   dword ebx,[__PROC_PRIORITY__ + eax]
	bts   dword [which_queue],ebx			; enable the queue
	xor   dword esi,esi
	mov   dword esi,[process_tab + (ebx * 4)]

	; esi points to the queue head, ie
	;     mov dword esi,[process_tab + (ebx * 4)]
	; eax points to the new entry. 
	; ebx contains the priority.
	QUEUE_INSERT

	; make sure to set state == to asm_switch
	; prior to jmp to asm_select_queue
	xor   dword ebx,ebx
	mov   dword esi,1
	jmp   asm_select_queue

;
; 	asm_switch_to_current
;		This is our context switching code
;	we accomplish a context switch by doing a long jmp
;	into the next process' TSS. In this version,
;   the C code in kernel/scheduler.c has selected
;   which process to switch to and its in current_process
;   so we switch to that process.
;
C_ENTRY asm_switch_to_current
        push dword esi
        mov dword esi,[current_process]

		; extract the TSS selector from selected process
		mov  dword eax,[__PROC_TSS_SEG__ + esi]

	 	; restore registers and stack
		pop dword esi

		; long jump into the TSS selector, causing CONTEXT-SWITCH
		; jmp dword [eax]
        mov dword [proc_selector],eax
        ;jmp dword proc_selector:0 ; This is not ok, but what we want ?
        ;jmp dword [proc_selector] ; This is ok but not what we want.
        jmp dword 48:0 ; This is broken, we have the wrong state.
        ;jmp dword proc_selector ; This is ok but not what we want.
        ;call dword proc_selector:0
		ret

;
; stack layout
;
%define OLD_EIP 0x0
%define OLD_TSS 0x4 ; first param
%define NEW_TSS 0x8 ; second param
%define MODE    0xC ; third param

;
; offsets into struct tss
; NOTE: These are in decimal.
; see platform/i386/include/tss.h
; Context switch is based on :=
; forum.osdev.org/viewtopic.php?f=1&t=20059
;
%define TSS_CR3     28
%define TSS_EIP     32
%define TSS_EFLAGS  36
%define TSS_ESP     56
%define TSS_EBP     60
%define TSS_EBX     52
%define TSS_ESI     64
%define TSS_EDI     68
%define TSS_EAX     40
%define TSS_ECX     44
%define TSS_EDX     48

; TODO - This routine needs some work...
;        Perhaps the stack/EIP are not being saved/restored correctly
;void asm_soft_switch(struct tss *old_tss, struct tss *new_tss, int mode);
C_ENTRY asm_soft_switch
    push eax
    mov dword eax, [esp + MODE + 0x4]
    mov [mode],eax
    pop eax
    cmp dword [esp + OLD_TSS],0 ; old was null (should only occur on init)
    je .copy_new
    push eax
    mov dword eax,[esp + OLD_TSS]
    push eax
    ;call print_reg
    add esp,4
    pop eax
.save_old:
    mov dword [ctx_tmp],esp
    push eax
    push ebx
    mov dword ebx,[esp + OLD_EIP + 0x8] ; save EIP
    mov dword eax,[esp + OLD_TSS + 0x8] ; get pointer to old_tss, EAX is struct tss *old_tss
    ; mov dword [eax + TSS_EIP],ebx ; move EIP
    mov dword ebx,[ctx_tmp]
    mov dword [eax + TSS_ESP],ebx ; move stack
    pop ebx; restore EBX
    mov dword [eax + TSS_EBP],ebp
    mov dword [eax + TSS_EBX],ebx
    mov dword [eax + TSS_EDI],edi
    mov dword [eax + TSS_ESI],esi
    mov dword [eax + TSS_ECX],ecx
    mov dword [eax + TSS_EDX],edx
    pushf
    pop dword ebx ; Now has EFLAGS contents
    mov dword [eax + TSS_EFLAGS],ebx
    pop ebx ; Now has eax contents
    mov dword [eax + TSS_EAX],ebx ; Save eax.
.copy_new:
    cmp dword [esp + NEW_TSS],0 ; new was null
    je .done
    ; Do the switch
    mov dword eax,[esp + NEW_TSS] ; EAX is struct tss *new_tss.
    ;push dword eax ; RGDDEBUG
    ;call print_reg
    ;add esp,4
    mov esi,[eax + TSS_ESP]
    push dword esi ; save new stack
    mov esi,[eax + TSS_ESI]
    mov edi,[eax + TSS_EDI]
    mov ebx,[eax + TSS_EBX]
    mov ebp,[eax + TSS_EBP]
    mov ecx,[eax + TSS_ECX]
    mov edx,[eax + TSS_EDX]
    push ebx
    mov ebx,[eax + TSS_EFLAGS] ; Save EFLAGS to restore.
    push dword ebx
    popf    ; Restore EFLAGS.
    mov ebx,[eax + TSS_EIP]
    mov dword [ctx_tmp],ebx ; Save EIP to restore.
    mov ebx,[eax + TSS_EAX] ; Save EAX to restore.

    ; Print out the address we are going to for debugging.
    ; push dword [ctx_tmp] ; RGDDEBUG
    ; call print_reg
    ; add esp,4
    ; re-enable the scheduler() interrupt
    ;sti
    cli
    push dword 0
    call enable_irq
    add esp,4
    ; NOTE: I tried this out in user space where
    ; I save the EIP by issuing a 'mov eax,[esp]'
    ; and then calling eax later to load the new
    ; EIP. It seems to work.
    mov eax,ebx; Restore EAX value which we moved into ebx above.
    pop ebx ; Restore EBX value which we pushed onto the stack above.
    pop esp ; Restore old stack.
    cmp dword [mode],1
    jne .done
        call [ctx_tmp] ; Switch to this EIP
.done:
    ret ; Shouldn't return here but to where this function is called.
        ; This is based on the EIP we save in OLD_TSS which
        ; we get from the stack.
;
; EOF
;
