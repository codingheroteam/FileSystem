#include "../cmd/cmd.h"
#include "../fs/fs.h"
#include <sstream>



/**
 * @brief		程序入口文件
 * @author		Brayden
 * @date		2024-06
 */



// 测试命令
void test(CmdManager cmdmgr)
{
    // char temp[100];
    // stringstream ss;
    // cmdmgr.exec("format -y");
    // cmdmgr.exec("mkdir a");
    // cmdmgr.exec("cd a");
    // cmdmgr.exec("mkdir b");
    // cmdmgr.exec("cd /");
    // cmdmgr.exec("slk a/b etc/lkb");
    // char temp[100];
    // format();
    // install();

    s_login("root", "admin");

    // fd = fopen("filesystem", "r+b");

    // struct inode * inode = iget(1);
    // iget(1);
    // iput(inode);
    // inode = iget(1);
    // iput(inode);
    // iput(inode);

    // cmdmgr.exec("slk 1.txt 1.link");
    // cmdmgr.exec("slk etc etc.link");
    // stringstream ss;
    // cmdmgr.exec("mkdir home");
    // cmdmgr.exec("cd ./home");
    // cmdmgr.exec("mkdir Brayden");
    // cmdmgr.exec("mkdir Peter");
    // cmdmgr.exec("mkdir Zachary");
    // cmdmgr.exec("hlk /etc/user /user.hlk");
    // cmdmgr.exec("slk /etc/user /user.slk");
    // // cmdmgr.exec("slk /etc/user /user.slk");
    // cmdmgr.exec("slk /etc/user /home/user.slk");

    // cmdmgr.exec("mkdir a");
    // cmdmgr.exec("cd a");
    // cmdmgr.exec("mkdir b");
    // cmdmgr.exec("login");
    // cmdmgr.exec("slk a/b etc/lkb");
    // for (int i = 0; i < 100; i++)
    // {
    //     string name = "file(";
    //     name += to_string(i);
    //     name += ").txt";
    //     char buf[1280] = "123456789";
    //     // cmdmgr.exec(cmd);
    //     int fd = aopen(user_id, name.c_str(), "w");
    //     if(fd == INVALIDFD) {
    //         printerror("test-open");
    //     }
    //     s_write(fd, buf, 120*5);
    //     close(user_id, fd);
    //     // s_mkdir(name.c_str());
    // }
    // int ifd = aopen(user_id, "1.txt", "w");
    // for(int i = 0; i < 10; i++) {
    //     s_write(ifd, "0123456789abcdefghijklmnopqrsquvwxyz", 36);
    // }
    // close(user_id, ifd);
}

// 空闲块测试
void test0(CmdManager cmdmgr)
{

    format();
    install();
    s_login("root", "admin");

    
    for (int i = 0; i < 100; i++)
    {
        unsigned int a = balloc();
        cout << a << endl;
    }
}

// 文件管理系统

int main()
{
    // FileSystemServer server(8089);
    // server.start();
    
    
    if(install() == SYSCALL_FAIL) {
        printerror("install");
        cout << "输入回车开始格式化" << endl;
        getchar();
        format();
        install();
    }



    // cout << "1";
    CmdManager cmdmgr;

    test(cmdmgr);

    cout << "欢迎使用文件管理系统" << endl;
    char temp[100];


    cout << cmdmgr.getuid() << "@"
         << "PC:" << cmdmgr.getpwd() << "$";
    while (cin.getline(temp, 100))
    {
        if (temp[0] != '\0')
        {
            // string vals[10];
            // char *argv[10];
            // stringstream ss;
            // ss << temp;
            // ss >> vals[0];
            // int idx = 1;
            // while (ss >> vals[idx++])
            //     ;
            // idx--;
            // for (int i = 0; i < idx; i++)
            // {
            //     argv[i] = (char *)vals[i].c_str();
            // }
            int result = cmdmgr.exec(temp);

            // if (result == -45656)
            // {
            //     cout << argv[0] << ":未找到命令" << endl;
            // }
        }
        cout << endl
             << cmdmgr.getuid() << "@"
             << "PC:" << cmdmgr.getpwd() << "$";
    }
    return 0;
}
