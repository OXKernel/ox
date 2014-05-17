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
//      dir.c
// 
// @description:
//      Implementation of directory specific functionality.
//      This is based on our inode implementation.
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

#ifdef _TEST_DIR_INC
#include <stdio.h>
#include <string.h>
#define printk printf
#else
#include <ox/error_rpt.h>
#endif

#include "file.h"
#include "dir.h"

/* Note for directory, we could try hashing since the
 blocks are 127 in length which is prime.
 So: 
 
 int
 d_find(char *path, *ptr)
 {
   block_t i = 0;  
   *ptr = zero_ptr;
   while(true) {
    block_t pos = hash(path) % 127;
    for(i = pos; i < 127; ++i) {
          block_read(bmap.blocks[i], tmp);
          if(strcmp(tmp.path,path) == 0) {
            *ptr = tmp;
            return FOUND;
          }
    }
    if(pos) {
        for(i = 0; i < pos; ++i) {
            block_read(bmap.blocks[i], tmp);
            if(strcmp(tmp.path,path) == 0) {
                *ptr = tmp;
                return FOUND;
            }
        }
    }
    if(bmap.next == INODE_NULL) {
        return NOT_FOUND;
    }
    block_read(bmap.next, tmp);
    current = bmap.next;
    bmap = tmp;
  }
  return NOT_FOUND;
 }

 This requires we hash on insert into a dir.
 For dir specific, we could still use rbst.

*/
/*
   open the dir using inode_get return data into DIR *
*/
DIR *kopendir(const char *path)
{
    int i = 0;
    bool found = false;
    int dev = master_get_dev(path);
    for(i = 0; i < MAX_DIR; ++i) {
        if(current_process->dir_tab[i].__allocation == 0) {
            current_process->dir_tab[i].__fd = i;
            current_process->dir_tab[i].__allocation = DEV_BLOCK_SIZE;
            found = true;
            break;
        }
    }
    if(!found) {
        return NULL;
    }
    if(inode_get(dev, current_process->cwd, path, 
                 false, INODE_RX, &(current_process->dir_tab[i].__data)) != INODE_OK) {
        printk("opendir:: error opening directory\n");
        current_process->dir_tab[i].__allocation = 0;
        current_process->dir_tab[i].__fd = 0;
        return NULL;
    }
    current_process->dir_tab[i].__data.accessed_time = ktime(0);
    strncpy(current_process->dir_tab[i].__path, path, MAX_PATH);
    return &(current_process->dir_tab[i]);

}// opendir

/*
   close the directory
*/
int kclosedir(DIR *dir)
{
    static DIR zero_dir={0};
    int dev = master_get_dev(dir->__path); // RGDTODO - Not sure if path is the full dir path.
    if(!dir || dir->__fd < 0 || dir->__fd >= MAX_DIR) {
        errno = EINVAL;
        printk("closedir:: invalid param\n");
        return -1;
    }
    current_process->dir_tab[dir->__fd].__data.accessed_time = ktime(0);
    if(block_write(dev, current_process->dir_tab[dir->__fd].__data.self,
                  (char *)&current_process->dir_tab[dir->__fd].__data) != BLOCK_OK) {
        errno = EACCES;
        printk("closedir:: error closing directory dev=%d block=%u\n",
                dev, current_process->dir_tab[dir->__fd].__data.self);
    }
    // Struct assign.
    current_process->dir_tab[dir->__fd] = zero_dir;
    return 0;

}// closedir

/*
   start our scan back to the begining
*/
void krewinddir(DIR *dir)
{
    if(!dir || dir->__fd < 0 || dir->__fd >= MAX_DIR) {
        errno = EINVAL;
        printk("rewinddir:: invalid parameter\n");
        return;
    }
    dir->__offset = 0;
}

