#include <stdio.h>
#include <string.h>
#include "fs.h"

struct dir temp_dir;
struct inode *temp_cur_path_inode;
std::vector<std::string> path_token;

/* 
 * 目录项操作相关系统调用 
 * By Brayden, Zachary
 * 
 */

inode *_getlinkinode(inode *inode)
{
    // 读取软链接内容
    char _linkedfile[1024];
    fseek(fd, DATASTART + inode->di_addr[0] * BLOCKSIZ, SEEK_SET);
    fread(_linkedfile, inode->di_size, 1, fd);
    _linkedfile[inode->di_size] = '\0';
    // 首先进行跳转
    // 对目录进行路径和文件拆分
    int namestart = -1;
    for (int i = 0; _linkedfile[i]; i++)
    {
        if (_linkedfile[i] == '/')
        {
            namestart = i;
        }
    }
    char *path = ".";
    char *file = _linkedfile;
    if (namestart != -1)
    {
        _linkedfile[namestart] = '\0';
        path = _linkedfile;
        file = _linkedfile + namestart + 1;
    }
    if (namestart == 0)
    {
        path = "/";
    }
    // 首先进入链接目录上级目录
    struct inode *inode2 = detect(path);
    if (inode2 == 0)
    {
        return inode2;
    }
    // 然后拿到该目录的索引节点号
    path_token.push_back(file);
    unsigned int ino = 0; // = namei(file);
    int i;
    for (i = 0; i < temp_dir.size; i++)
    {
        if ((!strcmp(temp_dir.direct[i].d_name, file)) && (temp_dir.direct[i].d_ino != 0))
            ino = temp_dir.direct[i].d_ino; /* find */
    }
    if (ino == 0)
    {
        return 0;
    }
    return iget(ino);
}

// 获取软链接类型索引节点指向的实际索引节点
inode *getlinkinode(inode *inode)
{

    int i, j;
    // 保存当前目录，将目录写入内存
    for (i = getInodeBlockNum(cur_path_inode) - 1; i >= 0; i--)
    {
        bfree(cur_path_inode->di_addr[i]);
    }
    for (i = 0, j = 0; i < dir.size; i += BLOCKSIZ / DIRITEMSIZ, j++)
    {
        int block = balloc();
        cur_path_inode->di_addr[j] = block;
        fseek(fd, DATASTART + block * BLOCKSIZ, SEEK_SET);
        fwrite(&dir.direct[i], 1, BLOCKSIZ, fd);
    }
    cur_path_inode->di_size = dir.size * DIRITEMSIZ;


    path_token.clear();
    temp_dir = dir;
    temp_cur_path_inode = cur_path_inode;
    return _getlinkinode(inode);
}

// 打印当前路径下内容
void _dir()
{
    unsigned int di_mode;
    int i, one;
    struct inode *temp_inode;
    char showc[] = "rwx rwx rwx ";
    printf("%-10s\tino\tmode\t\tsize\tuid\tblocks\tgid\tdinumber\n", "name");

    // 权限检查
    if (!access(user_id, cur_path_inode, READ))
    {
        printf("您没有读取权限\n");
        return;
    }

    for (i = 0; i < dir.size; i++)
    {
        // 不为空
        if (dir.direct[i].d_ino != DIEMPTY)
        {
            // printf("名字：(%s)\t", dir.direct[i].d_name);
            temp_inode = iget(dir.direct[i].d_ino);
            di_mode = temp_inode->di_mode;

            printf("%-10s\t", dir.direct[i].d_name);
            printf("%d\t", temp_inode->i_ino);

            if (di_mode & DIFILE)
                printf("f");
            if (di_mode & DIDIR)
                printf("d");
            if (di_mode & DILINK)
                printf("l");
            for (int j = 0; j < 12; j++)
            {
                one = di_mode % 2;
                di_mode = di_mode / 2;
                if ((j + 1) % 4 == 0)
                    continue;
                if (one)
                    printf("%c", showc[j]);
                else
                    printf("-");
            }
            printf("\t");

            // printf("%d\t",di_mode);
            printf("%d\t", temp_inode->di_size);

            printf("%d\t", temp_inode->di_uid);

            for (int i = 0; i < getInodeBlockNum(temp_inode); i++)
            {
                if (i != 0)
                    printf(",");
                printf("%d", temp_inode->di_addr[i]);
            }
            printf("\t");

            printf("%d\t", temp_inode->di_gid);
            printf("%d\t", temp_inode->di_number);

            di_mode = temp_inode->di_mode;

            if (di_mode & DILINK)
            {
                inode *link = getlinkinode(temp_inode);
                if (link == 0)
                {
                    printf("->none");
                }
                else if (link->di_mode & DIFILE)
                {
                    printf("->file");
                }
                else if (link->di_mode & DIDIR)
                {
                    printf("->dir");
                }
                else
                {
                    printf("->null");
                }
            }
            printf("\t");

            printf("\n");

            iput(temp_inode);
        }
    }
}

