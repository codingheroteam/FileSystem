#include <stdio.h>
#include "fs.h"

/* 
 * 文件创建函数 
 * By Brayden
 * 
 */


// 创建空文件
int creat(unsigned int user_id, const char *filename, unsigned short mode)
{
    int ifd = aopen(user_id, filename, "w");
    if(ifd == SYSCALL_FAIL) {
        return SYSCALL_FAIL;
    }
    close(user_id, ifd);
    return SYSCALL_SUCC;
}