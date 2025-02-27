#include <stdio.h>
#include <string.h>
#include "fs.h"
#include <stdlib.h>



/* 
 * 安装文件系统 
 * By Brayden
 * 
 */



int install()
{
    debug("安装");
    user_id = USERNUM;

    int i, j;
    /* 0. 打开文件系统文件 */
    fd = fopen("filesystem", "r+b");
    if (fd == NULL)
    {
        seterror("未找到文件系统");
        return SYSCALL_FAIL;
    }
    /* 1. 从超级块中读取文件系统信息 */
    fseek(fd, BLOCKSIZ, SEEK_SET);
    fread(&filsys, 1, sizeof(struct filsys), fd);
    /* 2. 初始化索引节点哈希链 */
    for (i = 0; i < NHINO; i++)
    {
        hinode[i].i_forw = NULL;
    }
    /* 3. 初始化系统打开文件表 */
    for (i = 0; i < SYSOPENFILE; i++)
    {
        sys_ofile[i].f_count = 0;
        sys_ofile[i].f_inode = NULL;
    }
    /* 4. 初始化用户信息表 */
    for (i = 0; i < USERNUM; i++)
    {
        user[i].u_uid = 0;
        user[i].u_gid = 0;
        user[i].super = 0;
        for (j = 0; j < NOFILE; j++)
        {
            user[i].u_ofile[j] = SYSOPENFILE;
        }
    }
    /* 5. 读取主目录以初始化目录信息 */
    cur_path_inode = iget(ROOTINO);
    dir.size = cur_path_inode->di_size / DIRITEMSIZ;
    

    for (i = 0, j = 0; i < dir.size; i += BLOCKSIZ / DIRITEMSIZ, j++)
    {
        fseek(fd, DATASTART + cur_path_inode->di_addr[j] * BLOCKSIZ, SEEK_SET);
        fread(&dir.direct[i], 1, BLOCKSIZ, fd);
    };

    for (i = dir.size; i < DIRNUM; i++)
    {
        strcpy(dir.direct[i].d_name, "                 ");
        dir.direct[i].d_ino = 0;
    }
    return SYSCALL_SUCC;
}
