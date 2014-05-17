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
//      krealpath.h
// @description:
//      Implements kernel's realpath resolution.
//      This resolves a relative path into an absolute
//      path given a starting current_dir.
//
// @author:
//      Dr. Roger G. Doss, PhD

//
// For testing, we use integer paths from 0 to n-1
// this way parent is i-1 and self is i and it should
// simplify testing of the algorithm. This allows us
// to test outside of the block/inode code to make sure
// this function works correctly prior to integration.
//


#if (!_TEST_KREALPATH) && (!_TEST_KREALPATH_WITH_INODE)
#include <errno.h> // RGDTODO - Check which file has the errno values.
#include "paths.h"
#include "bool.h"
#include "block.h"
#include "inode.h"
#include "compat.h"
#endif

// Use _TEST_KREALPATH_WITH_INODE define and not _TEST_KREALPATH to test with inode.c
// and the rest of the system. Use _TEST_KREALPATH to test krealpath standalone.
#ifdef _TEST_KREALPATH_WITH_INODE
#include <stdio.h>
#include <string.h>

#include "paths.h"
#include "bool.h"
#include "block.h"
#include "inode.h"
#include "compat.h"

#define printk printf
#endif

#ifdef _TEST_KREALPATH
typedef unsigned int block_t;
#include <stdio.h>
#include <string.h>

#define printk printf
#define INODE_ROOT_BLOCK 0
#define INODE_OK 0
#define EOK 0   // RGDTODO - Check which ox file has these, if not create.
#define EINVAL -1
#define ENAMETOOLONG -2
#define INODE_NULL 0
#define BLOCK_OK 1
#endif

#include "krealpath.h"
#include "paths.h"

#ifdef _TEST_KREALPATH
typedef struct inode {
   block_t self;
   block_t parent;
   char path[MAX_PATH];
} inode_t;


int block_read(int dev, block_t block, char *data)
{
	inode_t *inode = (inode_t *)data;

	inode->self = block;
	if(inode->self == 0) {
        inode->parent = 0;
	} else {
	    inode->parent = --block;
	}
    if(inode->self == 0) {
	    sprintf(inode->path, "/");
    } else {
	    sprintf(inode->path, "%d", inode->self);
    }
	return BLOCK_OK;
}

int inode_get(int dev, block_t current_dir, char *path, inode_t *inode)
{
    char ptr_path[MAX_PATH]={0}, *ptr = NULL;
    block_t i = 0;
    int start = 0;
    strncpy(ptr_path, path, MAX_PATH);
    ptr = ptr_path;
    
    while((ptr=get_next_path(&start, ptr_path)) != NULL) {
         i = strtoul(ptr, NULL, 10);
	     start++;
    }
    printf("path=%s i=%d\n",path,i);
    inode->self = i;
    inode->parent = --i;
    sprintf(inode->path, "%d", inode->self);
    return INODE_OK;
}

#endif