// 创建文件夹
int s_mkdir(const char *dirname)
{
    int dirid, dirpos;

    struct inode *inode;

    struct direct buf[DIRNUM];
    unsigned int block;

    // 权限检查
    if (!access(user_id, cur_path_inode, WRITE))
    {
        seterror("您没有写入权限");
        return SYSCALL_FAIL;
    }

    // 在当前目录下查找
    dirid = namei(dirname);
    // 已存在
    if (dirid != 0)
    {
        inode = iget(dirid);
        if (inode->di_mode & DIDIR)
        {
            seterror("目录已存在");
        }
        else
        {
            seterror("存在同名文件");
        }
        iput(inode);
        return SYSCALL_FAIL;
    }

    // 不存在则新建
    dirpos = iname(dirname);

    if (dirpos == 0)
    {
        return SYSCALL_FAIL;
    }

    inode = ialloc();
    if (inode == 0)
    {
        return SYSCALL_FAIL;
    }
    // inode->i_ino = dirid;
    dir.direct[dirpos].d_ino = inode->i_ino;
    dir.size++;
    /*    fill the new dir buf */
    debug(1);
    strcpy(buf[1].d_name, ".");
    buf[1].d_ino = inode->i_ino;
    strcpy(buf[0].d_name, "..");
    buf[0].d_ino = cur_path_inode->i_ino;
    for (int j = 2; j < DIRNUM; j++)
        buf[j].d_ino = 0;
    block = balloc();
    if (block == DISKFULL)
    {
        dir.direct[dirpos].d_ino = 0;
        dir.size--;
        inode->di_number = 0;
        ifree(inode);
        return SYSCALL_FAIL;
    }

    fseek(fd, DATASTART + block * BLOCKSIZ, SEEK_SET);
    fwrite(buf, 1, BLOCKSIZ, fd);

    debug(2);
    debug(user_id);
    inode->di_size = 2 * (DIRSIZ + 4);
    inode->di_number = 1;
    inode->di_mode = user[user_id].u_default_mode | DIDIR;
    inode->di_uid = user[user_id].u_uid;
    inode->di_gid = user[user_id].u_gid;
    inode->di_addr[0] = block;
    debug(2.5);
    iput(inode);
    debug(3);


    return SYSCALL_SUCC;
}

