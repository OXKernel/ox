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
	.file	"piotest.c"
	.intel_syntax noprefix
	.text
	.globl	get_drive_head
	.type	get_drive_head, @function
get_drive_head:
.LFB0:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	sub	esp, 24
	mov	edx, DWORD PTR [ebp+8]
	mov	eax, DWORD PTR [ebp+12]
	mov	BYTE PTR [ebp-20], dl
	mov	BYTE PTR [ebp-24], al
	mov	BYTE PTR [ebp-1], -96
	movzx	eax, BYTE PTR [ebp-20]
	sal	eax, 4
	mov	edx, eax
	movzx	eax, BYTE PTR [ebp-1]
	or	eax, edx
	mov	BYTE PTR [ebp-1], al
	movzx	eax, BYTE PTR [ebp-24]
	mov	edx, eax
	and	edx, -16
	movzx	eax, BYTE PTR [ebp-1]
	or	eax, edx
	mov	BYTE PTR [ebp-1], al
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	get_drive_head, .-get_drive_head
	.globl	read_disk
	.type	read_disk, @function
read_disk:
.LFB1:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	push	ebx
	sub	esp, 32
	mov	ebx, DWORD PTR [ebp+8]
	.cfi_offset 3, -12
	mov	ecx, DWORD PTR [ebp+12]
	mov	edx, DWORD PTR [ebp+16]
	mov	eax, DWORD PTR [ebp+20]
	mov	BYTE PTR [ebp-24], bl
	mov	BYTE PTR [ebp-28], cl
	mov	BYTE PTR [ebp-32], dl
	mov	BYTE PTR [ebp-36], al
	movzx	eax, BYTE PTR [ebp-24]
	mov	BYTE PTR [ebp-5], al
	movzx	eax, BYTE PTR [ebp-28]
	add	BYTE PTR [ebp-5], al
	movzx	eax, BYTE PTR [ebp-32]
	add	BYTE PTR [ebp-5], al
	movzx	eax, BYTE PTR [ebp-36]
	add	BYTE PTR [ebp-5], al
	movzx	eax, BYTE PTR [ebp-5]
	add	esp, 32
	pop	ebx
	.cfi_restore 3
	pop	ebp
	.cfi_def_cfa 4, 4
	.cfi_restore 5
	ret
	.cfi_endproc
.LFE1:
	.size	read_disk, .-read_disk
	.globl	foo
	.type	foo, @function
foo:
.LFB2:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	sub	esp, 4
	mov	eax, DWORD PTR [ebp+8]
	mov	BYTE PTR [ebp-4], al
	movzx	eax, BYTE PTR [ebp-4]
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	foo, .-foo
	.section	.rodata
.LC0:
	.string	"error reading drive"
.LC1:
	.string	"error writing drive"
	.align 4
.LC2:
	.string	"BBK(bit 7) sector marked as bad by host"
	.align 4
.LC3:
	.string	"UNC(bit 6) uncorrectable data error"
.LC4:
	.string	"NID(bit 4) ID mark not found"
.LC5:
	.string	"ABT(bit 2) command abort"
.LC6:
	.string	"NTO(bit 1) track 0 not found"
	.align 4
.LC7:
	.string	"NDM(bit 0) data address mark not found"
	.text
	.globl	status_check
	.type	status_check, @function
status_check:
.LFB3:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	sub	esp, 56
	mov	edx, DWORD PTR [ebp+12]
	mov	eax, DWORD PTR [ebp+16]
	mov	BYTE PTR [ebp-28], dl
	mov	BYTE PTR [ebp-32], al
	mov	DWORD PTR [ebp-12], 0
	movzx	eax, BYTE PTR [ebp-32]
	and	eax, 1
	test	al, al
	je	.L5
	movzx	eax, BYTE PTR [ebp-32]
	test	al, al
	js	.L5
	cmp	DWORD PTR [ebp+8], 0
	jne	.L6
	mov	DWORD PTR [esp], OFFSET FLAT:.LC0
	call	puts
	jmp	.L7
.L6:
	cmp	DWORD PTR [ebp+8], 1
	jne	.L7
	mov	DWORD PTR [esp], OFFSET FLAT:.LC1
	call	puts
.L7:
	mov	DWORD PTR [ebp-12], 1
	movzx	eax, BYTE PTR [ebp-28]
	test	al, al
	jns	.L8
	mov	DWORD PTR [esp], OFFSET FLAT:.LC2
	call	puts
.L8:
	movzx	eax, BYTE PTR [ebp-28]
	and	eax, 64
	test	eax, eax
	je	.L9
	mov	DWORD PTR [esp], OFFSET FLAT:.LC3
	call	puts
.L9:
	movzx	eax, BYTE PTR [ebp-28]
	and	eax, 16
	test	eax, eax
	je	.L10
	mov	DWORD PTR [esp], OFFSET FLAT:.LC4
	call	puts
.L10:
	movzx	eax, BYTE PTR [ebp-28]
	and	eax, 4
	test	eax, eax
	je	.L11
	mov	DWORD PTR [esp], OFFSET FLAT:.LC5
	call	puts
