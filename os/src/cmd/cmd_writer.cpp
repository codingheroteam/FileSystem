
#include <iostream>
#include <cstring>
#include <unistd.h>
#include "../fs/fs.h"
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#endif
using namespace std;

/**
 * @brief		逐行写入器
 * @author		Brayden
 * @date		2024-06
 */


#ifdef _WIN32
// Windows下用于读取单个字符的函数
int getch()
{
    return _getch();
}
#else
// Linux下用于读取单个字符的函数
int getch()
{
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

int cmd_writer(int argc, char *argv[])
{

    int fd = aopen(user_id, argv[1], "w");

    if (fd == INVALIDFD)
    {
        printerror("write");
        return SYSCALL_FAIL;
    }
    char buf[1026];
    int firstLine = 1;
    int i = 0;
    cout << "欢迎使用写入器" << endl;
    cout << "已打开" << argv[1] << "文件" << endl;
    cout << "点击 Esc 来保存并退出" << endl;
    int line = 1;
    cout << "请写入第" << line++ << "行：";
    while (true)
    {
        int ch = getch();
        if (ch == 27)
        { // 检测到Esc键
            if (i > 0)
            { // 如果缓冲区有未写入的数据
                buf[i] = '\0';
                if (!firstLine)
                {
                    s_write(user_id, (char *)"\n", 1);
                }
                s_write(user_id, buf, strlen(buf));
            }
            break;
        }
        else if (ch == '\n' || ch == '\r')
        { // 换行符
            putchar(ch);
            buf[i] = '\0';
            if (!firstLine)
            {
                s_write(user_id, (char *)"\n", 1);
            }
            s_write(user_id, buf, strlen(buf));
            firstLine = 0;
            i = 0; // 重置缓冲区索引
            cout << "请写入第" << line++ << "行：";
        }
        else
        {
            buf[i++] = ch;
            putchar(ch);
            if (i >= 1024)
            {
                buf[i] = '\0';
                if (!firstLine)
                {
                    s_write(user_id, (char *)"\n", 1);
                }
                s_write(user_id, buf, strlen(buf));
                firstLine = 0;
                i = 0; // 重置缓冲区索引
            }
        }
    }

    close(user_id, fd);
    return 0;
}


// 保存内容 save 1.txt 5 "abcde"
int cmd_save(int argc, char *argv[])
{
    int fd = aopen(user_id, argv[1], "w");
    if(fd == INVALIDFD) {
        printerror("save");
        // return -1;
    }
    int num;
    s_write(fd, argv[3], atoi(argv[2]));
    close(user_id, fd);
    return 0;
}