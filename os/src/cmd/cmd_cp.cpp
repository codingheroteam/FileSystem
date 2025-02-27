#include "cmd.h"
#include "../fs/fs.h"
#include <iostream>
using namespace std;


/**
 * @brief		cp命令，支持递归删除
 * @author		Peter
 * @date		2024-06
 */


// 调用该函数意味着当前已经转跳到源目录最后一级目录，需要手动转回原始路径
int chcpfile(char *sourse, char *destination) // cp文件的函数
{
    char temp[100]; // 记录当路径；
    strcpy(temp, PWD);
    int tfd = aopen(user_id, sourse, "r");
    if (tfd == INVALIDFD)
    {
        printerror("cp_invalid");
    }
    if (s_chdir(destination) == SYSCALL_FAIL)
    {
        close(user_id, tfd);
        printerror("cp_chcpfile");
        return -1;
    }
    unsigned int id = namei(sourse);
    if (id)
    {
        printf("目标路径存在与源文件同名的文件%s,跳过\n", sourse);
        close(user_id, tfd);
        return -1;
    }
    unsigned int nid = aopen(user_id, sourse, "w");
    int num;
    char buf[129];
    while ((num = s_read(tfd, buf, 128)) != 0)
    {
        s_write(nid, buf, num);
    }
    close(user_id, tfd);
    close(user_id, nid);
    printf("拷贝完成%s\n", sourse);
    s_chdir(temp);
    return 0;
}
int chcp_link(char *sourse, char *destination)
{
    char temp[100]; // 记录当路径；
    strcpy(temp, PWD);
    unsigned int id = namei(sourse);
    inode *inode = iget(id);
    char linkpath[1024];
    if (readlink(inode, linkpath) == SYSCALL_FAIL)
    {
        printerror("cp_link");
        iput(inode);
        return -1;
    }
    if (s_chdir(destination) == SYSCALL_FAIL)
    {
        printerror("cp_link");
        iput(inode);
        return -1;
    }
    iput(inode);
    char temp2[1024];
    sprintf(temp2, "%s/%s", destination, sourse);
    unsigned int id2 = namei(sourse);
    if (id2)
    {
        printf("目标路径存在与源文件同名的软连接%s,跳过\n", sourse);
        return -1;
    }
    if (softlink(linkpath, temp2) == SYSCALL_FAIL)
    {
        printerror("cp_link");
        return -1;
    }
    printf("拷贝完成%s\n", sourse);
    s_chdir(temp);
    return 0;
}

int chcpdir(char *sourse, char *destination) // cp路径的函数
{
    char temp[100]; // 记录当路径；
    strcpy(temp, PWD);

    if (s_chdir(destination) == SYSCALL_FAIL)
    {
        // printf("目标路径名无效，请检查重试\n");
        printerror("cp");
        return -1;
    }
    unsigned int id = namei(sourse);
    if (id)
    {
        printf("目标路径 %s 下已经存在同名路径 %s 拷贝取消，请检查重试\n", destination, sourse);
        s_chdir(temp);
        return -1;
    }
    if (s_mkdir(sourse) == SYSCALL_FAIL)
    {
        printerror("cp");
        return -1;
    }
    s_chdir(sourse);
    char destination2[100];
    strcpy(destination2, PWD);
    if (s_chdir(temp) == SYSCALL_FAIL)
    {
        printerror("cp");
        return -1;
    }
    if (s_chdir(sourse) == SYSCALL_FAIL)
    {
        printerror("cp");
        return -1;
    }
    struct dir dt = dir; // 记录当前的目录
    for (int i = 0; i < dt.size; i++)
    {
        if ((strcmp(dt.direct[i].d_name, ".\0") != 0) && (strcmp(dt.direct[i].d_name, "..\0") != 0))
        {
            unsigned int id = namei(dt.direct[i].d_name);
            inode *inode = iget(id);
            if ((inode->di_mode & DIFILE) == DIFILE)
            {
                iput(inode);
                chcpfile(dt.direct[i].d_name, destination2);
            }
            else if ((inode->di_mode & DIDIR) == DIDIR)
            {
                iput(inode);
                chcpdir(dt.direct[i].d_name, destination2);
            }
            else if ((inode->di_mode & DILINK) == DILINK)
            {
                iput(inode);
                chcp_link(dt.direct[i].d_name, destination2);
            }
        }
    }
    s_chdir(temp);
    return 0;
}
int charfront(char a[], char *buf)
{
    char _a[100];
    strcpy(_a, a);
    int e = -1;
    for (int i = 0; _a[i]; i++)
    {
        if (_a[i] == '/')
        {
            e = i;
        }
    }
    char *path = nullptr;
    if (e == 0)
    {
        path = "/";
    }
    else if (e > 0)
    {
        _a[e] = '\0';
        path = _a;
    }
    else if (e == -1)
    {
        return -1;
    }
    strcpy(buf, path);
    return 0;
}
void charbehind(char a[], char *buf)
{
    char _a[100];
    strcpy(_a, a);
    int e = -1;
    for (int i = 0; _a[i]; i++)
    {
        if (_a[i] == '/')
        {
            e = i;
        }
    }
    char *back = _a;
    if (e != -1)
    {
        back = a + e + 1;
    }
    strcpy(buf, back);
    return;
}

