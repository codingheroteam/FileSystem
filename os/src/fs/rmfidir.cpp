#include <stdio.h>
#include <string.h>
#include "fs.h"



/* 
 * 删除目录、文件或软链接 
 * By Brayden, Zachary
 * 
 */


int rmfile(char * name) 
{
    unsigned int ino = namei(name);
    if(ino == 0) {
        seterror("文件不存在");
        return SYSCALL_FAIL;
    }
    inode* inode = iget(ino);
    if(!(inode->di_mode & DIFILE)) {
        seterror("不是文件");
        iput(inode);
        return SYSCALL_FAIL;
    }

    if(!access(user_id, inode, WRITE)) {
        return SYSCALL_FAIL;
    }

    inode->di_number--;

    int i;

    for (i = 0; i < dir.size; i++)
    {
        if (!strcmp(dir.direct[i].d_name, name)) {
            break;
        }
    }
    for(i; i < dir.size - 1; i++) {
        strcpy(dir.direct[i].d_name ,dir.direct[i + 1].d_name);
        dir.direct[i].d_ino = dir.direct[i + 1].d_ino;
    }
    dir.size--;
    dir.direct[dir.size].d_ino = 0; 


    // iput(inode);
    if(inode->di_number > 0) {
        iput(inode);
        return SYSCALL_SUCC;
    }

    // for (i = getInodeBlockNum(inode) - 1; i >= 0; i--)
    // {
    //     bfree(inode->di_addr[i]);
    // }

    iput(inode);
    return SYSCALL_SUCC;
}

int rmdir(char *name) /* mkdir */
{
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
        seterror("非法目录名");
        return SYSCALL_FAIL;
    }
    unsigned int ino = namei(name);
    if(ino == 0) {
        seterror("目录不存在");
        return SYSCALL_FAIL;
    }
    inode* inode = iget(ino);
    if(!(inode->di_mode & DIDIR)) {
        seterror("不是目录");
        iput(inode);
        return SYSCALL_FAIL;
    }
    if((inode->di_size > 2 * (DIRSIZ + 4))) {
        iput(inode);
        seterror("目录非空");
        return SYSCALL_FAIL;
    }

    if(!access(user_id, inode, WRITE)) {
        return SYSCALL_FAIL;
    }

    inode->di_number--;

    int i;
    for (i = 0; i < dir.size; i++)
    {
        if (!strcmp(dir.direct[i].d_name, name)) {
            break;
        }
    }
    for(i; i < dir.size - 1; i++) {
        strcpy(dir.direct[i].d_name ,dir.direct[i + 1].d_name);
        dir.direct[i].d_ino = dir.direct[i + 1].d_ino;
    }

    dir.size--;
    dir.direct[dir.size].d_ino = 0; 

    // iput(inode);
    if(inode->di_number > 0) {
        iput(inode);
        return SYSCALL_SUCC;
    }


    // for (i = getInodeBlockNum(inode) - 1; i >= 0; i--)
    // {
    //     bfree(inode->di_addr[i]);
    // }

    iput(inode);
    return SYSCALL_SUCC;
}


int rmlink(const char * name) 
{
    unsigned int ino = namei(name);
    if(ino == 0) {
        seterror("文件不存在");
        return SYSCALL_FAIL;
    }
    inode* inode = iget(ino);
    if(!(inode->di_mode & DILINK)) {
        seterror("不是文件");
        iput(inode);
        return SYSCALL_FAIL;
    }

    if(!access(user_id, inode, WRITE)) {
        return SYSCALL_FAIL;
    }

    inode->di_number--;

    int i;

    for (i = 0; i < dir.size; i++)
    {
        if (!strcmp(dir.direct[i].d_name, name)) {
            break;
        }
    }
    for(i; i < dir.size - 1; i++) {
        strcpy(dir.direct[i].d_name ,dir.direct[i + 1].d_name);
        dir.direct[i].d_ino = dir.direct[i + 1].d_ino;
    }
    dir.size--;
    dir.direct[dir.size].d_ino = 0; 


    // iput(inode);
    if(inode->di_number > 0) {
        iput(inode);
        return SYSCALL_SUCC;
    }

    // for (i = getInodeBlockNum(inode) - 1; i >= 0; i--)
    // {
    //     bfree(inode->di_addr[i]);
    // }

    iput(inode);
    return SYSCALL_SUCC;
}