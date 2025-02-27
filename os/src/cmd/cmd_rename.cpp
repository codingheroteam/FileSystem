#include "cmd.h"
#include "../fs/fs.h"
#include <iostream>
using namespace std;

/**
* @brief		修改文件或文件夹名命令
* @author		Brayden
* @date		    2024-06
*/


// 修改文件或文件夹名
int cmd_rename(int argc, char *argv[])
{
    
    if (argc < 3) {
        cout << "无效的命令" << endl;
        cout << "用例：rename <oldname> <newname>" << endl;
        return -1;
    }

    if(rename(argv[1], argv[2]) != SYSCALL_SUCC) {
        printerror("rename");
    }

    return 0;
}
