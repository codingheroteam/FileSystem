#include <stdio.h>
#include "fs.h"


/* 
 * 访问控制函数 
 * By Brayden
 * 
 */


// 访问权限检查
unsigned int access(unsigned int user_id, struct inode *inode, unsigned short mode)
{
    if(user_id >= USERNUM) {
        seterror("无效的用户标识符");
        return 0;
    }
    if(user[user_id].u_uid == ROOTUID) {
        return 1;
    }
    switch (mode)
    {
    case READ:
        if (inode->di_mode & ODIREAD)
            return 1;
        if ((inode->di_mode & GDIREAD) && (user[user_id].u_gid == inode->di_gid))
            return 1;
        if ((inode->di_mode & UDIREAD) && (user[user_id].u_uid == inode->di_uid))
            return 1;

    case WRITE:
        if (inode->di_mode & ODIWRITE)
            return 1;
        if ((inode->di_mode & GDIWRITE) && (user[user_id].u_gid == inode->di_gid))
            return 1;
        if ((inode->di_mode & UDIWRITE) &&
            (user[user_id].u_uid == inode->di_uid))
            return 1;

    case EXICUTE:

        if (inode->di_mode & ODIEXICUTE)
            return 1;
        if ((inode->di_mode & GDIEXICUTE) &&
            (user[user_id].u_gid == inode->di_gid))
            return 1;
        if ((inode->di_mode & UDIEXICUTE) &&
            (user[user_id].u_uid == inode->di_uid))
            return 1;
    }
    seterror("您没有权限");
    return 0;
}