int cmd_cp(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: cp <directory>/<filename> <directory>\n");
        return -1;
    }
    char temp[100]; // 将当前路径暂存
    strcpy(temp, PWD);
    char temp2[100]; // 用于存储目标绝对路径
    if (s_chdir(argv[2]) == SYSCALL_FAIL)
    {
        printerror("cp");
        // printf("%s不是有效的文件/路径名，请重试\n", argv[2]);
        // s_chdir(temp);
        return -1;
    }
    strcpy(temp2, PWD);
    s_chdir(temp);
    char front1[100], behind1[100];
    if (charfront(argv[1], front1) == 0)
    {
        if (s_chdir(front1) == SYSCALL_FAIL)
        {
            // printf("%s不是有效的文件/路径名，请重试\n", argv[1]);
            printerror("cp");
            // printf("%s不是有效的文件/路径名，请重试\n", front1);
            s_chdir(temp);
            return -1;
        }
    }
    char temp1[100]; // 暂存原路径最后一级路径
    strcpy(temp1, front1);
    charbehind(argv[1], behind1);
    unsigned int id = namei(behind1);
    if (id) // 源文件或者路径确实存在
    {
        inode *inode = iget(id);
        if ((inode->di_mode & DIFILE) == DIFILE)
        {
            iput(inode);
            chcpfile(behind1, temp2);
        }
        else if ((inode->di_mode & DIDIR) == DIDIR)
        {
            iput(inode);
            chcpdir(behind1, temp2);
        }
        else if ((inode->di_mode & DILINK) == DILINK)
        {
            iput(inode);
            chcp_link(behind1, temp2);
        }
        s_chdir(temp);
        return 0;
    }
    printerror("cp");
    // printf("%s不是有效的文件/路径名，请重试\n", argv[1]);
    s_chdir(temp);
    return -1;

    // int fd = aopen(user_id, argv[1], "r");
    // if (fd == INVALIDFD)
    // {
    //     cout << "文件无效" << endl;
    //     return INVALIDFD;
    // }

    // if (s_chdir(argv[2]) == SYSCALL_FAIL)
    // {
    //     printf("目标路径名无效，请检查重试\n");
    //     return -1;
    // }
    // // unsigned int id=namei(argv[1]);
    // if (id)
    // {
    //     printf("目标路径存在与源文件同名的文件，跳过\n");
    // }
    // unsigned int nid = aopen(user_id, argv[1], "w");

    // int num;
    // char buf[129];
    // while ((num = s_read(fd, buf, 128)) != 0)
    // {
    //     s_write(nid, buf, num);
    // }
    // close(user_id, fd);
    // close(user_id, nid);
    // printf("拷贝完成\n");
    // s_chdir(temp);
    // return 0;
}