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
;		interrupt.s
;
;	@description
;
;		low-level i8259 code
;
;	@author Roger G. Doss
;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  include files
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%include "common/macros.inc"
%include "common/ctx.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; external symbols
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

extern  irq_dispatch_tab        ; arch/i386/interrupt.c
extern  INTERRUPT_COUNTER	    ; kernel/def_int.c
extern  DEFINT_PENDING		    ; kernel/def_int.c
extern  print_reg               ; libk/printk.c
extern  enable_irq              ; arch/i386/interrupt.c

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; external routines
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

extern  defint_exec		    ; kernel/def_int.c

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  macros and defines
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%define INTERRUPT_MASTER     0x20   ; I/O Port for master controller
%define INTERRUPT_REQUEST    0x21   ; I/O Port for master IRQ's

%define INTERRUPT_SLAVE      0xA0   ; I/O Port for slave controller
%define INTERRUPT_REQUEST_S  0xA1   ; I/O Port for slave IRQ's

%define ENABLE               0x20   ; re-enable code for i8259

%ifdef  _USE_NOP
%define NOP nop
%else
%define NOP
%endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; text section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

section .text

;
; irq_init_ctl:-
;	This routine initializes the i8259 interrupt controller.
; For reference material on the code, see pp 661-675, "The Indispensible
; Hardware Book", Hans-Peter Messmer.
;
C_ENTRY irq_init_ctl
	cli
	; ICW1
	mov al,0x11
	out INTERRUPT_MASTER, al	; ICW1 edge triggering, cascading, ICW4 required
	out INTERRUPT_SLAVE,  al
	; ICW2
	mov al,0x08
	out INTERRUPT_REQUEST,al	; ICW2 master irq0 == int 0x08
	mov al,0x70
	out INTERRUPT_REQUEST_S,al	; ICW2 slave  irq8 == int 0x70
	; ICW3
	mov al,0x04
	out INTERRUPT_REQUEST,al	; ICW3 tell master slave PIC is attached at int 0x02
	mov al,0x02
	out INTERRUPT_REQUEST_S,al	; ICW3 tell slave PIC it is attached at int 0x02
	; ICW4
	mov al,0x01			; ICW4 tell master/slave to operate in 8088/8086 mode
	out INTERRUPT_REQUEST,al
	out INTERRUPT_REQUEST_S,al
	; disable interrupts for both master and slave
	; except for int 0x02
	mov al,11111011b
	out INTERRUPT_REQUEST,al
	mov al,11111111b
	out INTERRUPT_REQUEST_S,al
	sti
	ret


; irq_disable:-
;	Allows caller to disable a specified
; irq by programming the i8259 interrupt controller.
;
C_ENTRY irq_disable
	push dword edx			; save edx
	push dword ecx			; save ecx
	mov  dword ecx,[12 + esp]	; get  irq from stack
	pushf				; save flags
	cli				; disable interrupts

	mov  byte ah,1			; set ah == 1
	rol  byte ah,cl			; ah = 1 << (irq % 8)
	cmp  byte cl,8			; if (irq >= 8)
	jae  disable_slave_irq		;  goto disable_slave_irq

	; disable_master_irq
	in   al,INTERRUPT_REQUEST
	test byte al,ah			; check if port already disabled
	jnz  disabled
	or   byte al,ah
	out  INTERRUPT_REQUEST,al
	mov  eax,1			; return 1 on success
	jmp  irq_disable_done

disable_slave_irq:
	in   al,INTERRUPT_REQUEST_S
	test byte al,ah
	jnz  disabled
	or   byte al,ah
	out  INTERRUPT_REQUEST_S,al
	mov  eax,1			; return 1 on success
	jmp  irq_disable_done

disabled:				; return 0 on error
	xor eax,eax			
irq_disable_done:
	popf				; restore flags
	pop dword ecx			; restore ecx
	pop dword edx			; restore edx
	ret

;
; irq_enable:-
;	Allows caller to enable a specified
; irq by programming the i8259 interrupt controller.
;
C_ENTRY irq_enable
	push dword edx			; save edx
	push dword ecx			; save ecx
	mov  dword ecx,[12 + esp]	; get irq from stack
	pushf				; save flags
	cli				; disable interrupts

	mov byte ah,11111110b		; set ah == ~1
	rol byte ah,cl			; ah = ~(1 << (irq % 8))
	cmp byte cl,8			; if (irq >= 8)
	jae enable_slave_irq		;   goto enable_slave_irq

	; enable_master_irq
	in  al,INTERRUPT_REQUEST
	and byte al,ah
	out INTERRUPT_REQUEST,al	; clear bit
	jmp irq_enable_done

