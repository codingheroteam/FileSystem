#include "cmd.h"
#include <iostream>
#include "../fs/fs.h"
using namespace std;

#include <cstdio>
#include <cstdlib>
#include <cstring>


/**
 * @brief		测试命令
 * @author		Brayden
 * @date		2024-06
 */


// 申请空闲索引节点测试
int cmd_test(int argc, char *argv[])
{
    format();
    install();
    
    inode * inode1 = ialloc();
    inode * inode2 = ialloc();  
    inode * inode3 = ialloc();  
    inode * inode4 = ialloc();  
    inode * inode5 = ialloc();
    ialloc();
    ialloc();
    ifree(inode1);
    ifree(inode2);
    ifree(inode3);
    ifree(inode4);
    ifree(inode5);
    // ifree(inode2);
    ialloc();  ialloc();  ialloc();  ialloc();
    ialloc();  ialloc();  ialloc();  ialloc();
    ialloc();  ialloc();  ialloc();  ialloc();
    
    // for (int i = 0; i < 10; i++)
    // {
    //     cout << i << endl;
    //     inode* inode = ialloc();
    //     ialloc();
    //     ialloc();
    //     ialloc();
    //     ifree(inode);
    // }
    return 0;
}

// 申请空闲块功能测试
int cmd_test2(int argc, char *argv[])
{
    // for (int i = 0; i < 20; i ++) {
    //     cout << i << endl;
    //     unsigned int a = balloc();
    //     balloc();
    //     balloc();
    //     balloc();
    //     balloc();
    //     a += rand() % 3;
    //     bfree(a);
    // }
    format();
    install();
    for (int i = 0; i < 15; i++)
    {
        string str;
        stringstream ss;
        ss << "folder" << i;
        ss >> str;
        s_mkdir(str.c_str());
    }
    return 0;
}

int cmd_test1(int argc, char *argv[])
{
    cout << "test" << endl;

    // fseek(fd, DATASTART + BLOCKSIZ * 10, SEEK_SET);
    // fwrite("123456", 1, 6, fd);

    // char buf[1025];
    // fseek(fd, DATASTART + BLOCKSIZ * 10, SEEK_SET);
    // int len = fread(buf, 1, 12, fd);

    // // int ifd = aopen(user_id, (char*)"test.txt", "w");
    // int num;
    // int wlen = s_write(fd, (char*)"12\03456\0\0\0\0\0", 12);
    // wlen = s_write(fd, (char*)"12\03456\0\0\0\0\0", 12);
    // cout << "写入长度" << wlen << endl;
    // // while((num = s_write(user_id, buf, atoi(argv[2]))) != 0) {
    // //     buf[num] = 0;
    // //     cout << buf;
    // // }
    // close(user_id, fd);

    int fd = aopen(user_id, (char *)"test.txt", "w");

    char buf[2048] = "12345678901234567890123456789012345678901234567890";

    for (int i = 0; i < 5; i++)
    {
        cout << "第" << i << "次写入" << endl;
        s_write(fd, buf, 500);
    }

    close(user_id, fd);

    fd = aopen(user_id, (char *)"test.txt", "r");

    int len;
    while ((len = s_read(fd, buf, 4000)) != 0)
    {
        cout << "长度" << len << endl;
        cout << "内容" << endl;
        for (int i = 0; i < len; i++)
        {
            cout << buf[i];
        }
        cout << endl;
    }

    // while((num = s_write(user_id, buf, atoi(argv[2]))) != 0) {
    //     buf[num] = 0;
    //     cout << buf;
    // }
    close(user_id, fd);

    return 0;
}

int cmd_test0(int argc, char *argv[])
{
    cout << "test" << endl;

    // fseek(fd, DATASTART + BLOCKSIZ * 10, SEEK_SET);
    // fwrite("123456", 1, 6, fd);

    // char buf[1025];
    // fseek(fd, DATASTART + BLOCKSIZ * 10, SEEK_SET);
    // int len = fread(buf, 1, 12, fd);

    // // int ifd = aopen(user_id, (char*)"test.txt", "w");
    // int num;
    // int wlen = s_write(fd, (char*)"12\03456\0\0\0\0\0", 12);
    // wlen = s_write(fd, (char*)"12\03456\0\0\0\0\0", 12);
    // cout << "写入长度" << wlen << endl;
    // // while((num = s_write(user_id, buf, atoi(argv[2]))) != 0) {
    // //     buf[num] = 0;
    // //     cout << buf;
    // // }
    // close(user_id, fd);

    s_chdir("/etc");
    int fd = aopen(user_id, (char *)"user", "r");

    char buf[1025];

    int len;
    while ((len = s_read(fd, buf, 200)) != 0)
    {
        cout << "长度" << len << endl;
        cout << "内容" << endl;
        for (int i = 0; i < len; i++)
        {
            cout << buf[i];
        }
        cout << endl;
    }

    // while((num = s_write(user_id, buf, atoi(argv[2]))) != 0) {
    //     buf[num] = 0;
    //     cout << buf;
    // }
    close(user_id, fd);

    return 0;
}




