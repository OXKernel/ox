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
/********************************************************************
 * Copyright (C) Roger G. Doss.  All Rights Reserved.
 ********************************************************************/
#include <elf.h>
#include <stdio.h>

int
main ( int argc, char **argv )
{
	FILE *fp = NULL;		/* file pointer */
	Elf32_Ehdr *header = NULL;	/* 32bit Elf header */
	char *data = NULL;		/* data pointer */
	Elf32_Phdr *p = NULL;		/* program header */

	if(argc != 2) {
		fprintf(stderr,"syntax %s file_name\n",argv[0]);	
		exit(-1);
	}

	if(!(fp=fopen(argv[1],"r"))) {
		fprintf(stderr,"error opening file %s\n",argv[1]);
		exit(-2);
	}

	data = (char *)malloc(sizeof(Elf32_Ehdr));

	/* read ELF header */
	fread(data,sizeof(Elf32_Ehdr),1,fp);
	header = (Elf32_Ehdr *)data;

	/* free to access the header linearly (ASM)
	 * or structurely (C)
	 */
	/* MUST first cast to pointer of approperiate type/size
	 * then dereference, otherwise, we are only accessing
	 * a char.
	 */
    printf("e_entry %X\n", header->e_entry);
	printf("testing offset of e_type\n");
	printf("%X\n",header->e_type);
	printf("%X\n",*(Elf32_Half *)(data + 0x10));

	printf("testing offset of e_machine\n");
	printf("%X\n",header->e_machine);
	printf("%X\n",*(Elf32_Half *)(data + 0x12));

	printf("testing offset of e_version\n");
	printf("%X\n",header->e_version);
	printf("%X\n",*(Elf32_Word *)(data + 0x14));

	printf("testing offset of e_entry\n");
	printf("%X\n",header->e_entry);
	printf("%X\n",*(Elf32_Addr *)(data + 0x18));

	printf("testing offset of e_phoff\n");
	printf("%X\n",header->e_phoff);
	printf("%X\n",*(Elf32_Off *)(data + 0x1C));

	printf("testing offset of e_phnum\n");
	printf("%X\n",header->e_phnum);
	printf("%X\n",*(Elf32_Half *)(data + 0x2C));
	
	printf("testing offset of e_phentsize\n");
	printf("%X\n",header->e_phentsize);
	printf("%X\n",*(Elf32_Half *)(data + 0x2A));

	
	printf("testing pht location %X\n",
		(((header->e_phnum-1) * header->e_phentsize) +
		header->e_phoff + 0x14000 ));

	p=(Elf32_Phdr *)data + 0x54;
	printf("testing Elf32_Phdr->p_type %X\n",
			p->p_type);
DONE:
	fclose(fp);
	fp = NULL;

}/* main */
