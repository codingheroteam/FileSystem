

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fs.h"
using namespace std;


/* 
 * 格式化函数 
 * By Brayden
 * 
 */


void format()
{
    debug("格式化");
    

    struct inode *inode;
    struct direct dir_buf[BLOCKSIZ / (DIRSIZ + 4)];
    struct pwd passwd[PWDNUM];

    struct filsys filsys;
    unsigned int block_buf[BLOCKSIZ / sizeof(int)];
    char *buf;
    int i, j;

    // 清空已iget表
    for (i = 0; i < NHINO; i++)
    {
        hinode[i].i_forw = NULL;
    }

    /*    creat the file system file */
    fd = fopen("filesystem", "w+b");
    buf = (char *)malloc((DINODEBLK + FILEBLK + 2) * BLOCKSIZ * sizeof(char));
    memset(buf, 0, (DINODEBLK + FILEBLK + 2) * BLOCKSIZ * sizeof(char));
    if (buf == NULL)
    {
        printf("\nfile system file creat failed! \n");
        exit(0);
    }
    fseek(fd, 0, SEEK_SET);
    fwrite(buf, 1, (DINODEBLK + FILEBLK + 2) * BLOCKSIZ * sizeof(char), fd);
    /*0.initialize the passwd */
    passwd[0].p_uid = 1;
    passwd[0].p_gid = 1;
    strcpy(passwd[0].u_username, "root");
    strcpy(passwd[0].password, "admin");
    passwd[1].p_uid = 2;
    passwd[1].p_gid = 2;
    strcpy(passwd[1].u_username, "Brayden");
    strcpy(passwd[1].password, "123456");
    passwd[2].p_uid = 3;
    passwd[2].p_gid = 2;
    strcpy(passwd[2].u_username, "Zachary");
    strcpy(passwd[2].password, "123456");
    passwd[3].p_uid = 4;
    passwd[3].p_gid = 3;
    strcpy(passwd[3].u_username, "Peter");
    strcpy(passwd[3].password, "123456");
    passwd[4].p_uid = 5;
    passwd[4].p_gid = 3;
    strcpy(passwd[4].u_username, "user5");
    strcpy(passwd[4].password, "123456");
    /*    1.creat the main directory and its sub dir etc and the file password */
    debug("creat the main directory and its sub dir etc and the file password");
    inode = iget(0); /* 0 empty dinode id */
    inode->di_mode = DIEMPTY;
    iput(inode);
    debug("123");
    inode = iget(1); /* 1 main dir id */
    debug("123");
    inode->di_number = 1;
    inode->di_mode = ALLPERMIT | DIDIR;
    inode->di_size = 3 * (DIRSIZ + 4);
    inode->di_uid = 0;
    inode->di_gid = 0;
    inode->di_addr[0] = 0; /* block 0tfl is used by the main directory */
    strcpy(dir_buf[0].d_name, "..");
    dir_buf[0].d_ino = 1;
    strcpy(dir_buf[1].d_name, ".");
    dir_buf[1].d_ino = 1;
    strcpy(dir_buf[2].d_name, "etc");
    dir_buf[2].d_ino = 2;
    // 数据区第一个块，存储根目录项
    fseek(fd, DATASTART, SEEK_SET);
    for (int i = 0; i < sizeof(dir_buf) / sizeof(dir_buf[0]); i++)
    {
        fwrite(&dir_buf[i], DIRSIZ + 4, 1, fd);
    }
    // 写入根目录索引节点
    iput(inode);


    inode = iget(2); /* 2 etc dir id */
    inode->di_number = 1;
    inode->di_mode = ALLPERMIT | DIDIR;
    inode->di_size = 3 * (DIRSIZ + 4);
    inode->di_uid = 0;
    inode->di_gid = 0;
    inode->di_addr[0] = 1; /* block 0# is used by the etc */
    strcpy(dir_buf[0].d_name, "..");
    dir_buf[0].d_ino = 1;
    strcpy(dir_buf[1].d_name, ".");
    dir_buf[1].d_ino = 2;
    strcpy(dir_buf[2].d_name, "user");
    dir_buf[2].d_ino = 3;
    fseek(fd, DATASTART + BLOCKSIZ * 1, SEEK_SET);
    for (int i = 0; i < sizeof(dir_buf) / sizeof(dir_buf[0]); i++)
    {
        fwrite(&dir_buf[i], DIRSIZ + 4, 1, fd);
    }
    // fwrite(dir_buf, (DIRSIZ + 4), 3, fd);
    iput(inode);

    inode = iget(3); /* 3 password id */
    inode->di_number = 1;
    inode->di_mode = ALLPERMIT | DIFILE;
    inode->di_size = BLOCKSIZ;
    inode->di_uid = 0;
    inode->di_gid = 0;
    inode->di_addr[0] = 2;
    for (i = 5; i < PWDNUM; i++)
    {
        passwd[i].p_uid = 0;
        passwd[i].p_gid = 0;
        strcpy(passwd[i].password, "123456");
    }
    fseek(fd, DATASTART + 2 * BLOCKSIZ, SEEK_SET);
    fwrite(passwd, 1, BLOCKSIZ, fd);
    iput(inode);

    /*    2. initialize the superblock */
    cout << "初始化空闲块表" << endl;
    // 初始化空闲块表
    // 从最后一个块开始
    // 每50个为一组，每一组的第一个块存储本组信息，第一个块的第一项存储下一组信息块号
    // 倒数第50个块的块号
    int start = FILEBLK - NICFREE;
    // 这个块存储内容为：
    // 第1项:        FILEBLKNPTR 表示之后没有新的组了
    // 第2～50项:    start+1 start+2 ... start+49
    block_buf[0] = FILEBLKNPTR;
    for (int j = 1; j < NICFREE; j++)
    {
        block_buf[j] = start+NICFREE-j;
    }
    // 将这个块写入磁盘
    fseek(fd, DATASTART + BLOCKSIZ * start, SEEK_SET);
    fwrite(block_buf, 1, BLOCKSIZ, fd);

    while (1)
    {
        // 继续下一组
        start -= NICFREE;
        // 若下一组数量不足50了，则将这一组写入超级块区
        // 注意0号块保留为root目录，1号块保留为etc目录，2号块保留为用户密码文件
        if (start < 3)
        {
            start += NICFREE;
            break;
        }
        // 这个块存储内容为：
        // 第1项:        start+NICFREE  是下一个组的信息块的块号
        // 第2～50项:    start+1 start+2 ... start+49
        block_buf[0] = start+NICFREE;
        for (int j = 1; j < NICFREE; j++)
        {
            block_buf[j] = start+NICFREE-j;
        }
        // 将这个块写入磁盘
        fseek(fd, DATASTART + BLOCKSIZ * start, SEEK_SET);
        fwrite(block_buf, 1, BLOCKSIZ, fd);
    }

    cout << "初始化超级块" << endl;
    // 相关配置信息
    filsys.s_isize = DINODEBLK;
    filsys.s_fsize = FILEBLK;
    // 空闲索引节点数量
    filsys.s_ninode = DINODEBLK * BLOCKSIZ / DINODESIZ - 4;
    // 空闲块数量
    filsys.s_nfree = FILEBLK - 3;

    cout << "初始化空闲索引节点表" << endl;
    // 初始化空闲索引节点表
    for (i = 0; i < NICINOD && i < filsys.s_ninode; i++)
    {
        /*    begin with 4, 0,1.2,3, is used by main, etc, password */
        filsys.s_inode[i] = 4 + i;
    }
    
    filsys.s_pinode = NICINOD - i;
    filsys.s_rinode = i + 4;

    cout << "初始化超级块中的空闲块数组" << endl;
    // 之前对于不足50个的空闲块，未进行处理，实际是在这里进行处理，将其加到s_free表中
    // s_free表最后一项为下一组信息块的块号，即为start
    // s_free表除了最后一项的倒数n项为剩下的空闲块号（最多49个，最少0个）
    filsys.s_free[0] = start;
    for(j = 1; j < NICFREE; j++) {
        start--;
        // 0,1,2块已经使用了，从3开始
        if(start < 3) {
            break;
        }
        filsys.s_free[j] = start;
    }
    filsys.s_pfree = j - 1;

    // 将超级块写入磁盘
    fseek(fd, BLOCKSIZ, SEEK_SET);
    fwrite(&filsys, 1, sizeof(struct filsys), fd);

    fclose(fd);
    cout << "格式化完成" << endl;
}