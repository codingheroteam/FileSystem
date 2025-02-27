#include "cmd.h"
#include "../fs/fs.h"
#include <iostream>
using namespace std;


/**
 * @brief		执行一个sh文件命令
 * @author		Brayden
 * @date		2024-06
 */

CmdManager cm;
int cmd_bash(int argc, char *argv[])
{
    cm.init();
    if (argc < 2)
    {
        printf("用例: bash <filename>\n");
        return -1;
    }
    char *path = argv[1];

    stringstream ss;

    int fd = aopen(user_id, argv[1], "r");
    if (fd == INVALIDFD)
    {
        cout << "文件无效" << endl;
        return INVALIDFD;
    }
    int num;
    char buf[129];
    while ((num = s_read(user_id, buf, 128)) != 0)
    {
        buf[num] = '\0';
        ss << buf;
    }
    close(user_id, fd);
    char temp[1024];
    while (ss.getline(temp, 1024))
    {
        cout << "执行" << temp << endl;
        cm.exec(temp);
    }
    return 0;
}


// echo命令
int cmd_echo(int argc, char *argv[])
{
    cout << argv[1] << endl;
    return 0;
}