/*
   return an entry in the dir filling the dirent struct accordingly
*/
struct dirent *kreaddir(DIR *dir)
{
    // Offset is the number of the file we are pointing to.
    // All data is in block_map starting from __data.next.
    static struct dirent zero_entry={0};
    block_map_t bmap = {0};
    inode_t tmp = {0};
    int dev = master_get_dev(dir->__path);
    int block  = 0;
    int offset = 0;
    // RGDTODO - Test this.
    current_process->dir_tab[dir->__fd].__data.accessed_time = ktime(0);
    if(dir->__offset == 0 || dir->__offset == 1) {
        dir->__entry = zero_entry;
        if(tmp.is_directory) {
            dir->__entry.d_type = 'D'; // RGDTODO - Is this how we specify the type?
        }
        dir->__entry.d_ino = 0;
        dir->__entry.d_off = 0;
        dir->__entry.d_reclen = 0;
        strncat(dir->__entry.d_name,dir->__path,MAX_PATH);
        strncat(dir->__entry.d_name,"/",1);
        if(dir->__offset == 0) {
            strncat(dir->__entry.d_name,".",1);
        } else {
            strncat(dir->__entry.d_name,"..",2);
        }
        dir->__offset++;
        return &(dir->__entry);
    }
RETRY:
    // We subtract the offset from 2 because
    // we have . and .. which are virtual files
    // taking up offset 0 and 1 respectively.
    block  = (dir->__offset-2) / BMAP_BLOCKS;
    offset = (dir->__offset-2) % BMAP_BLOCKS;
    // printk("block_ptr=%u block=%u offset=%u\n", 
    //                  dir->__block_ptr,block,offset);
    if(dir->__block_ptr == block && dir->__block != INODE_NULL) {
        if(block_read(dev, dir->__block, (char *)&bmap) != BLOCK_OK) {
            errno = EACCES;
            printk("readdir:: error reading directory dev=%d block=%u\n",
                    dev, dir->__block);
            return NULL;
        }
        if(bmap.blocks[offset] == INODE_NULL) {
            // We unlink by zero setting the entry,
            // so to traverse we have to skip all zero
            // entries until a null bmap.next found below
            // in scan to the correct block code.
            dir->__offset++;
            goto RETRY;
        }
        if(block_read(dev, bmap.blocks[offset], (char *)&tmp) != BLOCK_OK) {
            errno = ENOENT;
            printk("readdir:: error reading entry dev=%d block=%u\n",
                    dev, bmap.blocks[offset]);
            return NULL;
        }
        // Struct assign.
        dir->__entry = zero_entry;
        if(tmp.is_directory) {
            dir->__entry.d_type = 'D'; // RGDTODO - Is this how we specify the type?
        }
        if(tmp.is_file) {
            dir->__entry.d_type = 'F';
        }
        if(tmp.is_symlink) {
            dir->__entry.d_type = 'S';
        }
        if(tmp.is_hardlink) {
            dir->__entry.d_type = 'H';
        }
        dir->__entry.d_ino = bmap.blocks[offset];
        dir->__entry.d_off = offset;
        dir->__entry.d_reclen = tmp.size;
        strncat(dir->__entry.d_name,dir->__path,MAX_PATH);
        strncat(dir->__entry.d_name,"/",1);
        strncat(dir->__entry.d_name,tmp.path,MAX_PATH);
        dir->__offset++;
        return &(dir->__entry);
    }
    // Scan to the correct block, if we reach INODE_NULL
    // first, we return NULL.
    // RGDTODO - We are concerned here that we have the wrong block
    // once this loop exists. Try with block == 0 and block == 1
    // where block==1 and we really have just two blocks will
    // it end with dir->__block == INODE_NULL ?
    for(dir->__block_ptr = 0, dir->__block = dir->__data.next; 
            dir->__block_ptr < block; ++(dir->__block_ptr)) {
       if(dir->__block == INODE_NULL) {
            return NULL;
       }
       if(block_read(dev, dir->__block, (char *)&bmap) != BLOCK_OK) {
            errno = EACCES;
            printk("readdir:: error reading block dev=%d block=%u\n",
                    dev, dir->__block);
            return NULL;
       }
       // Advance block pointer.
       dir->__block = bmap.next;
    }
    // Terminate, if we put this case first, we would never
    // execute, but if we don't consider it, we will go into
    // an infinite loop.
    if(dir->__block_ptr == block && dir->__block == INODE_NULL) {
        return NULL;
    }
    goto RETRY;
}