// 在临时工作目录进行单级跳转
inode *step(const char *dirname)
{
    path_token.push_back(dirname);
    unsigned int dirid = 0;
    struct inode *inode;
    unsigned short block;
    int i, j, low = 0, high = 0;
    if (strcmp(dirname, "/") == 0)
    {
        inode = iget(ROOTINO);
    }
    else
    {
        // 查找目录
        int i;
        for (i = 0; ((i < temp_dir.size)); i++)
        {
            if ((!strcmp(temp_dir.direct[i].d_name, dirname)) && (temp_dir.direct[i].d_ino != 0))
                dirid = temp_dir.direct[i].d_ino; /* find */
        }

        if (dirid == 0)
        {
            char temp[1024];
            sprintf(temp, "目录不存在：%s", dirname);
            seterror(temp);
            return 0;
        }
        inode = iget(dirid);
    }


    if (inode->di_mode & DIFILE)
    {
        seterror("不是目录");
        iput(inode);
        return 0;
    }

    if (inode->di_mode & DILINK)
    {
        path_token.pop_back();
        struct inode *linkinode = _getlinkinode(inode);
        iput(inode);
        inode = linkinode;
        if (inode == 0)
        {
            seterror("软链接指向路径已失效");
            return 0;
        }
        // 如果不是目录
        if (inode->di_mode & DIFILE)
        {
            printf("软链接指向的不是目录\n");
            iput(inode);
            return 0;
        }
    }

    for (j = 0; j < DIRNUM; j++)
        temp_dir.direct[j].d_ino = 0;

    temp_cur_path_inode = inode;

    // 读取跳转到的目录内容
    j = 0;
    int idx = 0;
    temp_dir.size = inode->di_size / DIRSIZ;

    for (i = 0, j = 0; i < temp_dir.size; i += BLOCKSIZ / DIRITEMSIZ, j++)
    {
        fseek(fd, DATASTART + inode->di_addr[j] * BLOCKSIZ, SEEK_SET);
        fread(&temp_dir.direct[i], 1, BLOCKSIZ, fd);
    };

    return inode;
}

// 判断路径是否存在，若存在则返回索引节点
inode *detect(const char *path)
{
    const char *pathback = path;
    char temp[DIRSIZ];
    char len = 0;
    inode *target;
    char last = '\0';
    inode *result = 0;

    // 如果path以‘/’开头
    if (*path == '/')
    {
        iput(result);
        result = step("/");
        last = '/';
        path++;
    }

    while (*path != '\0')
    {
        if (*path == '/')
        {
            temp[len] = '\0';
            iput(result);
            if ((result = step(temp)) == 0)
            {
                return result;
            }
            len = 0;
        }
        else
        {
            temp[len++] = *path;
        }
        last = *path;
        path++;
    }
    if (last != '/')
    {
        temp[len] = '\0';
        iput(result);
        result = step(temp);
    }

    if (result == 0)
    {
        return 0;
    }

    if (!access(user_id, result, READ))
    {
        char temp[1024];
        sprintf(temp, "您没有读取权限：%s", pathback);
        seterror(temp);
        iput(result);
        return 0;
    }

    return result;
}

#define MAX_PATH_LEN 1024
#define MAX_TOKENS 256

