#include "cmd.h"
#include "../fs/fs.h"
#include <iostream>
using namespace std;


/**
* @brief		软硬链接相关命令
* @author		Brayden
* @date		    2024-06
*/


// 创建硬链接
int cmd_hlk(int argc, char *argv[])
{
    if(hardlink(argv[1], argv[2]) == SYSCALL_FAIL) {
        printerror("slk");
        return -1;
    }
    return 0;
}


// 创建软链接
int cmd_slk(int argc, char *argv[])
{
    if(softlink(argv[1], argv[2]) == SYSCALL_FAIL) {
        printerror("slk");
        return -1;
    }
    return 0;
}


// 读取软链接内容
int cmd_rdlk(int argc, char *argv[])
{
    unsigned ino = namei(argv[1]);
    struct inode* inode = iget(ino);
    char temp[1024];
    if(readlink(inode, temp) == SYSCALL_FAIL) {
        printerror("readlink");
    }
    else {
        cout << temp << endl;
    }
    iput(inode);
    return 0;
}



// 删除软链接
int cmd_rmlk(int argc, char *argv[])
{
    rmlink(argv[1]);
    return 0;
}