/*
   delete a directory, if dir is free
   otherwise, the user code will have to iterate it and
   delete all the files in it recursively
*/
int krmdir(const char *path)
{
    int dev = master_get_dev(path);
    if(inode_free(dev, current_process->cwd, path, NULL) != INODE_OK) {
        printk("rmdir:: error removing directory\n");
        return -1;
    }
    return 0;
}

/*
   inode_create with dir option
   The user must implement mkdir -p as this is recursive.
*/
int kmkdir(const char *path, mode_t mode)
{
    int dev = master_get_dev(path);
    if(inode_create(dev, current_process->cwd, path, INODE_CREATE_DIR, 
                    mode, current_process->umask, 
                    current_process->group, 
                    current_process->owner, NULL) != INODE_OK) {
        printk("mkdir:: error creating directory\n");
        return -1;
    }
    return 0;
}

#ifdef _TEST_DIR
int main(int argc, char **argv)
{
    // Open the filesystem (see the code from file.c test driver).
    // Test kmkdir, krmdir.
    // Test kopendir, kclosedir, krewinddir, kreaddir.
    // Test kchdir and kfchdir (by going back into file.c and using mkdir 
    // to create directories we can change to and correcting kchdir,kfchdir
    // to check if we are changing to a directory).
    // Test krename (mkdir, create a bunch of files, then rename, 
    // then readdir and see all the files we created are there).
    int  i  = 0;
    int fd  = 0;
    int dev = 0;
    master_inode_t *master = NULL;
    struct stat stbuf={0};
    DIR *dir = 0;
    struct dirent *dir_ent = 0;
    char buf[128]={0};

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
    P();
    if(kmkdir("/bar", S_IRWXU) == -1) {
        printk("kmkdir:: error creating directory [/bar]\n");
    } else {
        printk("kmkdir:: success\n");
    }
    P();
    if(kstat("/bar", &stbuf) == -1) {
        printk("kstat:: error stat'ing directory\n");
    } else {
        printk("kstat:: success\n");
    }
    if(krmdir("/bar") == -1) {
        printk("krmdir:: error removing directory\n");
    } else {
        if(kstat("/bar", &stbuf) == -1) {
            printk("kstat:: stat succeeded unable to stat deleted dir\n");
        } else {
            printk("kstat:: error stat of deleted directory succeeded\n");
        }
    }
    // - Create a file inside the directory, should fail to delete.
    // - Unlink the file, should allow us to delete the directory.
    if(kmkdir("/bar", S_IRWXU) == -1) {
        printk("kmkdir:: error creating directory [/bar]\n");
    } else {
        printk("kmkdir:: success\n");
    }
    if((fd = kcreat("/bar/foo1", S_IRUSR)) == -1) {
        printk("kcreat:: error creating file\n");
    } else {
        printk("kcreat:: successfully creating file [/bar/foo1]\n");
    }
    if(krmdir("/bar") == -1) {
        printk("krmdir:: successfully did not remove non-empty directory [/bar]\n");
    }
    if(kunlink("/bar/foo1") == -1) {
        printk("kunlink:: error failed to unlink file [/bar/foo1]\n");
    } else {
        printk("kunlink:: successfully unlinked file [/bar/foo1]\n");
    }
    if(krmdir("/bar") == -1) {
        printk("krmdir:: error failed to remove empty directory [/bar]\n");
    } else {
        printk("krmdir:: successfully removed directory [/bar]\n");
    }
    if(kmkdir("/bar", S_IRWXU) == -1) {
        printk("kmkdir:: error failed to make directory [/bar]\n");
    }
    if(kmkdir("/bar/foodir", S_IRWXU) == -1) {
        printk("kmkdir:: error failed to make directory [/bar/foodir]\n");
    }
    if(kstat("/bar/foodir", &stbuf) == -1) {
        printk("kstat:: error failed to stat directory [/bar/foodir]\n");
    } else {
        printk("kstat:: successfully stat directory [/bar/foodir]\n");
    }
    if(kmkdir("/bar/foodir", S_IRWXU) == -1) {
        printk("kmkdir:: successfully failed to create existing directory\n");
    }
    if((fd = kcreat("/bar/foodir/file1", S_IRUSR)) == -1) {
        printk("kcreat:: error creating file\n");
    } else {
        printk("kcreat:: successfully created file [/bar/foodir/file1]\n");
    }
    if((fd = kcreat("/bar/foodir/file2", S_IRUSR)) == -1) {
        printk("kcreat:: error creating file\n");
    } else {
        printk("kcreat:: successfully created file [/bar/foodir/file2]\n");
    }
    // Test kopendir, kclosedir, krewinddir, kreaddir.

    dir = kopendir("/bar/foodir");
    if(!dir) {
        printk("kopendir:: error failed to open directory [/bar/foodir]\n");
    } else {
        printk("kopendir:: successfully opened directory [/bar/foodir]\n");
    }
    do {
        dir_ent = kreaddir(dir);
        if(dir_ent) {
            printk("file=%s\n",dir_ent->d_name);
        }
    } while(dir_ent);

    // - delete file1, then re-set dir and re-read, should have
    //   only one file and tests krewinddir.
    if(kunlink("/bar/foodir/file1") == -1) {
        printk("kunlink:: error unlinking file [/bar/foodir/file1]\n");
    } else {
        printk("kunlink:: successfully unlinked file [/bar/foodir/file1]\n");
    }
    krewinddir(dir);
    // Expect only . .. and file2.
    do {
        dir_ent = kreaddir(dir);
        if(dir_ent) {
            printk("file=%s\n",dir_ent->d_name);
        }
    } while(dir_ent);
    // - fill directory to 128 entries and re-read.
    for(i = 0; i < 129; ++i) {
       if(i == 2) continue;
       sprintf(buf, "/bar/foodir/file%d", i);
       if(kcreat(buf, S_IRUSR) == -1) {
            printk("kcreat:: error creating file [%s]\n", buf);
       }
    }
    krewinddir(dir);
    printk("created latest file%d\n",i);
    // Expect . .. file0...filei
    do {
        dir_ent = kreaddir(dir);
        if(dir_ent) {
            printk("file=%s\n",dir_ent->d_name);
        }
    } while(dir_ent);
    // - fill directory to 128 entires, delete one/few and re-read.
    // - delete at the boundaries...
    if(kunlink("/bar/foodir/file0") == -1) {
        printk("kunlink:: error unlinking file [/bar/foodir/file0]\n");
    }
    if(kunlink("/bar/foodir/file64") == -1) {
        printk("kunlink:: error unlinking file [/bar/foodir/file64]\n");
    }
    if(kunlink("/bar/foodir/file128") == -1) {
        printk("kunlink:: error unlinking file [/bar/foodir/file128]\n");
    }
    krewinddir(dir);
    // Expect . .. file1...file63...file127 (skip 0, 64, 128)
    do {
        dir_ent = kreaddir(dir);
        if(dir_ent) {
            printk("file=%s\n",dir_ent->d_name);
        }
    } while(dir_ent);
    if(kclosedir(dir) == -1) {
        printk("kclosedir:: error closing directory [/bar/foodir]\n");
    } else {
        printk("kclosedir:: successfully closed dir [/bar/foodir]\n");
    }
    if(kstat("/bar/foodir", &stbuf) == -1) {
        printk("kstat:: error stat'ing file\n");
        return 0;
    }
    // Test krename (mkdir, create a bunch of files, then rename, 
    // then readdir and see all the files we created are there).
    if(krename("/bar/foodir", "/bar/foodir2") == -1) {
        printk("krename:: error failed to rename directory [/bar/foodir]\n");
    }
    dir = kopendir("/bar/foodir2");
    // Expect . .. file1...file63...file127 (skip 0, 64, 128)
    // Same as /bar/foodir
    do {
        dir_ent = kreaddir(dir);
        if(dir_ent) {
            printk("file=%s\n",dir_ent->d_name);
        }
    } while(dir_ent);
    if(kclosedir(dir) == -1) {
        printk("kclosedir:: error closing directory [/bar/foodir]\n");
    } else {
        printk("kclosedir:: successfully closed dir [/bar/foodir]\n");
    }
}
#endif
