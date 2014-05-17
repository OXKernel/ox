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
//
// @file:
// 	exec.c
//
// @author:
// 	Dr. Roger G. Doss, PhD
//
// @description:
// 	Rudimentary exec facility with support for
// 	ELF. Executables must be static linked
// 	and fully position independent.
//
// @todo:
// 	- Integrate with the rest of the kernel.
// 	- Certain code has to be added to allocate
// 	  memory from ox, originally, its all read/write/exec
// 	  then we go back and set it read-only as described
// 	  in the ELF.
//  - Make the calls to memory management from ox.
//  - Make the calls to make read-only.
//  - Integrate with struct process.
//
#ifdef _TEST_EXEC
#include <stdio.h>
#include <string.h>
#define kstat   stat
#define kopen   open
#define kread   read
#define kclose close
#define printk printf
#define MAX_PATH 256
#include "elf.h"
#include "exec.h"
#include <malloc.h>
#include <sys/mman.h>
void asm_disable_interrupt() {}
void asm_enable_interrupt()  {}
#else // In kernel
#include <elf.h>
#include <ox/error_rpt.h>
#include <ox/exec.h>
#include <ox/fs.h>
#include <ox/fs/fs_syscalls.h>
#include <ox/fs/compat.h>
#include <sys/signal.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <ox/types.h>
#include <ox/defs.h>
#include <platform/protected_mode_defs.h>
#include <platform/segment.h>
#include <platform/tss.h>
#include <ox/mm/page.h>
#include <ox/mm/malloc.h>
#include <ox/process.h>
#include <ox/scheduler.h>
#include <platform/asm_core/util.h>
#endif

#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>


#define EXEC_MAX_RECURSION 16

/*
 * file_load:
 *
 * load a file from disk to memory.
 * Return 1 if success.
 * Return 0 if there was an error.
 *
 */
int file_load(const char *filename, char **image, unsigned int *image_size)
{
    struct stat stbuf={0};
    int fd = -1;
    char *mem = NULL;
    if(!filename || !image || !image_size) {
        printk("file_load:: null param\n");
        return 0;
    }
    if(kstat(filename,&stbuf) == -1) {
        printk("file_load:: error stat'ing file [%s]\n",filename);
        return 0;
    }
    *image_size = stbuf.st_size;
    mem = (char *)malloc(stbuf.st_size + 1);
    if(mem == NULL) {
        printk("file_load:: error allocating memory\n");
        return 0;
    }
    fd = kopen(filename,O_RDONLY);
    if(fd == -1) {
        printk("file_load:: error loading file\n");
        free((void *)mem);
        return 0;
    }
    if(kread(fd, mem, stbuf.st_size) != stbuf.st_size) {
        printk("file_load:: error reading file [%s]\n",filename);
        free((void *)mem);
        kclose(fd);
        return 0;
    }
    mem[stbuf.st_size]='\0';
    *image = mem;
    kclose(fd);
    return 1;

}/* file_load */

/*
 * file_unload:
 *
 * Free file image.
 *
 * Return 1 on success.
 * Return 0 on failure.
 *
 */
int file_unload(char **image)
{
    if(!image || !*image) {
        printk("file_unload:: error free'ing null pointer\n");
        return 0;
    }
    free((void *)*image);
    *image = 0;
    return 1;

}/* file_unload */

/*
 * is_image_valid:
 *
 * Assert we have a valid ELF image.
 *
 * Returns:
 *
 * 1 success
 * 0 fail
 *
 */
int is_image_valid(Elf32_Ehdr *hdr)
{
        if(!hdr) {
            printk("is_image_valid:: null header\n");
            return 0;
        }

        if((hdr->e_ident[EI_MAG0] != ELFMAG0) ||
           (hdr->e_ident[EI_MAG1] != ELFMAG1) ||
           (hdr->e_ident[EI_MAG2] != ELFMAG2) ||
           (hdr->e_ident[EI_MAG3] != ELFMAG3)) {
            printk("is_image_valid:: invalid ELF signature\n");
            return 0;
        }

        if(hdr->e_phoff == 0 || hdr->e_phnum == 0) {
                printk("is_image_valid:: e_phoff == 0 || e_phnum == 0\n");
                return 0;
        }

        if(hdr->e_phentsize != sizeof(Elf32_Phdr)) {
                printk("is_image_valid:: e_phentsize!=sizeof Elf32_Phdr\n");
                return 0;
        }

        // Is it an executable ?
        if (hdr->e_type != ET_EXEC) {
                printk("is_image_valid:: ELF not an executable\n");
                return 0;
        }

        // Is it for x86 ?
        if (hdr->e_machine != EM_386) {
                printk("is_image_valid:: run architecture (not x86)\n");
                return 0;
        }

        // Success.
        return 1;

}/* is_image_valid */

