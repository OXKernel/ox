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
/************************************************************* 
 * Copyright Roger George Doss.
 *************************************************************
 *
 * @module elf.h
 *
 *         Executable Linking File Format
 */
#ifndef _ELF_H
#define _ELF_H 1
#ifdef __cplusplus
 extern "C" {
#endif

/* ELF types
 */
typedef unsigned long   Elf32_Addr;
typedef unsigned short  Elf32_Half;
typedef unsigned long   Elf32_Off;
typedef long            Elf32_Sword;
typedef unsigned long   Elf32_Word;

#define EI_NIDENT 16

/* ELF Header
 */
typedef struct {
	unsigned char   e_ident[EI_NIDENT]; /* identification      */
	Elf32_Half      e_type;      /* ELF Type                   */
	Elf32_Half      e_machine;   /* ELF Archeticture           */
	Elf32_Word      e_version;   /* ELF version                */
	Elf32_Addr      e_entry;     /* process entry point        */
	Elf32_Off       e_phoff;     /* program hdr (phdr) offset  */ 
	Elf32_Off       e_shoff;     /* section hdr offset         */
	Elf32_Word      e_flags;     /* cpu specific flags         */
	Elf32_Half      e_ehsize;    /* ELF hdr size in bytes      */
	Elf32_Half      e_phentsize; /* phdr entry size, all equal */
	Elf32_Half      e_phnum;     /* number of entries in phdr  */
	Elf32_Half      e_shentsize; /* section hdr size           */
	Elf32_Half      e_shnum;     /* section hdr number         */
	Elf32_Half      e_shstrndx;  /* section name string table  */
}Elf32_Ehdr;

/* e_ident:
 * identification, marks initial bytes of the file
 * as an ELF file. ie, e_ident[EI_MAG0] == 0x7f, etc.
 */
#define EI_MAG0      0   /* 0x7f         */
#define EI_MAG1      1   /* 'E'          */
#define EI_MAG2      2   /* 'L'          */
#define EI_MAG3      3   /* 'F'          */
#define EI_CLASS     4   /* elf classes  */
#define EI_DATA      5   /* elf data     */
#define EI_VERSION   6   /* EV_CURRENT   */
#define EI_PAD       7   /* reserved     */

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

/* elf class */
#define ELFCLASSNONE 0   /* invalid       */
#define ELFCLASS32   1   /* 32bit objects */
#define ELFCLASS64   2   /* 64bit objects */

/* elf data */
#define ELFDATANONE  0   /* invalid       */
#define ELFDATA2LSB  1   /* 2's complement,lsb in low mem  */
#define ELFDATA2MSB  2   /* 2's complement,msb in high mem */

/* e_type: 
 * object file type
 */
#define ET_NONE     0  /* invalid            */
#define ET_REL      1  /* relocatable file   */
#define ET_EXEC     2  /* executable         */
#define ET_DYN      3  /* shared object file */
#define ET_CORE     4  /* core file          */
#define ET_LOROC    5  /* processor specific */
#define ET_HIPROC   6  /* processor specific */

/* e_machine:
 * machine architecture
 */
#define EM_NONE     0  /* invalid        */
#define EM_M32      1  /* At&t WE32100   */
#define EM_SPARC    2  /* Sparc          */
#define EM_386      3  /* Intel 386      */
#define EM_68K      4  /* Motorola 68000 */
#define EM_88K      5  /* Motorola 88000 */
#define EM_860      7  /* Intel 80860    */
#define EM_MIPS     8  /* Mips RS3000    */

/* e_version:
 * ELF version
 */
#define EV_NONE     0  /* invalid        */
#define EV_CURRENT  1  /* current        */

/* section header table:
 * Allows us to locate all sections within the file
 * and is an array of Elf32_Shdr structures.
 * The ELF headers field e_shoff gives the byte
 * offset from the begining of the file to the table.
 * e_shnum tells how many entries there are, and
 * e_shentsize gives the size of each section.
 * An index is a subscript into the array.
 */
#define SHN_UNDEF     0      /* undefined */
#define SHN_LORESERVE 0xff00 /* lower bound of reserved indexes */
#define SHN_LOPROC    0xff00 /* SHN_LOPROC - SHN_HIPROC cpu reserved */
#define SHN_HIPROC    0xff1f
#define SHN_ABS       0xfff1 /* absolute */
#define SHN_COMMON    0xfff2 /* common   */
#define SHN_HIRESERVE 0xffff /* upper bound of reserved indexes */

typedef struct {
	Elf32_Word     sh_name;      /* index into section hdr string table */
	Elf32_Word     sh_type;      /* section type */
	Elf32_Word     sh_flags;     /* section flags */
	Elf32_Addr     sh_addr;      /* addr of 1st byte of this section */
	Elf32_Off      sh_offset;    /* offset to start of section */
	Elf32_Word     sh_size;      /* section size */
	Elf32_Word     sh_link;      /* section hdr table index link */
	Elf32_Word     sh_info;      /* extra information */
	Elf32_Word     sh_addralign; /* alignment,0/1 mean no alignment */
	Elf32_Word     sh_entsize;   /* size of fixed size tables */
}Elf32_Shdr;

/* sh_type:
 * section types
 */
#define SHT_NULL      0     /* inactive */
#define SHT_PROGBITS  1     /* program defined information */
#define SHT_SYMTAB    2     /* symbol table */
#define SHT_STRTAB    3     /* string table */
#define SHT_RELA      4     /* relocation entries, explicit addends */
#define SHT_HASH      5     /* hash table for dynamic lookup */
#define SHT_DYNAMIC   6     /* dynamic linking */
#define SHT_NOTE      7     /* information special to file */
#define SHT_NOBITS    8     /* contains no bits in file, sh_size N/A */
#define SHT_REL       9     /* relocation entries, no explicit addends */
#define SHT_SHLIB    10     /* reserved */
#define SHT_DYNSYM   11     
#define SHT_LOPROC   0x70000000 /* SHT_LOPROC- SHT_HIPROC cpu reserved */
#define SHT_HIPROC   0x7fffffff
#define SHT_LOUSER   0x80000000 /* lower bound of reserved range for app's */
#define SHT_HIUSER   0xffffffff /* upper bound of reserved range for app's */

/* sh_flags:
 * section flags
 */
#define SHF_WRITE       0x1         /* writable section */
#define SHF_ALLOC       0x2         /* occupies memory  */
#define SHF_EXECINSTR   0x4         /* executable code  */
#define SHF_MASKPROC    0xf0000000  /* cpu reserved     */

/* symbol table:
 * contains symbolic information needed to locate and relocate
 * a program's symbolic definitions and references. A symbol
 * table index is a subscript into this array. Index 0 
 * designates the first entry in the table and serves as 
 * the undefined symbol index.
 */
#define STN_UNDEF       0

typedef struct {
	Elf32_Word    st_name;    /* index into symbol string table */
	Elf32_Addr    st_value;   /* value of associated symbol */
	Elf32_Word    st_size;    /* size */
	unsigned char st_info;    /* type and binding for symbol */
	unsigned char st_other;   /* 0, undefined */
	Elf32_Half    st_shndx;   /* section symbol defined in relation to */
}Elf32_Sym;

/* st_info:
 * information about a symbol, to be manipulated
 * by the macros below
 */
#define ELF32_ST_BIND(i)  ((i)>>4)
#define ELF32_ST_TYPE(i)  ((i)&0xf)
#define ELF32_ST_INFO(i)  (((b)<<4)+((t)&0xf))

/* st_shndx: 
 * every symbol table entry is defined in relation 
 * to some other section; st_shndx holds relevant
 * section header table index, special values are below
 */
typedef enum {
 	STB_LOCAL   =   0,  /* local, not visible outside of module */
	STB_GLOBAL  =   1,  /* visible to all objects */
	STB_WEAK    =   2,  /* like global, but lowe precedence */
	STB_LOPROC  =  13,  /* cpu reserved */
	STB_HIPROC  =  15
}ELF32_ST_BIND;

typedef enum {
	STT_NOTYPE  = 0,   /* type not specified */
	STT_OBJECT  = 1,   /* datatype */
	STT_FUNC    = 2,   /* function */
	STT_SECTION = 3,   /* section, STB_LOCAL binding */
	STT_FILE    = 4,   /* file, STB_LOCAL, SHN_ABS */
	STT_LOPROC  = 13,  /* cpu reserved */
	STT_HIPROC  = 15
}ELF32_ST_TYPE;

typedef struct {
	Elf32_Addr  r_offset; /* relative to section/V address (exe/shlib) */
	Elf32_Word  r_info;   /* symbol table index and type of rel. */
}Elf32_Rel;

typedef struct {
	Elf32_Addr  r_offset;
	Elf32_Word  r_info;
	Elf32_Sword r_addend; /* constant addend */
}Elf32_Rela;

/* r_info:
 */
#define ELF32_R_SYM(i)     ((i)>>8)
#define ELF32_R_TYPE(i)    ((unsigned char)(i))
#define ELF32_R_INFO(s,t)  (((s)<<8)+(unsigned char)(t))

/* Relocation types:
 * A   := addend
 * B   := base address at which file is loaded, zero for virtual
 *        address base, physical will differ
 * G   := offset into the global offset table where the address
 *        of the relocation entry's symbol table will reside 
 *        during execution
 * GOT := address of global offset table
 * L   := section offset or address of PLT
 * P   := section offset or address of unit being relocated
 * S   := value of symbol whose index resides in the relocation entry
 */
#define  R_386_NONE     0   /* none */
#define  R_386_32       1   /* S + A */
#define  R_386_PC32     2   /* S + A - P */
#define  R_386_GOT32    3   /* G + A - P */
#define  R_386_PLT32    4   /* L + A - P */
#define  R_386_COPY     5   /* none */
#define  R_386_GLOB_DAT 6   /* S */
#define  R_386_JMP_SLOT 7   /* S */
#define  R_386_RELATIVE 8   /* B + A */
#define  R_386_GOTOFF   9   /* S + A - GOT */
#define  R_386_GOTPC   10   /* GOT - A - P */

typedef struct {
	Elf32_Word    p_type;    /* type of segment */
	Elf32_Off     p_offset;  /* offset to segment */
	Elf32_Addr    p_vaddr;   /* virtual address */
	Elf32_Addr    p_paddr;   /* phyisical address */
	Elf32_Word    p_filesz;  /* number of bytes in file */
	Elf32_Word    p_memsz;   /* number of bytes in memory */
	Elf32_Word    p_flags;   /* flags */
	Elf32_Word    p_align;   /* alignment */
}Elf32_Phdr;

/* p_type:
 */
#define PT_NULL    0             /* null */
#define PT_LOAD    1             /* loadable segment */
#define PT_DYNAMIC 2             /* dynamic linking */
#define PT_INTERP  3             /* interpreter */
#define PT_NOTE    4             /* auxillary information */
#define PT_SHLIB   5             /* reserved */
#define PT_PHDR    6             /* program header location */
#define PT_LOPROC  0x70000000    /* cpu reserved */
#define PT_HIPROC  0x7fffffff

/*
 * p_flags:
 *
 * None => 0 => All access denied
 * PF_X => 1 => Execute
 * PF_W => 2 => Write
 * PF_R => 4 => Read
 *
 * PF_W + PF_X => 3 => Write, Execute
 * PF_R + PF_X => 5 => Read,  Execute
 * PF_R + PF_W => 6 => Read,  Write
 *
 * PF_R + PF_W + PF_X => 7 => Read, Write, Execute
 *
 * Code segments are usually PF_R + PF_X
 * Data segments are usually PF_R + PF_W + PF_X
 *
 */
#define PF_X        0x1             /* Execute      */
#define PF_W        0x2             /* Write        */
#define PF_R        0x4             /* Read         */
#define PF_MASKPROC 0xf0000000      /* Unspecified  */

typedef struct {
	Elf32_Sword   d_tag;
	union {
		Elf32_Word  d_val;  /* values */
		Elf32_Addr  d_ptr;  /* virtual addresses */
	}d_un;
}Elf32_Dyn;

extern Elf32_Dyn_Dynamic[];

#define DT_NULL      0     /* ignored, mandatory, mandatory */
#define DT_NEEDED    1     /* d_val, optional, optional */
#define DT_PLTRELSZ  2     /* d_ptr, optional, optional */
#define DT_PLTGOT    3     /* d_ptr, optional, optional */
#define DT_HASH      4     /* d_ptr, mandatory, mandatory */
#define DT_STRTAB    5     /* d_ptr, mandatory, mandatory */
#define DT_SYMTAB    6     /* d_ptr, mandatory, mandatory */
#define DT_RELA      7     /* d_ptr, mandatory, optional  */
#define DT_RELASZ    8     /* d_val, mandatory, optional  */
#define DT_RELAENT   9     /* d_val, mandatory, optional  */
#define DT_STRSZ    10     /* d_val, mandatory, mandatory */
#define DT_SYMENT   11     /* d_val, mandatory, mandatory */
#define DT_INIT     12     /* d_ptr, optional, optional */
#define DT_FINI     13     /* d_ptr, optional, optional */
#define DT_SONAME   14     /* d_val, ignored,  optional */
#define DT_RPATH    15     /* d_val, optional, optional */
#define DT_SYMBOLIC 16     /* ignored, ignored, optional */
#define DT_REL      17     /* d_ptr, mandatory, optional */
#define DT_RELSZ    18     /* d_val, mandatory, optional */
#define DT_RELENT   19     /* d_val, mandatory, optional */
#define DT_PLTREL   20     /* d_val, optional, optional */
#define DT_DEBUG    21     /* d_ptr, optional, ignored */
#define DT_TEXTREL  22     /* ignored, optional, optional */
#define DT_JMPREL   23     /* d_ptr, optional, optional */
#define DT_LOPROC   0x70000000  /* unspecified, unspecified, unspecified */
#define DT_HIPROC   0x7fffffff  /* unspecified, unspecified, unspecified */

extern Elf32_Addr   _GLOBAL_OFFSET_TABLE[];

unsigned long
elf_hash(const unsigned char *name)
{
	unsigned long h=0,g;
	while(*name) {
		h=(h<<4)+ *name++;
		if(g=h &0xf0000000)
			h ^= g >> 24;
		h &= ~g;
	}
	return h;
}

#ifdef __cplusplus
 }
#endif
#endif /* _ELF_H */
