#include <stdio.h>
#include "fs.h"


/* 
 * 打开文件 
 * By Brayden, Peter
 * 
 */


// "r" - 只读模式，打开文件用于读取，文件必须存在。
// "w" - 写模式，打开文件用于写入，如果文件已存在则清空文件内容，如果文件不存在则创建新文件。
// "a" - 追加模式，打开文件用于写入，如果文件不存在则创建新文件，如果文件已存在则在文件末尾追加内容。
// "r+" - 读写模式，打开文件用于读取和写入，文件必须存在。
// "w+" - 读写模式，打开文件用于读取和写入，如果文件已存在则清空文件内容，如果文件不存在则创建新文件。
// "a+" - 读写模式，打开文件用于读取和写入，如果文件不存在则创建新文件，如果文件已存在则在文件末尾追加内容。

unsigned short aopen(int user_id, const char *filename, const char *openmodestr)
{
    unsigned int openmode = 0;
    if (strstr(openmodestr, "r"))
    {
        openmode |= FREAD;
    }
    if (strstr(openmodestr, "w"))
    {
        openmode |= FWRITE;
    }
    if (strstr(openmodestr, "a"))
    {
        openmode |= FAPPEND;
    }
    if (strstr(openmodestr, "+"))
    {
        openmode |= FPLUS;
    }
    debug("想要以下面模式打开");
    debug(openmode);
    debug(filename);
    unsigned int dinodeid, di_ith;
    struct inode *inode;
    int i, j;
    dinodeid = namei(filename);

    // 若文件不存在
    if (dinodeid == 0)
    {
        // 以只读或读写模式打开
        if (!(openmode & FWRITE || openmode & FAPPEND))
        {
            seterror("文件不存在");
            return INVALIDFD;
        }

        // 其他模式，首先判断是否有在此目录新建文件的权限
        if (!access(user_id, cur_path_inode, WRITE))
        {
            seterror("您没有写入权限");
            return INVALIDFD;
        }

        // 有权限，则新建文件
        // 申请一个新的索引节点
        inode = ialloc();

        if (inode == 0)
        {
            return INVALIDFD;
        }

        // 申请一个新的目录项
        di_ith = iname(filename);
        if (di_ith == 0) {
            return INVALIDFD;
        }
        dir.size++;
        // 设置文件索引节点
        dir.direct[di_ith].d_ino = inode->i_ino;
        inode->di_mode = user[user_id].u_default_mode | DIFILE;
        inode->di_uid = user[user_id].u_uid;
        inode->di_gid = user[user_id].u_gid;
        inode->di_size = 0;
        inode->di_number = 1;
    }
    // 若文件存在
    else
    {
        inode = iget(dinodeid);

        // 如果是目录
        if (inode->di_mode & DIDIR)
        {
            seterror("已存在同名目录");
            return INVALIDFD;
        }

        // 如果是软链接
        if (inode->di_mode & DILINK)
        {
            // 读取软链接内容
            char _linkedfile[1024];
            fseek(fd, DATASTART + inode->di_addr[0] * BLOCKSIZ, SEEK_SET);
            fread(_linkedfile, inode->di_size, 1, fd);
            _linkedfile[inode->di_size] = '\0';
            // 首先进行跳转
            // 对目录进行路径和文件拆分
            int namestart = -1;
            for (int i = 0; _linkedfile[i]; i++)
            {
                if (_linkedfile[i] == '/')
                {
                    namestart = i;
                }
            }
            char *path = ".";
            char *file = _linkedfile;
            if (namestart != -1)
            {
                _linkedfile[namestart] = '\0';
                path = _linkedfile;
                file = _linkedfile + namestart + 1;
            }
            if (namestart == 0)
            {
                path = "/";
            }
            // 备份当前目录
            char backup[1024];
            strcpy(backup, PWD);
            // 首先进入被链接文件所在目录
            s_chdir(path);
            // 然后拿到该文件的索引节点号
            unsigned int ino = namei(file);
            // 然后返回原目录
            s_chdir(backup);
            iput(inode);
            inode = iget(ino);
            // 如果不是文件
            if ((inode->di_mode & DIFILE) == 0)
            {
                seterror("软链接指向的不是文件");
                iput(inode);
                return INVALIDFD;
            }
        }

        // 如果以只读或读写模式打开，判断是否有读权限
        if (openmode & FREAD && !access(user_id, inode, READ))
        {
            seterror("您没有读取权限");
            return INVALIDFD;
        }

        // 如果以写或追加或读写模式打开，判断是否有写权限
        if ((openmode & FWRITE || openmode & FAPPEND || openmode & FPLUS) && !access(user_id, inode, WRITE))
        {
            seterror("您没有写入权限");
            return INVALIDFD;
        }

        // 如果以写或读写模式打开，则要清空文件内容
        if (openmode & FWRITE)
        {
            // 释放文件所占用的数据块
            for (i = getInodeBlockNum(inode) - 1; i >= 0; i--)
            {
                bfree(inode->di_addr[i]);
            }
            // 在索引节点中更新文件大小
            inode->di_size = 0;
            // 如果这个文件已经被打开了，那么要将他们的偏移量都清零
            for (i = 0; i < SYSOPENFILE; i++)
            {
                if (sys_ofile[i].f_inode == inode)
                {
                    sys_ofile[i].f_off = 0;
                }
            }
        }
    }
    // 下面进行分配
    for (i = 0; i < SYSOPENFILE; i++)
    {
        if (sys_ofile[i].f_count == 0)
            break;
        // cout << "系统已打开文件" << i << "索引节点号" << sys_ofile[i].f_inode->i_ino << "引用数" << sys_ofile[i].f_count << endl;
    }

    if (i == SYSOPENFILE)
    {
        seterror("系统打开文件区已满");
        iput(inode);
        return INVALIDFD;
    }

    for (j = 0; j < NOFILE; j++)
    {
        if (user[user_id].u_ofile[j] == SYSOPENFILE)
            break;
        int idx = user[user_id].u_ofile[j];
        // cout << "用户已打开文件" << idx << "fd=" << j << "索引节点号" << sys_ofile[idx].f_inode->i_ino << "引用数" << sys_ofile[idx].f_count << endl;
    }

    if (j == NOFILE)
    {
        seterror("用户打开文件区已满");
        iput(inode);
        return -1;
    }

    sys_ofile[i].f_inode = inode;
    sys_ofile[i].f_flag = openmode;
    sys_ofile[i].f_count = 1;

    // 如果以追加模式打开，则需要将偏移量设置为文件末尾
    if (openmode & FAPPEND)
    {
        sys_ofile[i].f_off = inode->di_size;
        debug("以追加模式打开");
    }
    else
    {
        sys_ofile[i].f_off = 0;
        debug("以普通模式打开");
    }

    user[user_id].u_ofile[j] = i;
    return j;
}