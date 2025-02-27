#include "fs.h"
#include <string>


/* 
 * 更改权限模式 
 * By Brayden, Zachary
 * 
 */



int chmod(char *dirname, unsigned int mode)
{

    unsigned int dirid;
    struct inode *inode;
    unsigned short block;
    int i, j, low = 0, high = 0;

    dirid = namei(dirname);
    if (dirid == 0)
    {
        seterror("文件或目录不存在");
        return SYSCALL_FAIL;
    }
    inode = iget(dirid);


    // 权限检查
    if (!access(user_id, inode, WRITE))
    {
        seterror("您没有写入权限");
        return SYSCALL_FAIL;
    }


    inode->di_mode = mode | (inode->di_mode & 0xF000);
    iput(inode);

    return SYSCALL_SUCC;
}