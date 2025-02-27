#include "cmd.h"
#include "../fs/fs.h"
#include <iostream>
using namespace std;


/**
 * @brief		cat命令
 * @author		Brayden
 * @date		2024-06
 */


// 输出文件内容
int cmd_cat(int argc, char *argv[])
{
    int fd = aopen(user_id, argv[1], "r");
    if (fd == INVALIDFD)
    {
        printerror("cat");
        return INVALIDFD;
    }
    int num;
    char buf[300];
    while ((num = s_read(fd, buf, 200)) != 0)
    {
        for (int i = 0; i < num; i++)
        {
            cout << buf[i];
        }
    }
    close(user_id, fd);
    return 0;
}