/*
 *   image_load:
 *
 *   Given an ELF file loaded in memory with given
 *   size, parse the file and convert it into an
 *   executable. Return the process start routine.
 *
 *   NOTES: Currently supports statically linked executables
 *   that are entirely position independent code (-fpic).
 *   This means that current linux gcc code must be compiled
 *   this way: 
 *
 * gcc -c -static -nostdlib -fpic code.c
 * ld -elf_i386 test5.o -o code.exe -static -Ttext 0x100000
 *
 *   The relocation to address 0x100000 is arbitrary, the process 
 *   will be relocated to the address the kernel sees fit.
 *   The entry point to the process will be a C function
 *   called int lf_i386() and it may call _start().
 *   In this environment, the code for _start needs to be developed
 *   and is not the standard C start routine.
 *   Compiling regular gcc code as pic or pie implies that
 *   it will be dynamically linked which this exec function
 *   does not currently support.
 *
 */
void *image_load (char *elf_start, 
	          unsigned int size, 
		  unsigned char **exec_mem, 
		  unsigned int *exec_size)
{
        Elf32_Ehdr      *hdr    = NULL;
        Elf32_Phdr      *phdr   = NULL;
        unsigned char   *start  = NULL;
        Elf32_Addr      taddr   = 0;
        Elf32_Addr      offset  = 0;
        int i = 0;
        unsigned char *exec = NULL;
        Elf32_Addr      estart = 0;
        unsigned int    esize  = 0;

        hdr = (Elf32_Ehdr *) elf_start;

        if(!is_image_valid(hdr)) {
            printk("image_load:: invalid ELF image\n");
            return 0;
        }

        LINE();
        phdr = (Elf32_Phdr *)(elf_start + hdr->e_phoff);
        LINE();

        // Calculate the process size in memory.
        for(i=0; i < hdr->e_phnum; i++) {
                if(phdr[i].p_type != PT_LOAD) {
                        continue;
                }
                if(!estart) {
                    estart = phdr[i].p_paddr;
                }
		esize += ((phdr[i].p_paddr-estart) + phdr[i].p_memsz);
        }
        estart = 0;

#ifdef _TEST_EXEC
        exec = (unsigned char *)mmap(NULL, esize, 
			  PROT_READ | PROT_WRITE | PROT_EXEC,
                          MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
#else // Kernel
        exec = (unsigned char *)malloc(esize);
#endif

        if(!exec) {
            printk("image_load:: error allocating memory\n");
            return 0;
        }

        // Start with clean memory.
        memset(exec,0x0,esize);

        LINE();
        phdr = (Elf32_Phdr *)(elf_start + hdr->e_phoff);
        LINE();

#ifdef _TEST_EXEC
	printk("esize=%d size=%d=\n",esize,size);
        printk("hdr->e_phnum=[%x]\n",
                hdr->e_phnum);
#endif

        for(i=0; i < hdr->e_phnum; i++) {
                LINE();
#ifdef _TEST_EXEC
                printk("p_type=[%x]\n",phdr[i].p_type);
#endif
                if(phdr[i].p_type != PT_LOAD) {
                        continue;
                }
                if(phdr[i].p_filesz > phdr[i].p_memsz) {
                        printk("image_load:: p_filesz > p_memsz\n");
#ifdef _TEST_EXEC
                        munmap(exec, esize);
#else // Kernel
                        free((void *)exec);
#endif
                        return 0;
                }
                if(!phdr[i].p_filesz) {
#ifdef _TEST_EXEC
                        LINE();
			printk("p_filesz=[%d]\n",phdr[i].p_filesz);
			printk("p_offset=[%d]\n",phdr[i].p_offset);
			printk("p_memsz=[%d]\n",phdr[i].p_memsz);
#endif
		        memset((char *)
				((Elf32_Addr)exec + (phdr[i].p_paddr-estart)),
				0x0,phdr[i].p_memsz);

                        continue;
                }

                // p_filesz can be smaller than p_memsz,
                // the difference is zeroe'd out.
                LINE();
                start = (unsigned char *) (elf_start + phdr[i].p_offset);
                if(!estart) {
                    estart = phdr[i].p_paddr;
                }
                taddr = (Elf32_Addr)exec + offset;
#ifdef _TEST_EXEC
                printk("taddr=[%x] phdr[i].p_paddr=[%x] exec=[%x] "
                       "offset=[%x] phdr[i].p_memsz=[%x] phdr[i].p_filesz=[%x] "
                       "size=[%d] start=[%d]\n",
                        taddr, phdr[i].p_paddr, exec, offset, phdr[i].p_memsz,
                        phdr[i].p_filesz,esize,start);
#endif
                memmove((unsigned char *)taddr,
                        (unsigned char *)start,phdr[i].p_filesz);
                offset += ((phdr[i].p_paddr - estart) + phdr[i].p_memsz);

                LINE();
         }

         // Mark the sections read-only as described.
        for(i=0; i < hdr->e_phnum; i++) {
                LINE();
#ifdef _TEST_EXEC
                printk("p_type=[%x]\n",phdr[i].p_type);
#endif
                if(phdr[i].p_type != PT_LOAD) {
                        continue;
                }
                if(phdr[i].p_filesz > phdr[i].p_memsz) {
                        printk("image_load:: p_filesz > p_memsz\n");
#ifdef _TEST_EXEC
                        munmap(exec, esize);
#else
                        free((void *)exec);
#endif
                        return 0;
                }
                if(!phdr[i].p_filesz) {
                        continue;
                }

                // p_filesz can be smaller than p_memsz,
                // the difference is zeroe'd out.
                LINE();
                start = (unsigned char *) (elf_start + phdr[i].p_offset);
                if(!estart) {
                    estart = phdr[i].p_paddr;
                }
                taddr = (Elf32_Addr)exec + offset;
#ifdef _TEST_EXEC
                printk("taddr=[%x] phdr[i].p_paddr=[%x] exec=[%x] "
                       "offset=[%x] phdr[i].p_memsz=[%x] phdr[i].p_filesz=[%x] "
                       "size=[%d] start=[%d]\n",
                        taddr, phdr[i].p_paddr, exec, offset, phdr[i].p_memsz,
                        phdr[i].p_filesz,esize,start);
#endif
                offset += ((phdr[i].p_paddr - estart) + phdr[i].p_memsz);

                LINE();
                if(!(phdr[i].p_flags & PF_W)) {
#ifdef _TEST_EXEC
                        LINE();
                        // Read-only.
                        mprotect((unsigned char *) taddr, 
                                  phdr[i].p_filesz, // RGD was memsz
                                  PROT_READ | PROT_EXEC);
#else
                        mem_set_read_only(taddr, phdr[i].p_filesz / PAGE_SIZE);
#endif
                }
         }

        LINE();

#ifdef _TEST_EXEC
        printk("i=[%x] hdr->e_entry=[%x] exec=[%x]\n",
                i,hdr->e_entry,exec);
#endif
	(*exec_mem)  = exec;
	(*exec_size) = esize;
        return (void *)((hdr->e_entry - estart) + (Elf32_Addr)exec);

}/* image_load */

int kexecve(const char *filename, char *const argv[],
                  char *const envp[])
{
    // If first line is #! exec interpreter and feed
    // it the command line options and the file.
    // Otherwise, load the entire file into RAM,
    // and call image_load.
    // On error, return -1 and set errno.
    // On success, no return (0).
    // The latter implies we have to setup the process context
    // and free prior RAM.
    char        *image      = NULL;
    unsigned int image_size = 0;
    unsigned int i          = 0;
    unsigned int j          = 0;
    unsigned int start      = 0;
    unsigned int end        = 0;
    unsigned int len        = 0;
    int argc                = 0;
    char interpreter[MAX_PATH]  = {0};
    char args[MAX_PATH]         = {0};
    char *arg               = NULL;
    char **argv2            = NULL;
    static int nr_recursion = 0;
    int delete_argv2        = 0;
    start_t elf_entry       = NULL;
    unsigned char *exec     = NULL;
    unsigned int exec_size  = 0;

    asm_disable_interrupt();

    if(!file_load(filename, &image, &image_size)) {
        printk("execve:: error loading file\n");
        asm_enable_interrupt();
        return -1;
    }

    // TODO - We should store argv2 in the process and
    //        and mark if it should be free'd on exit.

    // Handle interpreter invocation.
    if(image[0] == '#' && image[1] == '!') {
#ifdef _TEST_EXEC
        printk("execve:: image = [%s]\n", image);
#endif
        for(i = 2; i < image_size; ++i) {
            if(!isspace(image[i])) {
                start = i;
                break;
            }
        }
        for(i = start + 1; i < image_size; ++i) {
            if(isspace(image[i])) {
                end = i;
                break;
            }
        }
        for(i = start,j=0; i < end; ++i,++j) {
            if(j >= MAX_PATH) {
                printk("execve:: interpreter name too long\n");
                asm_enable_interrupt();
                return -1;
            }
            interpreter[j] = image[i];
        }
        for(i = end,j=0; i < image_size; ++i,++j) {
            if(image[i]=='\n') {
                break;
            }
            args[j]=image[i];
        }
        arg = strtok(args," \t");
        i   = 0;
        while(arg) {
            if(!argv2) {
                argv2 = (char **)malloc(sizeof(char *) * MAX_PATH + 1);
            }
            if(i >= MAX_PATH) {
                free((void *)argv2);
                printk("execve:: error processing interpreter args\n");
                asm_enable_interrupt();
                return -1;
            }
            len = strlen(arg);
            argv2[i] = (char *)malloc(len + 1);
            strcpy(argv2[i],arg);
            argv2[i][len]='\0';
            ++i;
            arg = strtok(NULL," \t");
        }
        /* Now get the rest of the args. */
        j = 0;
        while(i < MAX_PATH && argv[j]) {
            len = strnlen(argv[j],1024);
            argv2[i] = (char *)malloc(len + 1);
            strncpy(argv2[i],argv[j],1024);
            argv2[i][len] = '\0';
            ++i;
            ++j;
        }

        if(argv[j] != NULL) {
            free((void *)argv2);
            printk("execve:: error argv[j] != NULL\n");
            asm_enable_interrupt();
            return -1;
        }

        /* Now copy the script filename. */
        if(i >= MAX_PATH) {
            free((void *)argv2);
            printk("execve:: error i >= MAX_PATH\n");
            asm_enable_interrupt();
            return -1;
        }
#if 0
        /* argv[0] should be the filename */
        len = strlen(filename);
        argv2[i] = (char *)malloc(len + 1);
        strcpy(argv2[i],filename);
        argv2[i][len]='\0';
        ++i;
        if(i >= MAX_PATH) {
            free((void *)argv2);
            printk("execve:: error i >= MAX_PATH[2]\n");
            return -1;
        }
#endif
        argv2[i]='\0';
        // Free the original file loaded.
        if(!file_unload(&image)) {
            free((void *)argv2);
            printk("execve:: error unloading file [%s]\n",filename);
            asm_enable_interrupt();
            return -1;
        }

#ifdef _TEST_EXEC
        printk("interpreter [%s]\n",interpreter);
        for(i = 0; argv2[i]; ++i) {
            printk("argv2[%d]=[%s]\n",i,argv2[i]);
        }
        for(i = 0; envp[i]; ++i) {
            printk("envp[%d]=[%s]\n",i,envp[i]);
        }
        return 0;
#else
        // Call ourselves with the proper paramters.
        if(nr_recursion > EXEC_MAX_RECURSION) {
            printk("execve:: error nr_recursion > EXEC_MAX_RECURSION\n");
            asm_enable_interrupt();
            return -1;
        }
        nr_recursion++;
        asm_enable_interrupt();
        return kexecve(interpreter, argv2, envp);
#endif
    }

    // If we are in a recursive call, then we
    // allocated argv2 and it must be free'd.
    if(nr_recursion) {
        delete_argv2 = 1;
    } else {
        delete_argv2 = 0;
        argv2 = (char **)argv;
    }
    for(i = 0; argv2[i]; ++i) {
        argc++;
    }
    nr_recursion--;
    // At this point, the ELF image is in RAM.
    // Parse it out and jump to it.
    LINE();
    elf_entry = image_load(image, image_size, &exec, &exec_size);
    LINE();

    // TODO - Do we put envp into the process struct
    //        and make it accessible via getenv() ?
#ifdef _TEST_EXEC
    printk("elf_entry=[%x]\n",elf_entry);
    LINE();
    i = elf_entry();
    printk("returns =[%d]\n",i);
#endif
    LINE();

    if(!file_unload(&image)) {
        if(nr_recursion) {
            for(i = 0; i < argc; ++i) {
                free((void *)argv2[i]);
            }
            free((void *)argv2);
        }
#ifdef _TEST_EXEC
        munmap(elf_entry, image_size);
#endif
        printk("execve:: error unloading file [%s]\n",filename);
        return -1;
    }
#ifdef _TEST_EXEC
        munmap(exec,exec_size);
#else // Kernel
    //
    // - Set up the entry point and args
    // into the process structure...
    // Mark flag for first time execution
    // as we need to call into the entry point
    // when we schedule the process.
    // - Free the old process image...
    //
    // We have:-
    //
    // exec         - memory image to free 
    // exec_size    - size of image
    // elf_entry    - pointer to the start method
    // argc         - argument count
    // argv2        - arguments
    // envp         - environment
    // delete_argv2 - flag to indicate if argv2 should be free'd
    //
    // Free up prior memory.
    // Should also happen in exit.c.
    if(current_process->p_exec) {
        mem_unset_read_only(current_process->p_exec,
                            current_process->p_exec_size);
        free((void *)current_process->p_exec);
    }
    if(current_process->p_delete_argv) {
        for(i = 0; i < current_process->p_argc; ++i) {
            free((void *)current_process->p_argv[i]);
        }
        free((void *)current_process->p_argv);
    }
    // Close all open files.
    for(i = 0; i < MAX_FILES; ++i) {
        for(j = 0; j < MAX_FILES; ++j) {
            if(current_process->file_desc[i] == 
               &(current_process->file_tab[j])) {
                kclose(i);
            }
        }
    }
    // Close all open directories.
    for(i = 0; i < MAX_DIR; ++i) {
        if(current_process->dir_tab[i].__allocation == DEV_BLOCK_SIZE) {
            kclosedir(&(current_process->dir_tab[i])); 
        }
    }
    // Setup our new image.
    current_process->p_exec         = exec;
    current_process->p_exec_size    = exec_size;
    current_process->p_elf_entry    = elf_entry;
    current_process->p_argc         = argc;
    current_process->p_argv         = argv2;
    current_process->p_envp         = envp;
    current_process->p_delete_argv  = delete_argv2;
    current_process->p_first_exec   = 1;
    asm_enable_interrupt();
    schedule();
#endif
    return 0;

}/* kexecve */

void
kstart()
{
    void (*elf_entry)(int argc, char **argv, char **envp);

    if(current_process->p_elf_entry == NULL) {
        panic("kstart:: invalid elf entry point\n");
    }
    elf_entry = current_process->p_elf_entry;
    (*elf_entry) (
                current_process->p_argc,
                current_process->p_argv,
                current_process->p_envp);
}/* kstart */

#ifdef _TEST_EXEC
int main(int argc, char **argv)
{
#ifdef _TEST_SHELL
    char *pargv[]={"./test.sh","-a","--verbose",0};
    char *penvp[]={"LD_LIBRARY_PATH=.:..","PATH=.:..",0};
    printk("argv[0]=[%s]\n",argv[0]);
    int  rtvl = kexecve("./test.sh",pargv,penvp);
#elif _TEST_EXE
    char *pargv[]={"./test.exe","-a","--verbose",0};
    char *penvp[]={"LD_LIBRARY_PATH=.:..","PATH=.:..",0};
    printk("argv[0]=[%s]\n",argv[0]);
    int  rtvl = kexecve("./test.exe",pargv,penvp);
#endif
    return 0;
}
#endif
