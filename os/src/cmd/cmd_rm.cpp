#include "cmd.h"
#include "../fs/fs.h"
#include <iostream>
using namespace std;

/**
 * @brief		rm命令，支持-rf参数，可递归删除
 * @author		Zachary
 * @date		2024-06
 */

int remove_entry(char *name, int recursive);

int cmd_rm(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: rm [-r/rf] <path>\n");
        return -1;
    }

    int recursive = 0;
    char *path;

    if (argc == 3 && (strcmp(argv[1], "-r") == 0 || strcmp(argv[1], "-rf") == 0))
    {
        recursive = 1;
        path = argv[2];
    }
    else if (argc == 2)
    {
        path = argv[1];
    }
    else
    {
        printf("Usage1: rm [-r/rf] <path>\n");
        return -1;
    }

    remove_entry(path, recursive);

    return 0;
}

int remove_entry(char *name, int recursive)
{
    unsigned int ino = namei(name);
    if (ino == 0)
    {
        printf("不存在\n");
        return SYSCALL_FAIL;
    }

    inode *inode = iget(ino);
    if (inode->di_mode & DIFILE)
    {
        //文件
        if (rmfile(name) == SYSCALL_SUCC) {
            iput(inode);
            return SYSCALL_SUCC;
        }
        else
        {
            iput(inode);
            printerror("rm");
            return SYSCALL_FAIL;
        }
    }
    if (inode->di_mode & DILINK)
    {
        //文件
        if (rmlink(name) == SYSCALL_SUCC) {
            iput(inode);
            return SYSCALL_SUCC;
        }
        else
        {
            iput(inode);
            printerror("rm");
            return SYSCALL_FAIL;
        }
    }
    else if (inode->di_mode & DIDIR)
    {
        // 文件夹
        int i; //当前指向的文件指针
        // cout<<"inode->di_size:"<<inode->di_size;
        if (inode->di_size > 2 * (DIRSIZ + 4))
        {
            // 文件夹非空
            if (!recursive)
            {
                printf("目录非空，无法删除\n");
                iput(inode);
                return SYSCALL_FAIL;
            }
            iput(inode);
            char original_path[256];
            strcpy(original_path, PWD);
            s_chdir(name);
            //递归删除
            for (i = 0; i < dir.size; i++)
            {
                if (dir.direct[i].d_ino != 0 && strcmp(dir.direct[i].d_name, ".") != 0 && strcmp(dir.direct[i].d_name, "..") != 0)
                {
                    //构建完整删除路径
                    char full_path[256];
                    snprintf(full_path, sizeof(full_path), "%s/%s", name, dir.direct[i].d_name);
                    if (remove_entry(dir.direct[i].d_name, recursive) == SYSCALL_SUCC)
                    {
                        i--;
                    }
                }
            }
            s_chdir(original_path);
        }
        else {
            iput(inode);
        }
        //最后删除已经为空的当前文件夹
        if (rmdir(name) == SYSCALL_SUCC)
            return SYSCALL_SUCC;
        else
        {
            printerror("rm");
            return SYSCALL_FAIL;
        }
    }
    else
    {
        printf("未知类型\n");
    }
}


int cmd_rmfile(int argc, char* argv[]) {
    rmfile(argv[1]);
    return 0;
}

int cmd_rmdir(int argc, char* argv[]) {
    rmdir(argv[1]);
    return 0;
}