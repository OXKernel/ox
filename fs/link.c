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
//      link.c
// 
// @description:
//      Implementation of file system symlinks
//      and hardlinks.
//
// @author:
//      Dr. Roger G. Doss, PhD
//
#include "bool.h"
#include "paths.h"
#include "block.h"
#include "dev.h"
#include "inode.h"
#include "compat.h"

#ifdef _TEST_LINK_INC
#include <stdio.h>
#include <string.h>
#define printk printf
#endif

#include "file.h"
#include "dir.h"
#include "link.h"

int klink(const char *oldpath, const char *newpath)
{
    int dev = master_get_dev(oldpath);
    if(!oldpath || !newpath) {
        errno = EINVAL;
        printk("link:: NULL path\n");
        return -1;
    }
    if(inode_create(dev, current_process->cwd, newpath, 
                    INODE_CREATE_SYMLINK,
                    0777,
                    current_process->umask,
                    current_process->group,
                    current_process->owner,
                    oldpath) != INODE_OK) {
        printk("link:: error creating link oldpath=[%s] newpath=[%s]\n",
                oldpath, newpath);
        return -1;
    }
    return 0;
}

// RGDTODOLINK - Add kreadlink and symlink to link.c
/*
    The algorithm is:
    Given a parent dir and a path component, open the parent dir,
    find the path component, if its inode is marked as a symlink,
    do an inode_get on this path with current_dir as parent_dir.
    We would have to call krealpath and this may be recursive,
    so we would need to check if we again call kreadlink, if we
    do it is an error as its a circular path. Otherwise, resolve
    the path to an absolute path, call inode_get to get the inode,
    return its block number for resolution in the path processing.
    See RGDTODOLINK comments in the code.

    // This is good, see where we have 'RGDTODOLINK' in inode.c, this
    // function goes there, parent = kreadlink(dev, current_dir, path, &errno);
    //
    // RGDTODO
    // This must also access hardlink, but we do not use it in path
    // processing in inode.c for hardlinks as we allow only symlinks to point
    // to directories (wherein symlinks do not modify the inode's reference count).
    //
*/
block_t kreadlink(int dev, int current_dir, char *pathcomponent, int *error)
{
        static int nr_recursion = 0;
        inode_t inode = {0};
        link_t  link  = {0};
        char link_path[MAX_PATH]={0};
        char res_path[MAX_PATH]={0};
        // Open current_dir, find pathcomponent.
        // Is its type a symlink ?
        // If yes, get the path from the block (cast to link_t)
        // call krealpath, call inode_get, get the block number
        // from inode.self, return.
        if(!error) {
            error = &errno; 
        }
        ++nr_recursion;
        if(nr_recursion > MAX_RECURSION) {
            *error = ELOOP; // Too many symlinks, avoid infinite loop.
                            // See 'man 2 link'.
            --nr_recursion;
            return INODE_NULL;
        }
        if(block_read(dev, current_dir, (char *)&inode)!= BLOCK_OK) {
            *error = EACCES;
            printk("kreadlink:: error reading block dev=%d current_dir=%u\n",
                    dev, current_dir);
            return INODE_NULL;
        }
        // NOTE: - We implemented hard links as soft links with
        //         the difference between hard and softlink being 
        //         hard links have a reference count.
        //       - We need a reference count for the inode to monitor
        //         how many hard links are pointing to it.
        //         With such a reference count, we do not delete
        //         if its not zero. Additionally, we do not allow
        //         hard links to anything other than an existing file.
        if(!(inode.is_symlink || inode.is_hardlink)) {
            // Regular inode.
            // We were going to :=
            //   Scan for the block containing the path.
            //   See scan_code in inode.c
            // But this is a logical error, kreadlink only reads links.
            *error = EINVAL;
            printk("kreadlink:: file [%s] not a link\n", pathcomponent);
            return INODE_NULL;
        } else {
            // Its a symlink. Get the actual path and
            // do symlink processing below.
            if(block_read(dev, inode.next, (char *)&link) != BLOCK_OK) {
                *error = EACCES;
                printk("kreadlink:: error reading block dev=%d block=%u\n",
                        dev, inode.next);
                return INODE_NULL;
            }
            strncpy(link_path, link.path, MAX_PATH);
        }
        if(!krealpath(dev, current_dir, link_path, res_path, error)) {
            printk("kreadlink:: error resolving path dev=%d current_dir=%u link_path=%s error=%d\n", dev, current_dir, link_path, *error);
            return INODE_NULL;
        }
        // Recursive call, we are called from inode_get.
        printk("kreadlink:: link_path=%s\n",link_path);
        if(inode_get(dev, current_dir, link_path, 
                            true, INODE_DONTCHECKPERMS, &inode)!=INODE_OK) {
           *error = EINVAL;
           printk("kreadlink:: error resolving inode dev=%d current_dir=%u link_path=%s error=%d\n", dev, current_dir, link_path, *error);
           return INODE_NULL;
        }
        --nr_recursion;
        return inode.self;
}