enable_slave_irq:
	in al,INTERRUPT_REQUEST_S
	and byte al,ah
	out INTERRUPT_REQUEST_S,al

irq_enable_done:
    ;push eax ;  RGDDEBUG
    ;call print_reg ; RGDEBUG
    ;add esp,4 ; RGDDEBUG
    sti ; RGDDEBUG
	popf				; restore flags
	pop dword ecx			; restore ecx
	pop dword edx			; restore edx
	ret

;
;	irq_save_i8259_state:-
;		Allows caller to store the control words
;	of both master and slave interrupt controllers.
;	This is primarily used for probing.
;
section .data
;dd MASTER_MASK 0
; MASTER_MASK times 1 dd 0
MASTER_MASK: dd 0
;dd SLAVE_MASK  0
; SLAVE_MASK times 1 dd 0 ; This creates a bad binary, C thinks SLAVE_MASK is a function.
SLAVE_MASK: dd 0
section .text
C_ENTRY irq_save_i8259_state
	xor eax,eax
	in al,INTERRUPT_REQUEST
	mov [MASTER_MASK],eax
	xor eax,eax
	in al,INTERRUPT_REQUEST_S
	mov [SLAVE_MASK],eax
	ret

;
;	irq_restore_i8259_state:-
;		Allows caller to restore the control
;	words of both master and slave interrupt controllers.
;
C_ENTRY irq_restore_i8259_state
	mov eax,[MASTER_MASK]
	out INTERRUPT_REQUEST,al
	mov eax,[SLAVE_MASK]
	out INTERRUPT_REQUEST_S,al
	ret

;	irq_i8259_enable_all:
;		Allows caller to enable all request lines.
;
C_ENTRY irq_i8259_enable_all
	mov al,00000000b
	out INTERRUPT_REQUEST,al
	out INTERRUPT_REQUEST_S,al
	ret

;
;	int_master_ctl:-
;
;		This co-routine is used by irq's 0 - 7
;	and accesses the master i8259 controller.
;	It takes only one argument, the integer value
;	of the irq. int_master_ctl reads from the IRQ port,
;	masks off the given IRQ, re-enables the master controller,
;	then enables interrupts. It then calls the approperiate 
;	handler with the IRQ number passed as a parameter on the stack.  
;	When the handler returns, we determine if we need to 
;	re-enable the IRQ by examining its return value in the eax register.
;	If eax is not zero, we re-enable the IRQ.
;
;	The INTERRUPT_COUNTER is used to record the level of
; 	nested interrupts that occur.  If we are executing
;	nested, then we do not execute any of our deferred
;	interrupts.
;
int_master_ctl:
	; irq_[00-07] co-routines push the irq # on the stack
	CTX_SAVE					; save context
	inc dword [INTERRUPT_COUNTER]; count nesting level
	mov eax,[__TMP__ + esp]	; eax now has the irq number
	mov ebx,eax					; ebx:ecx have a copy of the irq number
	mov ecx,eax
	xor eax,eax
	in al,INTERRUPT_REQUEST		; get map of irq's disabled from i8259
	bts ax,cx					; set bit corresponding to the called irq in the mask
	out INTERRUPT_REQUEST,al	; disable the irq
	mov al,ENABLE				; set enable bit
	out INTERRUPT_MASTER,al		; master re-enabled
	;sti	; enable interrupts allowing nested interrupt handling
    mov ebx,[__TMP__ + esp]

	push ebx					; pass irq number as an argument to the handler
	shl ebx,2					; multiply irq number by 4 bytes, to obtain offset
	add ebx,irq_dispatch_tab	; address of the handler
    cmp dword ebx,0
    jz error_handler
	call [ebx]					; call the handler
    add esp,4
	cli						; disable interrupts
	cmp dword eax,0		; did the handler return true to re-enable the irq ?
	jz common_handler_return
        push ecx
        call enable_irq
        add esp,4
	common_handler_return:
	sti						; re-enable interrupts
	dec dword [INTERRUPT_COUNTER]			; no longer handling this interrupt
	cmp dword [INTERRUPT_COUNTER],0			; should we handle def_int's ?
	;jne .restore_regs
		;.while:
		;	cmp dword [DEFINT_PENDING],0
		;	je .done
		;	call defint_exec
		;	jmp .while
	;	.done:
	.restore_regs:
		CTX_RESTORE				; restore context
		add esp,4				; restore stack
		iretd  ; out of interrupt handling