// 切换当前工作目录（支持多级跳转）
int s_chdir(const char *dirname)
{
    path_token.clear();
    unsigned int dirid;
    struct inode *inode;
    unsigned short block;
    int i, j, low = 0, high = 0;

    // 保存当前目录，将目录写入内存
    for (i = getInodeBlockNum(cur_path_inode) - 1; i >= 0; i--)
    {
        bfree(cur_path_inode->di_addr[i]);
    }
    for (i = 0, j = 0; i < dir.size; i += BLOCKSIZ / DIRITEMSIZ, j++)
    {
        block = balloc();
        cur_path_inode->di_addr[j] = block;
        fseek(fd, DATASTART + block * BLOCKSIZ, SEEK_SET);
        fwrite(&dir.direct[i], 1, BLOCKSIZ, fd);
    }
    cur_path_inode->di_size = dir.size * DIRITEMSIZ;
    // iput(cur_path_inode);



    temp_dir = dir;
    temp_cur_path_inode = cur_path_inode;
    inode = detect(dirname);


    if (inode == 0)
    {
        return SYSCALL_FAIL;
    }

    iput(cur_path_inode);
    cur_path_inode = inode;

    for (j = 0; j < DIRNUM; j++)
        dir.direct[j].d_ino = 0;


    // 读取跳转到的目录内容
    j = 0;
    dir.size = inode->di_size / DIRITEMSIZ;
    for (i = 0, j = 0; i < dir.size; i += BLOCKSIZ / DIRITEMSIZ, j++)
    {
        fseek(fd, DATASTART + inode->di_addr[j] * BLOCKSIZ, SEEK_SET);
        fread(&dir.direct[i], 1, BLOCKSIZ, fd);
        // j += BLOCKSIZ / (DIRSIZ + 4);
    };

    for (i = dir.size; i < DIRNUM; i++)
        dir.direct[i].d_ino = 0;

    char *path = (char *)dirname;

    std::string temp_PWD = PWD;
    // 处理绝对路径


    for (auto it = path_token.begin(); it != path_token.end(); it++)
    {
        // char* cdstr=const_cast<char*>((*it).c_str());
        // cout<<"现在进入："<<cdstr;
        // chdir(cdstr);
        if (*it == ".")
        {
            continue;
        }
        else if (*it == "/")
        {
            temp_PWD = "/";
            continue;
        }
        else if (*it == "..")
        {
            if (temp_PWD == "/")
            {
                continue;
            }
            for (int j = temp_PWD.size() - 1; j >= 0; j--)
            {
                // cout << "j:" << j << endl;
                // cout << "PWD:" << PWD << endl;
                if (temp_PWD[j] == '/')
                {
                    temp_PWD = temp_PWD.substr(0, j);
                    break;
                }
            }
            temp_PWD = temp_PWD == "" ? "/" : temp_PWD;
        }
        else
        {
            if (temp_PWD != "/")
            {
                temp_PWD += "/";
            }
            temp_PWD += *it;
        }
    }

    strcpy(OLDPWD, PWD);
    strcpy(PWD, temp_PWD.c_str());

    return SYSCALL_SUCC;
}

// 重命名
int rename(char *oldname, char *newname)
{
    unsigned int ino1 = namei(oldname);
    if (ino1 == 0)
    {
        char temp[100];
        sprintf(temp, "不存在的文件或目录：%s", oldname);
        seterror(temp);
        return SYSCALL_FAIL;
    }
    unsigned int ino2 = namei(newname);
    if (ino2 != 0)
    {
        char temp[100];
        sprintf(temp, "文件或目录名已存在：%s", newname);
        seterror(temp);
        return SYSCALL_FAIL;
    }
    // 权限检查
    if (!access(user_id, cur_path_inode, WRITE))
    {
        seterror("您没有写入权限");
        return SYSCALL_FAIL;
    }

    int i;
    for (i = 0; i < dir.size; i++)
    {
        if ((!strcmp(dir.direct[i].d_name, oldname)) && (dir.direct[i].d_ino != 0))
        {
            strcpy(dir.direct[i].d_name, newname);
        }
    }
    return SYSCALL_SUCC;
}

