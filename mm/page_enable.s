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
; @file:
;   page_enable.s
;
; @description:
;   NASM logic for setting up the kernels page dir
;   at address 0. See mem_init which calls this
;   routine.
;
%include "../platform/i386/arch/asm_core/common/macros.inc"

; void page_enable(void *kernel_page_dir)
C_ENTRY page_enable
        cli
        mov dword eax,[esp + 0x4] ; Get first parameter on stack.
        mov cr3,eax ; Load address of page dir into cr3
        mov eax,cr0
        or eax,0x80000000   ; enable paging bit
        ;or eax,0x80008000   ; enable paging and write protection (bits 31 and 16 set)
        mov cr0,eax         ; Set PG bit of CR0
        jmp short flush     ; Flush out the cache
flush:
        sti
        ret

; void page_flush_tlb(void *virtual_addr)
; NOTE: This function is for x486 and above
;       for x386 we need page_flush_tlb_386
C_ENTRY page_flush_tlb
    mov dword eax,[esp + 0x4] ; Get first parameter on stack.
    invlpg [eax]
    ret

; void page_flush_tlb_386(void *virtual_addr)
C_ENTRY page_flush_tlb_386
    mov dword eax,[esp + 0x4] ; Get first parameter on stack.
    mov cr3,eax
    ret