;
;	int_slave_ctl:-
;
;		This co-routine is used by irq's 8 - 15
;	and accesses the master/slave i8259 controllers.
;	It takes only one argument, the integer value
;	of the irq. int_slave_ctl reads from the IRQ port,
;	masks off the given IRQ, re-enables the master controller,
;	then the slave controller, then enables interrupts.
;	It then calls the approperiate handler with the IRQ
;	number passed as a parameter on the stack.  
;	When the handler returns, we determine if we need to 
;	re-enable the IRQ by examining its return value in the eax register.
;	If eax is not zero, we re-enable the IRQ.
;
;	The INTERRUPT_COUNTER is used to record the level of
; 	nested interrupts that occur.  If we are executing
;	nested, then we do not execute any of our deferred
;	interrupts.
;
int_slave_ctl:
							; irq_[08-15] co-routines pushed the irq # on the stack
	CTX_SAVE					; save context
	inc dword [INTERRUPT_COUNTER]			; count nesting level
	xchg eax,[__TMP__ + esp]			; eax now has the irq number
	mov ebx,eax					; ebx:ecx have a copy of the irq number
	mov ecx,eax
	xor eax,eax
	add ebx,8					; correct IRQ number
	in al,INTERRUPT_REQUEST_S			; get map of irq's disabled from i8259
	bts ax,cx					; set bit corresponding to the called irq in the mask
	out INTERRUPT_REQUEST_S,al			; disable the irq
	mov al,ENABLE					; set enable bit
	out INTERRUPT_MASTER,al				; master re-enabled
	NOP						; delay the controller
	out INTERRUPT_SLAVE,al				; slave  re-enabled
	;sti						; enable interrupts for nested interrupt handling
	push ebx					; pass irq number as an argument to the handler
	shl ebx,2					; multiply irq number by 4 bytes, to obtain offset
	add ebx,irq_dispatch_tab			; address of the handler
	call [ebx]					; call the handler
    add esp,4
	cli						; disable interrupts
	cmp dword eax,0					; did the handler return true to re-enable the irq ?
	jz common_handler_return
        push ecx
        call enable_irq
        add esp,4
	jmp common_handler_return

;
;	The following co-routines push the IRQ number
;	on the stack and jmp to the approperiate handler
;	block. When an interrupt occurs,
;	the hardware uses the IDT table entry to envoke
;	the handlers defined below.
;	The handlers common code, implemented in the above
;	co-routines acknowledge the i8259 controller, then
;	call a driver entry routine stored in irq_dispatch_tab
;	to start the OS handling of the interrupt.
;	OX uses a transparent mechanism for implementing
;	shared and exclusive IRQs and does not differentiate
;	from fast or slow IRQs ( CPUs are too fast these
;	days, and it makes for a cleaner design ).
;	The addresses of these entry points are installed
;	in the IDT by the init code in protected_mode.c.

;
;	[0-7]  are handled by the master interrupt controller
;
C_ENTRY	irq_00
	push dword 0
	jmp int_master_ctl

C_ENTRY	irq_01
    ;push eax ;  RGDDEBUG
    ;call print_reg ; RGDEBUG
    ;add esp,4 ; RGDDEBUG
    ;iretd ; RGDDEBUG
	push dword 1
	jmp int_master_ctl

C_ENTRY	irq_02
	push dword 2
	jmp int_master_ctl

C_ENTRY	irq_03
	push dword 3
	jmp int_master_ctl

C_ENTRY	irq_04
	push dword 4
	jmp int_master_ctl

C_ENTRY	irq_05
	push dword 5
	jmp int_master_ctl

C_ENTRY	irq_06
	push dword 6
	jmp int_master_ctl

C_ENTRY	irq_07
	push dword 7
	jmp int_master_ctl

;
;       [8-15] are handled by the slave  interrupt controller
;
C_ENTRY	irq_08
	push dword 0
	jmp int_slave_ctl

C_ENTRY	irq_09
	push dword 1
	jmp int_slave_ctl

C_ENTRY	irq_10
	push dword 2
	jmp int_slave_ctl

C_ENTRY	irq_11
	push dword 3
	jmp int_slave_ctl

C_ENTRY	irq_12
	push dword 4
	jmp int_slave_ctl

C_ENTRY	irq_13
	push dword 5
	jmp int_slave_ctl

C_ENTRY	irq_14
	push dword 6
	jmp int_slave_ctl

C_ENTRY	irq_15
	push dword 7
	jmp int_slave_ctl

C_ENTRY error_handler
    push 1313
    call print_reg
    hlt
;
;	EOF
;