// 打印已打开文件信息
int cmd_pof(int argc, char* argv[]) {
    int i, j;
    for (i = 0; i < SYSOPENFILE; i++)
    {
        if (sys_ofile[i].f_count == 0)
            break;
        cout << "系统已打开文件\tidx=" << i << "\t索引节点号" << sys_ofile[i].f_inode->i_ino << "\t指针位置" << sys_ofile[i].f_off << endl;
    }

    if (i == SYSOPENFILE)
    {
        printf("系统打开文件区已满\n");
        return 0;
    }

    /* alloc the user open file item */
    for (j = 0; j < NOFILE; j++)
    {
        if (user[user_id].u_ofile[j] == SYSOPENFILE)
            break;
        int idx = user[user_id].u_ofile[j];
        cout << "用户已打开文件\tidx=" << idx << "\tfd=" << j << "\t索引节点号" << sys_ofile[idx].f_inode->i_ino << endl;
    }

    if (j == NOFILE)
    {
        printf("用户打开文件区已满\n");
        return 0;
    }
    return 0;
}


// 用户信息
int cmd_pu(int argc, char* argv[]) {

    inode* inode = iget(3);
    fseek(fd, DATASTART+BLOCKSIZ*inode->di_addr[0], SEEK_SET);
    fread(passwds, 1, sizeof(passwds), fd);
    iput(inode);


    cout << "所有用户：" << endl;
    cout << "idx\tuid\tgid\tname\tpassword" << endl;
    for(int i = 0; i < sizeof(passwds) / sizeof(passwds[0]); i++) {
        if(passwds[i].p_uid == 0) continue;
        cout << i << "\t";
        cout << passwds[i].p_uid << "\t";
        cout << passwds[i].p_gid << "\t";
        cout << passwds[i].u_username << "\t";
        cout << passwds[i].password << "\n";
    }
    cout << "已登录用户：" << endl;
    cout << "idx\tuid\tgid\tname" << endl;
    for(int i = 0; i < sizeof(user) / sizeof(user[0]); i++) {
        if(user[i].u_uid == 0) continue;
        cout << i << "\t";
        cout << user[i].u_uid << "\t";
        cout << user[i].u_gid << "\t";
        cout << user[i].u_username << "\n";
    }
    return 0;
}


// 打印系统信息
int cmd_psys(int argc, char *argv[])
{
    cout << "索引区块数：" << filsys.s_isize << endl;
    cout << "数据区块数：" << filsys.s_fsize << endl;
    cout << "块大小：" << BLOCKSIZ << endl;
    cout << "空闲索引节点数：" << filsys.s_ninode << endl;
    cout << "空闲块数：" << filsys.s_nfree << endl;
    cout << "空间使用量：" << (filsys.s_fsize - filsys.s_nfree) * BLOCKSIZ / 1024 << "B/" <<  (filsys.s_fsize) * BLOCKSIZ / 1024 << "B" << endl;
    return 0;
}

