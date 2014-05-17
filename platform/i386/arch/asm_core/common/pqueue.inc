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
;
;	specialized/optimized queue manipulation macros
;	for use in asm_core.s ONLY
;
%define __PROC_NEXT__ 0x24
%define __PROC_PREV__ 0x28


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       external symbols
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

extern process_size_tab			; kernel/process_queue.c
extern panic				; kernel/panic.c

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; data section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

section .data
eformat  db 'line (%d),file (%s)',10,'ERROR (%s)',10,0
message  db 'queue size not equal to one, with a single node queue',10,0
message1 db 'process priority and queue are not equal',10,0

;
;	CHECK_QUEUE_PRIORITY
;		Explicitly checks to see if
;		we have an entry that has been
;		queued on the wrong queue, that
;		is, its priority did not equal
;		its queue.
;
%macro CHECK_QUEUE_PRIORITY 0
	; get the priority from the struct
	mov  dword ebx,[__PROC_PRIORITY__ + eax]
	
	; assert that the process priority
	; and the queue it was on are equal
	cmp dword ebx,[__WHICH_QUEUE__ + esp]
	je  %%priority_ok
	push dword message1
	push dword __FILE__
	push dword __LINE__
	push dword eformat
	call panic
%%priority_ok:
	%endmacro

;
;	CHECK_QUEUE_INTEGRITY
;		Explicitly checks to see
;		if we have an entry in the queue
;		that points to itself, and yet
;		has a size not equal to one		
;
%macro CHECK_QUEUE_INTEGRITY 0
	cmp dword [__PROC_NEXT__ + eax],eax
	jne dont_panic
	cmp dword [__PROC_PREV__ + eax],eax
	jne dont_panic
	cmp dword esi,1
	je  dont_panic
	push dword message
	push dword __FILE__
	push dword __LINE__
	push dword eformat
	call panic
	; kernel now idle
	%endmacro

;
;	QUEUE_READ_FRONT
;		Read the head of the queue,
;		adjust links accordingly.
;		If this was the last entry in the
;		queue, set next and prev pointers
;		to NULL, size == 0.
;		EAX has the address of the process to be
;		    removed.
;		EBX has its priority, already
;		    set by caller of this macro.
;
%macro  QUEUE_READ_FRONT 0
	; eax has the pointer to the
	; structure we are trying to remove
	push dword esi
%ifdef __VERBOSE_ERROR_CHECKING__
	; ebx has the queue this belongs
	push dword ebx
%endif
	push dword ecx
	push dword edx

%ifdef __VERBOSE_ERROR_CHECKING__
	CHECK_QUEUE_PRIORITY
%endif

	; get the size of the queue
	mov  dword esi,[process_size_tab + (ebx * 4)]
	
	; compare to see if this process
	; next and prev pointers point to itself
	cmp dword [__PROC_NEXT__ + eax],eax
	jne %%not_singular
	cmp dword [__PROC_PREV__ + eax],eax
	jne %%not_singular
	; compare size of queue to one
	cmp dword esi,1
	jne %%not_singular
	; zero out the entry in the process_tab
	mov dword [process_tab + (ebx * 4)],0
	jmp %%clear_pointers

%%not_singular:
%ifdef __VERBOSE_ERROR_CHECKING__
	CHECK_QUEUE_INTEGRITY
%endif

%%dont_panic:
	; normal queue processing
	; unlink the process from its queue
	
	;process_tab[priority]->p_prev->p_next = 
	;process_tab[priority]->p_next
	mov dword ecx,eax
	mov dword edx,[__PROC_PREV__ + ecx]
	mov dword ecx,[__PROC_NEXT__ + ecx]
	mov dword edx,[__PROC_NEXT__ + ecx]
	
	; process_tab[priority]->p_next->p_prev =
	; process_tab[priority]->p_prev
	mov dword ecx,eax
	mov dword edx,[__PROC_NEXT__ + ecx]
	mov dword ecx,[__PROC_PREV__ + ecx]
	mov dword edx,[__PROC_PREV__ + ecx]

	; process_tab[priority]= process_tab[priority]->p_next
	mov dword ecx,eax
	mov dword ecx,[__PROC_NEXT__ + ecx]
	mov dword [process_tab + (ebx * 4)],ecx

%%clear_pointers:
	; proc_q->p_next = proc_q->p_prev = NULL
	mov dword [__PROC_NEXT__ + eax],0
	mov dword [__PROC_PREV__ + eax],0
	
	; process_size_tab[priority]--
	dec dword [process_size_tab + (ebx * 4)]
	
	; restore stack
	pop  dword edx
	pop  dword ecx
%ifdef __VERBOSE_ERROR_CHECKING__
	pop  dword ebx
%endif
	pop  dword esi
	%endmacro

;
;	QUEUE_INSERT
;		Inserts process onto approperiate queue.
;		ESI points to the queue head, ie
;		    mov dword esi,[process_tab + (ebx * 4)]
;		EAX points to the new entry. 
;		EBX contains the priority.
;
%macro QUEUE_INSERT 0
	push dword edx
	
	cmp dword [esi],0
	jne %%queue_not_empty
	; queue was empty, insert eax as head
	mov dword [__PROC_NEXT__ + eax],eax
	mov dword [__PROC_PREV__ + eax],eax
	mov dword [process_tab + (ebx * 4)],eax
	jmp %%done
	
%%queue_not_empty:
	; proc->p_next = process_tab[priority]
	mov dword edx,esi
	mov dword [__PROC_NEXT__ + eax],edx
	
	; proc->p_prev = process_tab[priority]->p_prev;
	mov dword edx,esi
	mov dword edx,[__PROC_PREV__ + edx]
	mov dword [__PROC_PREV__ + eax],edx
	
	; process_tab[priority]->p_prev->p_next = proc;
	mov dword edx,esi
	mov dword edx,[__PROC_PREV__ + edx]
	mov dword [__PROC_NEXT__ + edx],eax
	
	; process_tab[priority]->p_prev = proc;
	mov dword edx,esi
	mov dword [__PROC_PREV__ + edx],eax
	
%%done:	
	; process_size_tab[priority]++;
	inc dword [process_size_tab + (ebx * 4)]
	
	; restore stack
	pop dword edx
	%endmacro
	
;
;	QUEUE_SIZE
;		Obtains the size of a given queue
;		EAX will have size of associated queue
;		EBX has priority
;
%macro QUEUE_SIZE 0
	; return ( process_size_tab[proc_q->p_priority] )
	mov dword eax,[process_size_tab + (ebx * 4)]
	%endmacro
