#include "cmd.h"
#include "../fs/fs.h"


/**
 * @brief		命令管理器类源文件
 * @author		Brayden
 * @date		2024-06
 */


CmdManager::CmdManager()
{
    init();
}

// 初始化
void CmdManager::init()
{

    // 基本命令
    CmdManager::add("login", cmd_login);
    CmdManager::add("mkdir", cmd_mkdir);
    CmdManager::add("cd", cmd_cd);
    CmdManager::add("ls", cmd_ls);
    CmdManager::add("create", cmd_create);
    CmdManager::add("open", cmd_open);
    CmdManager::add("write", cmd_write);
    CmdManager::add("read", cmd_read);
    CmdManager::add("close", cmd_close);
    CmdManager::add("rmfile", cmd_rmfile);
    CmdManager::add("rmdir", cmd_rmdir);
    CmdManager::add("logout", cmd_logout);
    CmdManager::add("format", cmd_format);
    CmdManager::add("halt", cmd_halt);
    CmdManager::add("ht", cmd_halt);
    CmdManager::add("help", cmd_help);

    // 测试命令
    CmdManager::add("test", cmd_test);
    CmdManager::add("pof", cmd_pof);
    CmdManager::add("pu", cmd_pu);
    CmdManager::add("psys", cmd_psys);
    CmdManager::add("loaddata", cmd_loaddata);

    // 新增命令
    CmdManager::add("cat", cmd_cat);
    CmdManager::add("chmod", cmd_chmod);
    CmdManager::add("cp", cmd_cp);
    CmdManager::add("hardlink", cmd_hlk);
    CmdManager::add("symbollink", cmd_slk);
    CmdManager::add("hlk", cmd_hlk);
    CmdManager::add("slk", cmd_slk);
    CmdManager::add("readlink", cmd_rdlk);
    CmdManager::add("rmlink", cmd_rmlk);
    CmdManager::add("rename", cmd_rename);
    CmdManager::add("rm", cmd_rm);
    CmdManager::add("writer", cmd_writer);
    CmdManager::add("save", cmd_save);

    // 用户相关
    CmdManager::add("chpasswd", cmd_chpasswd);
    CmdManager::add("urename", cmd_userrename);
    CmdManager::add("chgroup", cmd_changegroup);
    CmdManager::add("adduser", cmd_adduser);
    CmdManager::add("su", cmd_su);

    // bash命令
    CmdManager::add("bash", cmd_bash);
    CmdManager::add("echo", cmd_echo);
}

// 添加命令
void CmdManager::add(string cmd, int (*func)(int argc, char *argv[]))
{
    CmdManager::cmds[cmdNum].cmd = cmd;
    CmdManager::cmds[cmdNum].func = func;
    CmdManager::cmdNum++;
}

// 执行命令
int CmdManager::exec(string cmd, int argc, char *argv[])
{
    for (int i = 0; i < CmdManager::cmdNum; i++)
    {
        if (CmdManager::cmds[i].cmd == cmd)
        {
            int result = CmdManager::cmds[i].func(argc, argv);
            return result;
        }
    }
    return -45656;
}

int CmdManager::exec(string code)
{
    string vals[10];
    char *argv[10];
    stringstream ss;
    ss << code;
    ss >> vals[0];
    int idx = 1;
    while (ss >> vals[idx++])
        ;
    idx--;
    for (int i = 0; i < idx; i++)
    {
        argv[i] = (char *)vals[i].c_str();
    }
    int result = exec(argv[0], idx, argv);
    if (result == -45656)
    {
        cout << argv[0] << ":未找到命令" << endl;
    }
    return result;
}