// 创建软链接（符号链接）
int softlink(const char *linkedpath, const char *newpath)
{
    char _linkedfile[1024];
    char _newfile[1024];
    strcpy(_linkedfile, linkedpath);
    strcpy(_newfile, newpath);
    // 对第一个目录进行路径和文件拆分
    int namestart = -1;
    for (int i = 0; _linkedfile[i]; i++)
    {
        if (_linkedfile[i] == '/')
        {
            namestart = i;
        }
    }
    char *path = ".";
    char *file = _linkedfile;
    if (namestart != -1)
    {
        _linkedfile[namestart] = '\0';
        path = _linkedfile;
        file = _linkedfile + namestart + 1;
    }
    if (namestart == 0)
    {
        path = "/";
    }
    // 首先进入被链接文件所在目录
    if(s_chdir(path) == SYSCALL_FAIL) {
        return SYSCALL_FAIL;
    }
    // 然后拿到该文件的索引节点号
    unsigned int ino = namei(file);
    // 然后返回原目录
    s_chdir(OLDPWD);
    // 对第二个目录进行路径和文件拆分
    int namestart2 = -1;
    for (int i = 0; _newfile[i]; i++)
    {
        if (_newfile[i] == '/')
        {
            namestart2 = i;
        }
    }
    char *path2 = ".";
    char *file2 = _newfile;
    if (namestart2 != -1)
    {
        _newfile[namestart2] = '\0';
        path2 = _newfile;
        file2 = _newfile + namestart2 + 1;
    }
    if (namestart2 == 0)
    {
        path2 = "/";
    }
    // 进入目标文件目录
    if(s_chdir(path2) == SYSCALL_FAIL) {
        return SYSCALL_FAIL;
    }
    // 创建软链接文件
    int lfd = aopen(user_id, file2, "w");
    if(lfd == INVALIDFD) {
        s_chdir(OLDPWD);
        return SYSCALL_FAIL;
    }
    // 写入链接地址
    s_write(lfd, linkedpath, strlen(linkedpath));
    close(user_id, lfd);
    // mode设为链接
    ino = namei(file2);
    if(ino == 0) {
        return SYSCALL_FAIL;
    }
    inode *inode = iget(ino);
    inode->di_mode &= 0x0fff;
    inode->di_mode |= DILINK;
    iput(inode);
    // 然后返回原目录
    s_chdir(OLDPWD);

    return SYSCALL_SUCC;
}

// 创建硬链接
int hardlink(const char *linkedfile, const char *newfile)
{
    char _linkedfile[1024];
    char _newfile[1024];
    strcpy(_linkedfile, linkedfile);
    strcpy(_newfile, newfile);
    // 对第一个目录进行路径和文件拆分
    int namestart = -1;
    for (int i = 0; _linkedfile[i]; i++)
    {
        if (_linkedfile[i] == '/')
        {
            namestart = i;
        }
    }
    char *path = ".";
    char *file = _linkedfile;
    if (namestart != -1)
    {
        _linkedfile[namestart] = '\0';
        path = _linkedfile;
        file = _linkedfile + namestart + 1;
    }
    if (namestart == 0)
    {
        path = "/";
    }
    // 首先进入被链接文件所在目录
    if(s_chdir(path) == SYSCALL_FAIL) {
        return SYSCALL_FAIL;
    }
    // 然后拿到该文件的索引节点号
    unsigned int ino = namei(file);
    // 然后返回原目录
    s_chdir(OLDPWD);
    if(ino == 0) {
        return SYSCALL_FAIL;
    }
    // 对第二个目录进行路径和文件拆分
    int namestart2 = -1;
    for (int i = 0; _newfile[i]; i++)
    {
        if (_newfile[i] == '/')
        {
            namestart2 = i;
        }
    }
    char *path2 = ".";
    char *file2 = _newfile;
    if (namestart2 != -1)
    {
        _newfile[namestart2] = '\0';
        path2 = _newfile;
        file2 = _newfile + namestart2 + 1;
    }
    if (namestart2 == 0)
    {
        path2 = "/";
    }
    // 进入目标文件目录
    if(s_chdir(path2) == SYSCALL_FAIL) {
        return SYSCALL_FAIL;
    }
    // 创建目录项
    unsigned short dirpos = iname(file2);
    dir.direct[dirpos].d_ino = ino;
    dir.size++;
    inode *inode = iget(ino);
    inode->di_number++;
    iput(inode);
    // 然后返回原目录
    s_chdir(OLDPWD);

    return SYSCALL_SUCC;
}

// 读取软链接内容
int readlink(struct inode *inode, char *linkpath)
{
    if (!(inode->di_mode & DILINK))
    {
        seterror("不是软链接");
        return SYSCALL_FAIL;
    }
    // 读取软链接内容
    fseek(fd, DATASTART + inode->di_addr[0] * BLOCKSIZ, SEEK_SET);
    fread(linkpath, inode->di_size, 1, fd);
    linkpath[inode->di_size] = '\0';
    return SYSCALL_SUCC;
}