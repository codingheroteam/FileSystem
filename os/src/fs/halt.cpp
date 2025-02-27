#include <stdio.h>
#include "fs.h"
#include <stdlib.h>

/* 
 * 退出系统 
 * By Brayden
 * 
 */

void halt()
{
    struct inode *inode;
    int i, j;
    // 将当前目录写入磁盘
    s_chdir((char *)"..");
    // 释放全局的当前路径节点
    iput(cur_path_inode);

    // 扫描索引节点哈希表，释放所有未释放节点
    for(i = 0; i < NHINO; i++) {
        struct inode* temp = hinode[i].i_forw;
        while(temp) {
            iput(temp);
            temp = hinode[i].i_forw;
        }
    }

    // 释放系统和用户文件表
    for (i = 0; i < USERNUM; i++)
    {
        if (user[i].u_uid != 0)
        {
            for (j = 0; j < NOFILE; j++)
            {
                if (user[i].u_ofile[j] != SYSOPENFILE)
                {
                    close(i, user[i].u_ofile[j]);
                    user[i].u_ofile[j] = SYSOPENFILE;
                }
            }
        }
    }
    
    // 写回超级块
    fseek(fd, BLOCKSIZ, SEEK_SET);
    fwrite(&filsys, 1, sizeof(struct filsys), fd);
    
    // 关闭虚拟磁盘
    fclose(fd);
    
    // 打印信息
    printf("CodingHero全体成员欢迎您的下次使用\n");
    exit(0);
}