.L11:
	movzx	eax, BYTE PTR [ebp-28]
	and	eax, 2
	test	eax, eax
	je	.L12
	mov	DWORD PTR [esp], OFFSET FLAT:.LC6
	call	puts
.L12:
	movzx	eax, BYTE PTR [ebp-28]
	and	eax, 1
	test	al, al
	je	.L5
	mov	DWORD PTR [esp], OFFSET FLAT:.LC7
	call	puts
.L5:
	mov	eax, DWORD PTR [ebp-12]
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE3:
	.size	status_check, .-status_check
	.section	.rodata
.LC8:
	.string	"%x\n"
.LC9:
	.string	"ptr=%x %x\n"
.LC10:
	.string	"status_check:: no error"
.LC11:
	.string	"bit_test:: error"
.LC12:
	.string	"bit_test:: no error"
	.text
	.globl	main
	.type	main, @function
main:
.LFB4:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	push	edi
	push	ebx
	and	esp, -16
	sub	esp, 560
	mov	BYTE PTR [esp+559], 0
	mov	BYTE PTR [esp+558], 1
	lea	edx, [esp+46]
	mov	ebx, 512
	.cfi_offset 3, -16
	.cfi_offset 7, -12
	mov	eax, 0
	mov	ecx, edx
	and	ecx, 2
	test	ecx, ecx
	je	.L14
	mov	WORD PTR [edx], ax
	add	edx, 2
	sub	ebx, 2
.L14:
	mov	ecx, ebx
	shr	ecx, 2
	mov	edi, edx
	rep stosd
	mov	edx, edi
	mov	ecx, ebx
	and	ecx, 2
	test	ecx, ecx
	je	.L15
	mov	WORD PTR [edx], ax
	add	edx, 2
.L15:
	mov	ecx, ebx
	and	ecx, 1
	test	ecx, ecx
	je	.L16
	mov	BYTE PTR [edx], al
	add	edx, 1
.L16:
	mov	DWORD PTR [esp+4], 0
	mov	DWORD PTR [esp], 0
	call	get_drive_head
	movzx	eax, al
	mov	DWORD PTR [esp+4], eax
	mov	DWORD PTR [esp], OFFSET FLAT:.LC8
	call	printf
	mov	DWORD PTR [esp], 171
	call	test
	mov	DWORD PTR [esp+4], eax
	mov	DWORD PTR [esp], OFFSET FLAT:.LC8
	call	printf
	mov	DWORD PTR [esp], 186
	call	test1
	mov	DWORD PTR [esp+4], eax
	mov	DWORD PTR [esp], OFFSET FLAT:.LC8
	call	printf
	mov	DWORD PTR [esp], 202
	call	test2
	mov	DWORD PTR [esp+4], eax
	mov	DWORD PTR [esp], OFFSET FLAT:.LC8
	call	printf
	lea	eax, [esp+46]
	mov	DWORD PTR [esp+16], eax
	mov	DWORD PTR [esp+12], 13
	mov	DWORD PTR [esp+8], 12
	mov	DWORD PTR [esp+4], 11
	mov	DWORD PTR [esp], 10
	call	pio_read
	mov	DWORD PTR [esp+8], eax
	lea	eax, [esp+46]
	mov	DWORD PTR [esp+4], eax
	mov	DWORD PTR [esp], OFFSET FLAT:.LC9
	call	printf
	mov	DWORD PTR [esp+4], 16
	mov	DWORD PTR [esp], OFFSET FLAT:.LC8
	call	printf
	mov	DWORD PTR [esp+4], 20
	mov	DWORD PTR [esp], OFFSET FLAT:.LC8
	call	printf
	mov	BYTE PTR [esp+559], 1
	movzx	edx, BYTE PTR [esp+559]
	movzx	eax, BYTE PTR [esp+558]
	mov	DWORD PTR [esp+8], edx
	mov	DWORD PTR [esp+4], eax
	mov	DWORD PTR [esp], 0
	call	status_check
	test	eax, eax
	jne	.L17
	mov	DWORD PTR [esp], OFFSET FLAT:.LC10
	call	puts
.L17:
	movzx	eax, BYTE PTR [esp+559]
	mov	DWORD PTR [esp], eax
	call	bit_test
	cmp	eax, 1
	jne	.L18
	mov	DWORD PTR [esp], OFFSET FLAT:.LC11
	call	puts
	jmp	.L19
.L18:
	mov	DWORD PTR [esp], OFFSET FLAT:.LC12
	call	puts
.L19:
	mov	eax, 0
	lea	esp, [ebp-8]
	pop	ebx
	.cfi_restore 3
	pop	edi
	.cfi_restore 7
	pop	ebp
	.cfi_def_cfa 4, 4
	.cfi_restore 5
	ret
	.cfi_endproc
.LFE4:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.6.3 20120306 (Red Hat 4.6.3-2)"
	.section	.note.GNU-stack,"",@progbits
