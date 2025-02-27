#include <stdio.h>
#include "fs.h"

/* 
 * 文件关闭函数 
 * By Brayden
 * 
 */


void close(unsigned int user_id, unsigned short cfd)
{
    if(cfd >= INVALIDFD) {
        seterror("无效的文件描述符");
        return;
    }
    struct inode *inode;
    inode = sys_ofile[user[user_id].u_ofile[cfd]].f_inode;
    iput(inode);
    sys_ofile[user[user_id].u_ofile[cfd]].f_count--;
    user[user_id].u_ofile[cfd] = SYSOPENFILE;
}