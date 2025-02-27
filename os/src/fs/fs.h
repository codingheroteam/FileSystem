#ifndef OS_H_INCLUDED
#define OS_H_INCLUDED
#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <cassert>

/* 
 * 文件系统头文件 
 * By Brayden, Zachary, Peter
 * 
 */


/*---------------------若要详细输出，可使用下面宏--------------------*/
// #define DEBUG
#ifdef DEBUG
#define debug(content) std::cout << "debug: " << content << std::endl;
#else
#define debug(content)
#endif
/*---------------------------------------------------------------*/



/*-------------------------登录相关参数----------------------------*/
#define SYSOPENFILE 40      // 系统打开文件表最大项数
#define PWDSIZ      12      // 用户密码长度
#define USERNAMESIZ 10      // 用户名长度
#define PWDNUM      16      // user文件中能存储的最多用户数
#define NOFILE      20      // 每个用户最多打开文件数
#define NHINO       128     // 索引节点哈希链表，哈希值数目
#define USERNUM     10      // 系统最多允许登录用户数
/*---------------------------------------------------------------*/


/*------------------------存储相关参数-----------------------------*/
#define BLOCKSIZ    1024        // 每块大小
#define DIRSIZ      124         // 每个目录项名字部分所占字节数
#define NADDR       16          // 每个i节点最多指向数据块数
#define DINODESIZ   128         // 每个索引节点大小
#define DINODEBLK   32          // 索引区块数
#define FILEBLK     256         // 数据区块数
#define NICFREE     50          // 超级块中空闲块数组的最大块数
#define NICINOD     50          // 超级块中空闲索引节点的数目
#define DIRITEMSIZ (DIRSIZ+4)   // 每个目录项大小
// // 下面参数为边界测试时所用参数
// #define BLOCKSIZ    128     // 每块大小
// #define DIRSIZ      28      // 每个目录项名字部分所占字节数
// #define NADDR       10      // 每个i节点最多指向数据块数
// #define DINODESIZ   128     // 每个索引节点大小
// #define DINODEBLK   16      // 索引区块数
// #define FILEBLK     32      // 数据区块数
// #define NICFREE     10      // 超级块中空闲块数组的最大块数
// #define NICINOD     5       // 超级块中空闲索引节点的数目
// #define DIRITEMSIZ (DIRSIZ+4)   // 每个目录项大小
/*---------------------------------------------------------------*/


#define DIRNUM  (BLOCKSIZ*NADDR/DIRITEMSIZ)   // 每个索引节点的最多目录项数



#define FILEBLKNPTR FILEBLK    //// 数据区块号空指针


#define DINODESTART     2*BLOCKSIZ  // 索引节点区起始地址
#define DATASTART       (2+DINODEBLK)*BLOCKSIZ // 数据区起始地址
#define INVALIDFD       SYSOPENFILE

#define ROOTINO        1           // 根目录节点号
#define ROOTUID        1           // root用户ID 


#define SYSCALL_FAIL  -1        // 函数执行失败
#define SYSCALL_SUCC  0         // 函数执行成功


#define DIEMPTY     0x0000      // 空项
#define DIFILE      0x1000      // 文件项
#define DIDIR       0x2000      // 目录项
#define DILINK      0x4000      // 软链接项

#define DISKFULL    65535       // 磁盘已满
#define SEEK_SET    0           // 磁盘起始位置


// 更新标记
#define IUPDATE     0x0002
#define SUPDATE     0x0001




/*--------------------------------用户权限相关-------------------------------*/
#define UDIREAD     0x0001  // 所有者读取权限
#define UDIWRITE    0x0002  // 所有者写入权限
#define UDIEXICUTE  0x0004  // 所有者执行权限

#define GDIREAD     0x0010  // 组读取权限
#define GDIWRITE    0x0020  // 组写入权限
#define GDIEXICUTE  0x0040  // 组执行权限

#define ODIREAD     0x0100  // 其他用户读取权限
#define ODIWRITE    0x0200  // 其他用户写入权限
#define ODIEXICUTE  0x0400  // 其他用户执行权限

#define READ        1        // 读操作
#define WRITE       2        // 写操作
#define EXICUTE     4        // 执行操作

#define DEFAULTMODE 0x0117  // 创建新文件或目录时的默认权限
#define ALLPERMIT   0x0777  // 全部允许权限

// 文件打开模式
#define FREAD       0x0001
#define FWRITE      0x0002
#define FAPPEND     0x0004
#define FPLUS       0x0008
/*------------------------------------------------------------------------*/




/*---------------------------------------------------结构体-----------------------------------------------------*/

// 索引节点，注意前面的定义需要与dinode一致，di_number作为第一个不能改变，定位用
struct inode {
    struct inode* i_forw;    // 前向指针
    struct inode* i_back;    // 后向指针

    unsigned short di_number;// 硬链接数
    unsigned short di_mode;  // 权限模式
    unsigned short di_uid;   // 所有者的用户ID
    unsigned short di_gid;   // 所有者的组ID
    unsigned short di_size;  // 文件大小
    unsigned int di_addr[NADDR]; // 块地址表
    
    unsigned int i_count;    // 当前程序对此节点iget计数
    char i_flag;             // 标志位
    unsigned int i_ino;      // 索引节点号
};

// 磁盘上的索引节点
struct dinode {
    unsigned short di_number; // 硬链接数(----2)
    unsigned short di_mode;   // 权限模式(----4)
    unsigned short di_uid;    // 所有者的用户ID(----6)
    unsigned short di_gid;    // 所有者的组ID(----8)
    unsigned long di_size;    // 文件大小(----16)
    unsigned int di_addr[NADDR]; // 块地址表(----16+4*NADDR)
};

