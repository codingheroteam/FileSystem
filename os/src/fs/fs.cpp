#include "fs.h"


/* 
 * 文件系统的全局变量 
 * By Brayden, Zachary, Peter
 * 
 */

struct hinode hinode[NHINO];
struct dir dir;
struct file sys_ofile[SYSOPENFILE];
struct filsys filsys;
struct pwd pwd[PWDNUM];
struct pwd passwds[PWDNUM];
struct user user[USERNUM];
FILE * fd;
struct inode * cur_path_inode;
int user_id = USERNUM;
char errmsg[100];
char PWD[100] = "/";
char OLDPWD[100] = "/";

// 设置错误
void seterror(const char* msg) {
    debug("ERROR");
    debug(msg);
    strcpy(errmsg, msg);
}

// 打印错误
void printerror(const char* title) {
    printf("ERROR: %s: %s\n", title, errmsg);
}