#ifdef _TEST_LINK
int main(int argc, char **argv)
{
    int i   = 0;
    int dev = 0;
    int fd  = 0;
    master_inode_t *master = NULL;
    struct stat stbuf1={0}, stbuf2={0}, stbuf3={0}, stbuf4={0};
    char buf[1024]={0};
    DIR *dir = 0;
    struct dirent *dir_ent = 0;
    // Obtains the device based on block_open -> dev_open
    // Initialization here is similar to what would happen
    // in the real kernel. We must make the file system first.
    if(inode_dev_open("./inode.dat",&dev) == INODE_INIT) {
       P();
       if(inode_mkfs("./inode.dat", 2 * TWOMEG) != INODE_OK) {
            printk("fs_init:: error initializing filesystem\n");
            return 1;
       }
       P();
       if(inode_dev_open("./inode.dat", &dev) != INODE_OK) {
            printk("fs_init:: error opening device\n");
            return 1;
       }
    }
    P();
    master = master_get(dev);
    master_set_dev("./inode.dat", dev);
    // Create a file and a dir
    // Create links to the file and the dir
    // Stat the links
    // Open the link, write, open the file read
    // Add file to the link dir, open the dir, read
    // Test klstat
    // Add a mode to stat to determine file type (
    // yes, this is part of inode.c/inode_get_permissions
    // and is part of st_mode not st_rdev, the latter
    // is for device special files).
    if(kmkdir("/bar", S_IRWXU) == -1) {
        printk("kmkdir:: error creating directory [/bar]\n");
    }
    if((fd=kcreat("/bar/file1", S_IRUSR | S_IWUSR)) == -1) {
        printk("kcreat:: error creating file [/bar]\n");
    }
    if(kwrite(fd, "hello world", strlen("hello world")) == -1) {
        printk("kwrite:: error writing file\n");
    }
    if(kstat("/bar", &stbuf1) == -1) {
        printk("kstat:: error stat'ing directory [/bar]\n");
    }
    if(kstat("/bar/file1", &stbuf2) == -1) {
        printk("kstat:: error stat'ing directory [/bar]\n");
    }
    // Create links.
    if(klink("/bar", "/barlink") == -1) {
        printk("klink:: error creating link\n");
    }
    if(klink("/bar/file1", "/file1link") == -1) {
        printk("klink:: error creating link\n");
    }
    // Stat the links.
    if(kstat("/barlink", &stbuf3) == -1) {
        printk("kstat:: error stat'ing directory [/bar]\n");
    }
    if(kstat("/file1link", &stbuf4) == -1) {
        printk("kstat:: error stat'ing directory [/bar]\n");
    }
    // The stat buffers should be equal.
    if(!memcmp(&stbuf1,&stbuf3,sizeof(struct stat))) {
        printk("successfully stat'd /bar and /barlink\n");
    } else {
        printk("error link [/bar]\n");
    }
    if(!memcmp(&stbuf2,&stbuf4,sizeof(struct stat))) {
        printk("successfully stat'd /bar/file1 and /file1link\n");
    } else {
        printk("error link [/file1link]\n");
    }
    if(kclose(fd) == -1) {
        printk("error closing file [/bar/file1]\n");
    }
    if((fd=kopen("/file1link", O_RDWR)) == -1) {
        printk("error opening link [/file1link]\n");
    }
    if(kwrite(fd, "hello world, good bye", 
                strlen("hello world, good bye")) == -1) {
        printk("kwrite:: error writing file\n");
    }
    if(kclose(fd) == -1) {
        printk("error closing file [/file1link]\n");
    }
    if((fd=kopen("/bar/file1", O_RDWR)) == -1) {
        printk("error opening file [/bar/file1]\n");
    }
    if(kread(fd, buf,
                strlen("hello world, good bye")) == -1) {
        printk("kread:: error reading file\n");
    }
    if(!strcmp(buf,"hello world, good bye")) {
        printk("kread:: successfully read file\n");
    }
    if(kclose(fd) == -1) {
        printk("kclose:: error closing file\n");
    }
    if((fd=kcreat("/barlink/file2", S_IRUSR | S_IWUSR)) == -1) {
        printk("kcreat:: error creating file [/bar]\n");
    }
    if(kclose(fd) == -1) {
        printk("kclose:: error closing file\n");
    }
    dir = kopendir("/bar");
    if(!dir) {
        printk("kopendir:: error failed to open directory [/bar]\n");
    } else {
        printk("kopendir:: successfully opened directory [/bar]\n");
    }
    // Expect . .. file1 file2
    do {
        dir_ent = kreaddir(dir);
        if(dir_ent) {
            printk("file=%s\n",dir_ent->d_name);
        }
    } while(dir_ent);
    kclosedir(dir);
    if(klstat("/barlink", &stbuf1) == -1) {
        printk("klstat:: error stat'ing link [/barlink]\n");
    } else {
        printk("klstat:: successfully stat'd link size=%d\n",stbuf1.st_size);
    }
}
#endif