// 磁盘上的目录项
struct direct {
    char d_name[DIRSIZ];     // 目录项名
    unsigned int d_ino;      // 目录项对应的索引节点号
};

// 文件系统信息（超级块中存储此结构）
struct filsys {
    unsigned short s_isize;       // 索引区块数(----2)
    unsigned long s_fsize;        // 数据区块数(----10)
    unsigned int s_nfree;         // 剩余空闲块数量(----14)
    unsigned short s_pfree;       // 是下面s_free表的起始项索引(----16)
    unsigned int s_free[NICFREE]; // 第一组空闲块表(----16+4*NICFREE)
    unsigned int s_ninode;        // 空闲索引节点数量(----20+4*NICFREE)
    unsigned short s_pinode;      // 是下面s_inode表的起始项索引(----22+4*NICFREE)
    unsigned int s_inode[NICINOD];// 空闲索引节点表(----22+4*NICFREE+4*NICINOD)
    unsigned int s_rinode;        // 第一个未被记录的空闲索引节点号(----26+4*NICFREE+4*NICINOD)
    char s_fmod;                  // 超级块修改标识(----28+4*NICFREE+4*NICINOD)
};

// 用户密码信息
struct pwd {
    unsigned short p_uid;        // 用户ID
    unsigned short p_gid;        // 组ID
    char u_username[USERNAMESIZ];// 用户名
    char password[PWDSIZ];       // 密码
};

// 目录信息
struct dir {
    struct direct direct[DIRNUM]; // 目录项数组
    int size;                     // 目录大小
};

// 索引节点的哈希表项
struct hinode {
    struct inode* i_forw;         // 前向指针
};

// 打开文件信息
struct file {
    char f_flag;               // 文件标志
    unsigned int f_count;      // 引用计数
    struct inode* f_inode;     // 关联的索引节点
    unsigned long f_off;       // 文件偏移量
};

// 用户文件信息
struct user {
    unsigned short u_uid;          // 用户ID
    unsigned short u_gid;          // 组ID
    char u_username[10];           // 用户名
    unsigned short u_ofile[NOFILE];// 打开文件表
    unsigned short u_default_mode; // 默认权限模式
    unsigned short super;          // 是否已提权
};

/*------------------------------------------------------------------------------------------------------*/


/*----------------------------------------------全局变量--------------------------------------------------*/
extern struct hinode hinode[NHINO];           // 已get索引节点哈希表
extern struct dir dir;                        // 当前目录
extern struct file sys_ofile[SYSOPENFILE];    // 系统已打开文件表
extern struct filsys filsys;                  // 文件系统（超级块信息）
extern struct pwd passwds[PWDNUM];            // 所有用户信息（从/etc/user文件中读取）
extern struct user user[USERNUM];             // 已登录用户
extern        FILE* fd;                       // 虚拟磁盘文件描述符
extern struct inode* cur_path_inode;          // 当前目录索引节点
extern int    user_id;                        // 当前登录用户序号
extern char   PWD[100];                       // 当前工作目录
extern char   OLDPWD[100];                    // 上一工作目录
/*--------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------系统调用--------------------------------------------------*/

// 通用函数
void            seterror(const char* msg);      // 设置错误
void            printerror(const char* title);  // 打印错误
inline int getInodeBlockNum(inode* pinode) { return (pinode->di_size +  BLOCKSIZ - 1) / BLOCKSIZ; } // 计算索引的数据区使用块数

// 索引与块相关函数
struct inode*   iget(unsigned int ino);         // 根据索引节点号获取索引节点指针
void            iput(struct inode* inode);      // 写回索引节点
unsigned int    balloc();                       // 分配块
void            bfree(unsigned int block);      // 释放块
struct inode*   ialloc();                       // 分配索引节点
void            ifree(struct inode* inode);     // 释放索引节点

// 目录项相关函数
unsigned int    namei(const char* name);        // 根据目录项名获取索引节点号
unsigned short  iname(const char* name);        // 创建一个新的目录项，返回在dir中的序号
unsigned int    access(unsigned int user_id,struct inode * inode,unsigned short mode); // 访问权限检查
void            _dir();                         // 显示当前目录内容
int             s_chdir(const char* dirname);   // 改变当前目录
int             s_mkdir(const char* dirname);   // 创建目录
int             softlink(const char* linkedpath, const char* newpath); // 创建软链接
int             hardlink(const char* linkedfile, const char* newfile); // 创建硬链接
int             rename(char *oldname, char* newname);                  // 目录项重命名
int             rmfile(char * name);        // 删除文件
int             rmdir(char * name);         // 删除目录
int             rmlink(const char * name);  // 删除软链接
int             chmod(char *dirname, unsigned int mode);        // 更改权限模式
struct inode*   detect(const char *path);       // 给定复杂路径，判断是否存在，返回索引节点


// 文件读写相关函数
unsigned short  aopen(int user_id, const char* filename,const char* openmodestr);   // 打开文件
void            close(unsigned int user_id, unsigned short cfd);                    // 关闭文件
unsigned int    s_read(int ifd, void * buf, unsigned int size);                     // 读取文件内容
unsigned int    s_write(int ifd, const void * buf, unsigned int size);              // 写入文件内容
int             creat(unsigned int user_id, const char * filename, unsigned short mode);   // 创建文件
int             readlink(struct inode* inode, char* linkpath);                      // 读取软链接内容

// 用户相关函数
int             s_login(const char* username, const char* passwd);            // 用户登录
int             s_logout(const char* username);                               // 用户登出

// 文件系统相关函数
int             install();                  // 安装文件系统
void            format();                   // 格式化文件系统
void            halt();                     // 关闭系统

/*---------------------------------------------------------------------------------------------------*/



#endif