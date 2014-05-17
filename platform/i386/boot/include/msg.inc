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
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copyright (C) Roger George Doss. All Rights Reserved.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; @module
;	msg.s
; @desription
;	messages used by boot loader
; @author
;	Roger G. Doss
;
%ifndef _MSG_INC
%define _MSG_INC

;
; defines
;
%define END_STRING 	13,10,0		; \r\n

;
; messages used in booting
;
%macro _NEW_LINE 0
	NEW_LINE db END_STRING
	%endmacro

%macro _COPYING_MSG 0
	COPYING_MSG	db 'Copyright (C) 2013. Roger G. Doss. All Rights Reserved',END_STRING
	%endmacro

%macro _REBOOT_MSG 0
	REBOOT_MSG	db 'Press any key to reboot',END_STRING
	%endmacro

%macro _CPU_CHECK_MSG 0
	CPU_CHECK_MSG	db 'Testing for x386 microprocessor...',0
	%endmacro

%macro _CPU_8688_MSG 0
	CPU_8688_MSG	db 'ERROR detected 8086 or 8088',END_STRING
	%endmacro

%macro _CPU_286_MSG 0
	CPU_286_MSG	db 'ERROR detected x286',END_STRING
	%endmacro
	
%macro _CPU_ERROR_MSG 0
	CPU_ERROR_MSG	db 'invalid microprocessor',END_STRING
	%endmacro

%macro _CPU_OK_MSG 0
	CPU_OK_MSG	db 'Detected',END_STRING
	%endmacro

%macro _BOOT_MSG 0
	BOOT_MSG	db 'Loading OX Stage 2 Loader...',END_STRING
	%endmacro

%macro _OX_MSG 0
	OX_MSG		db 'Loading OX Kernel...',END_STRING
	%endmacro

%macro _BOUNDARY_MSG 0
	BOUNDARY_MSG	db '64K boundary access...',END_STRING
	%endmacro

%macro _READ_ERR_MSG 0
	READ_ERR_MSG 	db 'ERROR could not read sector...',END_STRING
	%endmacro

%macro _HALT_MSG 0
    HALT_MSG db 'HALT...',END_STRING
    %endmacro

%macro _ENTRY_MSG 0
    ENTRY_MSG db 'Kernel entry point :=',END_STRING
    %endmacro

%macro _ERROR_MSG 0
	ERROR_MSG	db 'Generic Error...',END_STRING
	%endmacro

%macro _BOOT_ERR_MSG 0
	BOOT_ERR_MSG	db 'ERROR could not load stage 2 loader...',END_STRING
	%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Exec ELF Error messages, coded to save space.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%macro _EXEC_ELF_MSG  0
	EXEC_ELF_MSG	db 'ERROR invalid ELF ',0
	%endmacro

%macro _EXEC_ELF_MSG0 0
	EXEC_ELF_MSG0	db 'signature',0
	%endmacro

%macro _EXEC_ELF_MSG1 0
	EXEC_ELF_MSG1	db 'class',0
	%endmacro

%macro _EXEC_ELF_MSG2 0
	EXEC_ELF_MSG2	db 'byte order',0
	%endmacro

%macro _EXEC_ELF_MSG3 0
	EXEC_ELF_MSG3	db 'version',0
	%endmacro

%macro _EXEC_ELF_MSG4 0
	EXEC_ELF_MSG4	db 'file not executable',0
	%endmacro

%macro _EXEC_ELF_MSGN 0
	EXEC_ELF_MSGN	db ' ...',END_STRING
	%endmacro

%endif ; _MSG_INC
