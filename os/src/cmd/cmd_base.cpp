#include "cmd.h"
#include "../fs/fs.h"
#include <iostream>
using namespace std;

/**
 * @brief		基本命令
 * @author		Brayden, Zachary
 * @date		2024-06
 */

// 创建文件夹
int cmd_mkdir(int argc, char *argv[])
{
    if (s_mkdir(argv[1]) == SYSCALL_SUCC)
    {
        cout << "创建目录：" << argv[1] << endl;
    }
    else
    {
        printerror("mkdir");
    }
    return 0;
}

// 路径跳转（支持多级复杂路径）
int cmd_cd(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: cd <directory>\n");
        return -1;
    }
    char *path = argv[1];

    if (s_chdir(argv[1]) == SYSCALL_FAIL)
    {
        printerror("cd");
        return -1;
    }
    return 0;
}

// 列出当前目录下内容
int cmd_ls(int argc, char *argv[])
{
    _dir();
    return 0;
}

// 创建文件
int cmd_create(int argc, char *argv[])
{
    if(argc < 2) {
        cout << "请通过 help 命令查看用法" << endl;
        return -1;
    }
    int fd = creat(user_id, argv[1], 10086);
    //cout << "fd is " << fd << endl;
    cout<<"文件"<<argv[1]<<"已创建"<<endl;
    return 0;
}

// 打开文件
int cmd_open(int argc, char *argv[])
{
    if(argc < 3) {
        cout << "请通过 help 命令查看用法" << endl;
        return -1;
    }
    int fd = aopen(user_id, argv[1], argv[2]);
    cout << "fd is " << fd << endl;
    if(fd == INVALIDFD) {
        printerror("open");
        return -1;
    }
    return 0;
}

// 写入
int cmd_write(int argc, char *argv[])
{
    if(argc < 4) {
        cout << "请通过 help 命令查看用法" << endl;
        return -1;
    }
    int len = s_write(atoi(argv[1]), argv[2], atoi(argv[3]));
    cout << "写入长度：" << len << endl;
    if(atoi(argv[3]) != 0 && len == 0) {
        printerror("write");
        return -1;
    }
    return 0;
}

// 读取
int cmd_read(int argc, char *argv[])
{
    if(argc < 2) {
        cout << "请通过 help 命令查看用法" << endl;
        return -1;
    }
    cout << "fd is " << atoi(argv[1]) << endl;
    char buf[100];
    int size = s_read(atoi(argv[1]), buf, 100);
    cout << "读取大小为" << size << "读取内容为" << endl;
    cout << "----------------------------------------" << endl;
    for (int i = 0; i < size; i++)
    {
        cout << buf[i];
    }
    cout << endl
         << "----------------------------------------" << endl;
    return 0;
}

// 关闭文件
int cmd_close(int argc, char *argv[])
{
    if(argc < 2) {
        cout << "请通过 help 命令查看用法" << endl;
        return -1;
    }
    close(user_id, atoi(argv[1]));
    return 0;
}

// 格式化
int cmd_format(int argc, char *argv[])
{
    int flay_yes = 0;
    if (argc == 2 && strcmp(argv[1], "-y") == 0)
    {
        flay_yes = 1;
    }
    char ans;
    if (!flay_yes)
    {
        cout << "确定要进行格式化吗，所有数据将丢失[y/N]：";
        ans = getchar();
    }
    if (ans == 'y' || flay_yes)
    {
        s_chdir("/");
        format();
        install();
    }
    else
    {
        cout << "已取消" << endl;
    }
    return 0;
}

// 退出系统
int cmd_halt(int argc, char *argv[])
{
    halt();
    return 0;
}