const char* HELPTXT = "读我文件"; 
const char* FSH = "fs头文件"; 
const char* READMETXT = "帮助文档\n\
# 操作系统课程设计-文件系统\n\
开发组成员：\n\
Brayden\n\
Zachary\n\
Peter\n\
# 功能说明\n\
>\n\
> 下面描述可能未及时更新，请以程序内 `help` 指令输出为准\n\
>\n\
## 基本命令\n\
`login`                     用户登入\n\
`mkdir <newdirname>`        在当前目录下新建目录\n\
`cd <directory>`            进入指定路径\n\
`ls`                        打印当前目录下的内容\n\
`create <filename>`         创建文件\n\
`open <filename> <openmode>`            打开文件\n\
`write <filedescriptor> <txt> <size>`   写入文件\n\
`read <filedescriptor>`                 读取文件内容\n\
`close <filedescriptor>`    关闭文件\n\
`rmfile <filename>`         删除文件\n\
`rmdir <directory>`         删除空目录\n\
`logout`                    用户登出\n\
`format`                    系统格式化\n\
`halt`                      系统保存退出\n\
`help`                      若无额外参数，显示所有的指令及其格式；若有额外参数，则显示指定参数的指令格式及其详细说明\n\
## 测试命令\n\
`pof`           打印所有已打开的文件\n\
`pu`            打印所有用户信息\n\
`psys`          打印超级块信息\n\
`loaddata`      载入样例数据\n\
## 新增功能\n\
`cat <filename>`                将文件内容打印至终端\n\
`chmod <mode> <filename>`       更改权限模式\n\
`cp`                            将指定内容复制到指定路径,支持目录到目录的复制\n\
`hardlink <linkedfile> <newfile>`   创建指定文件的硬链接\n\
`symbollink <source> <linkname>`    创建指定源的软链接\n\
`readlink <link>`               读取链接内容\n\
`rmlink <link>`                 删除软链接\n\
`rename <filename> <newfilename>`   文件重命名\n\
`rm [-r/rf] <path>`             删除路径的指定内容，`[-r/rf]`为可选参数，代表递归删除\n\
`writer <filename>`             编辑指定文件的内容，若文件不存在则直接新建文件\n\
`save <filename> <text> <size>` 将指定文本保存至指定文件内\n\
## 用户相关\n\
`chpasswd`                  修改密码\n\
`urename <username>`        修改指定用户的用户名\n\
`chgroup`                   修改用户所属用户组，仅root用户可操作\n\
`adduser`                   用于添加用户，仅root用户有此权限\n\
`su <username>`             切换到指定用户\n\
## bash命令\n\
`bash <filename>`   用于执行文件内容\n\
`echo <txt>`        直接输出文本内容\n\
"; 
const char* TESTSH = "echo 进入根目录\ncd /\necho 创建cdtest文件夹\nmkdir cdtest\necho 进入cdtest文件夹\ncd /cdtest";
const char* LONGTXT = "春江潮水连海平，海上明月共潮生。\n\
滟滟随波千万里，何处春江无月明。\n\
江流宛转绕芳甸，月照花林皆似霰。\n\
空里流霜不觉飞，汀上白沙看不见。\n\
江天一色无纤尘，皎皎空中孤月轮。\n\
江畔何人初见月？江月何年初照人？\n\
人生代代无穷已，江月年年望相似。\n\
不知江月待何人，但见长江送流水。\n\
白云一片去悠悠，青枫浦上不胜愁。\n\
谁家今夜扁舟子？何处相思明月楼？\n\
可怜楼上月裴回，应照离人妆镜台。\n\
玉户帘中卷不去，捣衣砧上拂还来。\n\
此时相望不相闻，愿逐月华流照君。\n\
鸿雁长飞光不度，鱼龙潜跃水成文。\n\
昨夜闲潭梦落花，可怜春半不还家。\n\
江水流春去欲尽，江潭落月复西斜。\n\
斜月沉沉藏海雾，碣石潇湘无限路。\n\
不知乘月几人归，落月摇情满江树。";


// 加载样例数据
int cmd_loaddata(int argc, char *argv[]) {
    format();
    install();
    s_login("root", "admin");
    s_chdir("/");
    // 根目录创建 读我.txt
    int ifd = aopen(user_id, "读我.md", "w");
    s_write(ifd, READMETXT, strlen(READMETXT));
    close(user_id, ifd);
    // 创建家目录和公共目录
    s_mkdir("home");
    // 设置权限
    chmod("home", ALLPERMIT);
    s_chdir("home");
    s_mkdir("Public");
       
    // 创建用户目录
    s_mkdir("user");
    chmod("user", ALLPERMIT);
    
    // 在用户目录中创建文件
    s_chdir("user");
    int ufd = aopen(user_id, "userfile.txt", "w");
    const char *usertext = "This is a user file.";
    s_write(ufd, usertext, strlen(usertext));
    close(user_id, ufd);
    
    // 回到根目录
    s_chdir("/");
    
    // 创建符号链接和硬链接
    // s_login("root", "admin");
    
    // 符号链接
    softlink("/home/user/userfile.txt", "/home/user/userfile_link.txt");
    
    // 硬链接
    hardlink("/home/user/userfile.txt", "/home/Public/public_hardlink.txt");
    
    // 创建公共目录中的文件
    s_chdir("/home/Public");
    int pfd = aopen(user_id, "publicfile.txt", "w");
    const char *publictext = "This is a public file.";
    s_write(pfd, publictext, strlen(publictext));
    close(user_id, pfd);
    
    // 创建符号链接指向公共文件
    softlink("/home/Public/publicfile.txt", "/home/Public/publicfile_link.txt");
    
    s_logout("root");
    
    // 使用对应用户创建对应目录
    s_login("Brayden", "123456");
    s_chdir("/home");
    s_mkdir("Brayden");
    s_chdir("/home/Brayden");
    ifd = aopen(user_id, "fs.h", "w");
    s_write(ifd, FSH, strlen(FSH));
    close(user_id, ifd);
    ifd = aopen(user_id, "test.sh", "w");
    s_write(ifd, TESTSH, strlen(TESTSH));
    close(user_id, ifd);
    ifd = aopen(user_id, "春江花月夜.txt", "w");
    s_write(ifd, LONGTXT, strlen(LONGTXT));
    close(user_id, ifd);
    s_logout("Brayden");

    s_login("Zachary", "123456");
    s_chdir("/home");
    s_mkdir("Zachary");
    s_chdir("/home/Zachary");
    int zfd = aopen(user_id, "Zachary.txt", "w");
    const char *Zachary_text = "This is Zachary's file.";
    s_write(zfd, Zachary_text, strlen(Zachary_text));
    s_logout("Zachary");

    s_login("Peter", "123456");
    s_chdir("/home");
    s_mkdir("Peter");
    s_chdir("/home/Peter");
    s_chdir("/");
    s_logout("Peter");

    cout << "数据加载完成，请登录" << endl; 
    return 0;
}