char *krealpath(int dev, block_t current_dir, char *path, char *res_path, int *error)
{
    master_inode_t *master = master_get(dev);
    int start = 0, count = 0, i = 0, ptr_tab_len = 0, gen_path_ptr = 0;
    char ptr_path[MAX_PATH]={0}, 
                      *ptr = NULL, 
        *ptr_tab[MAX_PATH] = {0}, *gen_path[MAX_PATH] = {0}, 
         tmp_path[MAX_PATH]= {0};
    block_t current = INODE_NULL;
    inode_t inode;

    // RGDTODOLINK: This currently does not handle reading links,
    //       it does read blocks and figure out the pre-pending
    //       path. The pre-pendin path needs to call readlink on
    //       every component of the path to resolve it correctly.
    //       ** We do not need to readlink here.
    //       We just want to construct a path. If we have
    //       a link, then we have /foo/link/bar.
    //       This depends on how this is done in inode.c
    //       if we have a sym link to a directory, then create
    //       a file underneath using the resolved sym link
    //       (I believe inode.c resolves the sym link first, 
    //       so we should be ok here).
    //
    // NOTE: This should just make the path absolute
    //       when we create or get we should check that
    //       the components are valid. i.e, that if 'path_comp/file'
    //       the path_comp should be a dir or link to a dir.
    //       links require special handling and are
    //       a different file type (aside from file/dir/dev).
    //
    memset(res_path, 0x0, MAX_PATH);

    // If the path didn't start at root, then
    // prepend the path from root to current_dir.
    if((path[0] == '.' && path[1] == '/') ||
       (path[0] == '.' && path[1] == '.') ||
       (path[0] != '/')) {
        // Construct absolute path relative to current_dir.
        char tmp[MAX_PATH]={0};
        while(current_dir != INODE_ROOT_BLOCK) {
            // printk("line=%d\n",__LINE__);
            if(block_read(dev, current_dir, (char *)&inode) != BLOCK_OK) {
                printk("error reading block dev=%d block=%u\n",
                        dev, current_dir);
                *error = EINVAL;
                return NULL;
            }
            // RGDTODOLINK - This should call readlink here.
            if(!push_front(tmp, inode.path)) {
                printk("error push_front tmp=%s inode.path=%s\n",
                        tmp, inode.path);
                *error = EINVAL;
                return NULL;
            }
            if(!push_front(tmp, "/")) {
                printk("error push_front tmp=%s inode.path=%s\n",
                        tmp, "/");
                *error = EINVAL;
                return NULL;
            }
            current_dir = inode.parent;
        }
        // Will execute in cases:
        // 1) start with ./
        // 2) start with ../
        // 3) start with != /
        // The above code will resolve and in case 3
        // where we have != / and current_dir is root
        // we just come here to get the appended / character.
        if(strlen(tmp) + 1 > MAX_PATH) {
            *error = ENAMETOOLONG;
            return NULL;
        }
        strcat(tmp,"/");
        // printk("tmp=%s res_path=%s\n",tmp,res_path);
        if(!push_front(res_path,tmp)) {
            printk("error push_front res_path=%s tmp=%s\n",
                    res_path,tmp);
            *error = EINVAL;
            return NULL;
        }
    }

    if(strlen(res_path) + strlen(path) > MAX_PATH) {
        *error = ENAMETOOLONG;
        return NULL;
    }

    strcat(ptr_path,res_path);
    strcat(ptr_path,path);
    memset(res_path,0x0,MAX_PATH);
    ptr = ptr_path;

    // Get all path components. This allows us to do a look ahead
    // while processing the path to implement '..' support.
    while((ptr = get_next_path(&start, ptr_path)) != NULL ) {
        ptr_tab[ptr_tab_len++] = ptr;
        start++;
    }
    for(i = 0; i < ptr_tab_len; ++i) {
        ptr = ptr_tab[i];
        //printf("ptr = %s\n", ptr);
	    if(!strcmp(ptr, ".")) {
	    } else if(!strcmp(ptr, "..")) {
            do {
	          if(gen_path_ptr == 0) {
	          } else {
                 gen_path_ptr--;
	         }
             ptr = ptr_tab[++i];
           } while(i < ptr_tab_len && !strcmp(ptr,".."));
           --i;
	    } else {
            gen_path[gen_path_ptr++] = ptr;
            gen_path[gen_path_ptr] = '\0';
	    }
    }
    // Assemble the path.
    for(i = 0; i < gen_path_ptr; ++i) {
       strcat(res_path, gen_path[i]);
       if(i != (gen_path_ptr-1)) {
           strcat(res_path, "/"); 
       }
    }
    // If nothing was given, assume root.
    if(strlen(res_path) == 0) {
        strcat(res_path,"/");
    }



    *error = EOK;
    return res_path;
}

#ifdef _TEST_KREALPATH
int
main(int argc, char **argv)
{
   char path[MAX_PATH], res_path[MAX_PATH]={0};
   int error = 0;
   //strcpy(path,"/1/2/../../3"); 
   //strcpy(path,"/"); 
   //strcpy(path,"./1");
   strcpy(path,"./../2");
   if(krealpath(0,2,path,res_path,&error) == NULL) {
       printf("error path=%s res_path=%s error=%d\n", path,res_path,error);
   } else {
       printf("result path=%s res_path=%s error=%d\n", path,res_path,error);
   }
}
#endif
