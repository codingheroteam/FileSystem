#include <stdio.h>
#include "fs.h"
#include <string.h>

/* 
 * 登录登出相关函数 
 * By Brayden, Peter
 * 
 */



// 用户登录
int s_login(const char* username, const char* passwd)
{

    inode* inode = iget(3);
    fseek(fd, DATASTART+BLOCKSIZ*inode->di_addr[0], SEEK_SET);
    fread(passwds, 1, sizeof(passwds), fd);
    iput(inode);
   
    int i, j;

    for (i = 0; i < USERNUM; i++) {
        if (strcmp(username, user[i].u_username) == 0 && user[i].u_uid != 0) {
            user_id = i;
            printf("该用户已登录\n");
            return i;
        }
    }



    for (i = 0;i < PWDNUM;i++)
    {
        if ((strcmp(username, passwds[i].u_username)==0) && (strcmp(passwd, passwds[i].password)==0))
        {
            for (j = 0;j < USERNUM; j++)
                if (user[j].u_uid == 0) break;
            if (j == USERNUM)
            {
                printf("达到登录用户上限\n");
                return j;
            }
            else
            {
                user[j].u_uid = passwds[i].p_uid;
                user[j].u_gid = passwds[i].p_gid;
                strcpy(user[j].u_username, passwds[i].u_username);
                user[j].u_default_mode = DEFAULTMODE;
            }
            break;
        }
    }

    if (i == PWDNUM)
    {
        printf("用户名或密码错误\n");
        return USERNUM;
    }
    else {
        printf("登录成功\n");
        user_id = j;
        return j;
    }
}


// 用户登出
int s_logout(const char* username)
{
    int i, j, sys_no;
    struct inode* inode;
    for (i = 0; i < USERNUM; i++)
        if (strcmp(username, user[i].u_username) == 0) break;
    if (i == USERNUM)
    {
        printf("该用户未登录：%s\n", username);
        return 0;
    }
    for (j = 0;j < NOFILE; j++)
    {
        if (user[i].u_ofile[j] != SYSOPENFILE)
        {
            sys_no = user[i].u_ofile[j];
            inode = sys_ofile[sys_no].f_inode;
            iput(inode);
            sys_ofile[sys_no].f_count--;
            user[i].u_ofile[j] = SYSOPENFILE;
        }
    }
    user[i].u_uid = 0;

    if(strcmp(user[user_id].u_username, username) == 0) {
        user_id = USERNUM;
    }
    
    return 1;
}