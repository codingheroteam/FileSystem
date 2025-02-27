#ifndef CMD_H
#define CMD_H
#include <iostream>
#include "../fs/fs.h"
using namespace std;

/**
 * @brief		命令管理器类，可方便地进行命令扩展
 * @author		Brayden
 * @date		2024-06
 */


class CmdManager
{
public:
    struct CMD
    {
        string cmd;
        int (*func)(int argc, char *argv[]);
    };

    CmdManager();

    // 初始化
    void init();

    // 添加命令
    void add(string cmd, int (*func)(int argc, char *argv[]));

    // 执行命令
    int exec(string cmd, int argc, char *argv[]);

    int exec(string code);

    // 获取当前工作路径
    inline string getpwd()
    {
        return PWD;
    }

    // 获取用户id
    string getuid()
    {
        if (user_id >= USERNUM)
        {
            return "traveler";
        }

        return user[user_id].u_username;
    }

private:
    CMD cmds[1000];
    int cmdNum;
};


// 基本命令
int cmd_login(int argc, char *argv[]);
int cmd_mkdir(int argc, char *argv[]);
int cmd_cd(int argc, char *argv[]);
int cmd_ls(int argc, char *argv[]);
int cmd_create(int argc, char *argv[]);
int cmd_open(int argc, char *argv[]);
int cmd_write(int argc, char *argv[]);
int cmd_read(int argc, char *argv[]);
int cmd_close(int argc, char *argv[]);
int cmd_rmfile(int argc, char *argv[]);
int cmd_rmdir(int argc, char *argv[]);
int cmd_logout(int argc, char *argv[]);
int cmd_format(int argc, char *argv[]);
int cmd_halt(int argc, char *argv[]);
int cmd_help(int argc, char* argv[]);

// 测试命令
int cmd_test(int argc, char *argv[]);
int cmd_pof(int argc, char *argv[]);
int cmd_pu(int argc, char *argv[]);
int cmd_psys(int argc, char *argv[]);
int cmd_loaddata(int argc, char *argv[]) ;

// 新增功能
int cmd_cat(int argc, char *argv[]);
int cmd_chmod(int argc, char *argv[]);
int cmd_cp(int argc, char* argv[]);
int cmd_hlk(int argc, char *argv[]);
int cmd_slk(int argc, char *argv[]);
int cmd_rdlk(int argc, char *argv[]);
int cmd_rmlk(int argc, char *argv[]);
int cmd_rename(int argc, char *argv[]);
int cmd_rm(int argc, char *argv[]);
int cmd_writer(int argc, char *argv[]);
int cmd_save(int argc, char *argv[]);

// 用户相关
int cmd_chpasswd(int argc, char *argv[]);
int cmd_userrename(int argc, char *argv[]);
int cmd_changegroup(int argc, char *argv[]);
int cmd_adduser(int argc, char *argv[]);
int cmd_su(int argc, char *argv[]);

// bash命令
int cmd_bash(int argc, char *argv[]);
int cmd_echo(int argc, char *argv[]);


#endif