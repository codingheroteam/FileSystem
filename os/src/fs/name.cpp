#include <string.h>
#include <stdio.h>
#include "fs.h"


/* 
 * 目录项操作 
 * By Brayden, Zachary
 * 
 */

// 给出文件或目录名，查找是否存在，若存在，返回索引节点号
unsigned int namei(const char *name)
{
    int i;
    for (i = 0; i < dir.size; i++)
    {
        if ((!strcmp(dir.direct[i].d_name, name)) && (dir.direct[i].d_ino != 0))
            return dir.direct[i].d_ino;
    }
    return 0;
};


// 创建目录
unsigned short iname(const char *name)
{
    int i, notfound = 1;
    for (i = 0; i < DIRNUM; i++)
        if (dir.direct[i].d_ino == 0)
        {
            notfound = 0;
            break;
        }

    if (notfound)
    {
        seterror("当前目录项已满");
        return 0;
    }
    else
    {
        strcpy(dir.direct[i].d_name, name);
        return i;
    }
}
