#include "cmd.h"
#include "../fs/fs.h"
#include <iostream>
using namespace std;

#include <iostream>
#include <string>


/**
 * @brief		用户相关操作命令
 * @author		Brayden
 * @date		2024-06
 */



#if defined(_WIN32) || defined(_WIN64)
#include <conio.h>
// Windows下密码输入函数
std::string getPassword()
{
    std::string password;
    char ch;

    while ((ch = _getch()) != '\r')
    { // '\r' is Enter key
        if (ch == '\b')
        { // Handle backspace
            if (!password.empty())
            {
                password.pop_back();
                std::cout << "\b \b";
            }
        }
        else
        {
            password.push_back(ch);
            std::cout << '*';
        }
    }
    std::cout << std::endl;
    return password;
}

#else

#include <termios.h>
#include <unistd.h>

// Linux下密码输入函数

void disableEcho()
{
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag &= ~ECHO;
    (void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void enableEcho()
{
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag |= ECHO;
    (void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

std::string getPassword()
{
    std::string password;

    disableEcho();
    std::cin >> password;
    enableEcho();
    std::cout << std::endl;

    return password;
}

#endif


// 登录
int cmd_login(int argc, char *argv[])
{
    cout << "请输入用户名: ";
    string user, pwd = "";
    cin >> user;
    cout << "请输入密码: ";
    pwd = getPassword();
    int idx = s_login(user.c_str(), pwd.c_str());
    if (idx == USERNUM)
    {
        cout << "登录失败" << endl;
        return -1;
    }
    user_id = idx;
    return 0;
}

// 登出
int cmd_logout(int argc, char *argv[])
{
    cout << "请输入用户名: ";
    string user;
    cin >> user;
    s_logout(user.c_str());
    return 0;
}


// 修改密码
int cmd_chpasswd(int argc, char *argv[])
{
    cout << "请输入新密码: ";
    string pwd = getPassword();

    for (int i = 0; i < sizeof(passwds) / sizeof(passwds[0]); i++)
    {
        if (strcmp(passwds[i].u_username, user[user_id].u_username) == 0)
        {
            strcpy(passwds[i].password, pwd.c_str());
            break;
        }
    }

    if (s_chdir("/etc") == SYSCALL_FAIL)
    {
        printerror("chpasswd");
    }

    int fd = aopen(user_id, "user", "w");
    if (s_write(fd, &passwds, sizeof(passwds)) == 0) {
        printerror("chpasswd");
    }
    close(user_id, fd);
    // fread(passwds, 1, sizeof(passwds), fd);

    s_chdir(OLDPWD);

    return 0;
}

// 修改用户名
int cmd_userrename(int argc, char *argv[])
{
    int curUserUid = user[user_id].u_uid;
    int tarUserUid = curUserUid;
    int flag_f = 0;

    if (argc < 2) {
        cout << "无效的命令" << endl;
        cout << "用例：urename <uid>" << endl;
        return -1;
    }


    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            for (int j = 0; argv[i][j]; j++)
            {
                if (argv[i][j] == 'f')
                {
                    flag_f = 1;
                }
            }
        }
        else
        {
            tarUserUid = atoi(argv[i]);
        }
    }
    

    if (curUserUid != tarUserUid && curUserUid != 1)
    {
        cout << "您没有权限" << endl;
        return -1;
    }

    if (tarUserUid == 1 && flag_f != 1)
    {
        cout << "不建议修改root用户的名字，若要修改，请使用[-f]参数" << endl;
        return -1;
    }

    cout << "请输入新用户名: ";
    string name;
    cin >> name;

    if(name.size() >= USERNAMESIZ) {
        cout << "用户名过长: " << name << endl;
        return -1;
    }

    int found = -1;
    for (int i = 0; i < sizeof(passwds) / sizeof(passwds[0]); i++)
    {
        if (passwds[i].p_uid == tarUserUid && passwds[i].p_uid != 0)
        {
            found = i;
        }
        if (strcmp(passwds[i].u_username, name.c_str()) == 0) {
            cout << "用户名已存在: " << name << endl;
            return -1;
        }
    }

    if(found < 1) {
        cout << "不存在的用户UID: " << tarUserUid << endl;
        return -1;
    }

    strcpy(passwds[found].u_username, name.c_str());
    
    if (s_chdir("/etc") == SYSCALL_FAIL)
    {
        printerror("chdir");
        return -1;
    }

    int fd = aopen(user_id, "user", "w");
    if(fd == INVALIDFD) {
        printerror("openfile");
        return -1;
    }

    s_write(fd, &passwds, sizeof(passwds));
    close(user_id, fd);
    s_chdir(OLDPWD);

    strcpy(user[user_id].u_username, name.c_str());

    cout << "修改完成" << endl;

    return 0;
}


// 修改用户所属用户组，仅root用户可操作
int cmd_changegroup(int argc, char *argv[])
{
    
    if (argc < 3) {
        cout << "无效的命令" << endl;
        cout << "用例：ugroup <uid> <gid>" << endl;
        return -1;
    }

    int curUserUid = user[user_id].u_uid;
    int tarUserUid = atoi(argv[1]);

    if (curUserUid != 1)
    {
        cout << "您没有权限" << endl;
        return -1;
    }

    int found = -1;
    for (int i = 0; i < sizeof(passwds) / sizeof(passwds[0]); i++)
    {
        if (passwds[i].p_uid == tarUserUid)
        {
            found = i;
        }
    }

    if(found == -1) {
        cout << "不存在的用户UID: " << tarUserUid << endl;
        return -1;
    }

    passwds[found].p_gid = atoi(argv[2]);
   
    if (s_chdir("/etc") == SYSCALL_FAIL)
    {
        printerror("chdir");
        return -1;
    }

    int fd = aopen(user_id, "user", "w");
    if(fd == INVALIDFD) {
        printerror("openfile");
        return -1;
    }

    s_write(fd, &passwds, sizeof(passwds));
    close(user_id, fd);
    s_chdir(OLDPWD);

    cout << "修改完成" << endl;

    return 0;
}


// 添加用户，仅root用户可以
int cmd_adduser(int argc, char *argv[])
{
    int curUserUid = user[user_id].u_uid;

    if (curUserUid != 1)
    {
        cout << "您没有权限" << endl;
        return -1;
    }

    string name;
    cout << "请输入用户名: ";
    cin >> name;
    string pwd;
    cout << "请输入密码: ";
    pwd = getPassword();
    int gid;
    cout << "请输入所属用户组: ";
    cin >> gid;
    int uid = 0;

    int found = -1;
    for (int i = 0; i < sizeof(passwds) / sizeof(passwds[0]); i++)
    {
        if (passwds[i].p_uid == 0 && found == -1)
        {
            found = i;
        }
        if (strcmp(passwds[i].u_username, name.c_str()) == 0) {
            cout << "用户名已存在: " << name << endl;
            return -1;
        }
    }

    if(found == -1) {
        cout << "无法添加：用户数量已满" << endl;
        return -1;
    }



    passwds[found].p_gid = gid;
    passwds[found].p_uid = found + 1;
    strcpy(passwds[found].password, pwd.c_str());
    strcpy(passwds[found].u_username, name.c_str());
   
    if (s_chdir("/etc") == SYSCALL_FAIL)
    {
        printerror("chdir");
        return -1;
    }

    int fd = aopen(user_id, "user", "w");
    if(fd == INVALIDFD) {
        printerror("openfile");
        return -1;
    }

    s_write(fd, &passwds, sizeof(passwds));
    close(user_id, fd);
    s_chdir(OLDPWD);

    cout << "修改完成" << endl;

    return 0;
}


// 切换用户
int cmd_su(int argc, char *argv[])
{
    if(argc < 2) {
        cout << "无效的命令" << endl;
        cout << "用例：su <username>" << endl;
        return -1;
    }
    string pwd = "";
    char* username = argv[1];

    cout << "请输入密码: ";
    pwd = getPassword();

    int found = -1;
    for (int i = 0; i < sizeof(user) / sizeof(user[0]); i++)
    {
        if (strcmp(user[i].u_username, username) == 0)
        {
            found = i;
        }
    }

    if(found == -1) {
        cout << "未找到用户: " << username << endl;
        return -1;
    }


    int foundpwd = -1;
    for (int i = 0; i < sizeof(passwds) / sizeof(passwds[0]); i++)
    {
        if (strcmp(passwds[i].u_username, username) == 0)
        {
            foundpwd = i;
        }
    }

    if(foundpwd == -1) {
        cout << "系统错误: 未找到用户: " << username << endl;
        return -1;
    }



    if(strcmp(pwd.c_str(), passwds[foundpwd].password) != 0) {
        cout << "密码错误" << endl;
        return -1;
    }

    user_id = found;
     
    return 0;
}
