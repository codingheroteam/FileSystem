#include <stdio.h>
#include "fs.h"
static struct dinode block_buf[NICINOD];


/* 
 * 索引节点分配与释放函数 
 * By Brayden
 * 
 */


// 申请新的索引节点
struct inode *ialloc()
{
    debug("申请新的索引节点");
    struct inode *temp_inode;
    unsigned int cur_di;
    int i, count, block_end_flag;
    // 如果没有空闲索引节点了，看来得去索引节点区进货了
    if (filsys.s_pinode == NICINOD)
    {
        debug("没有了，要去进货了");
        i = 0;
        count = 0;
        block_end_flag = 1;
        cur_di = filsys.s_rinode;
        // 不断进货，直到进货达到需求量或商家剩余量不足了
        while ((count < NICINOD) && (count < filsys.s_ninode))
        {
            // 不断读取新的索引节点用于判断是否可用
            fseek(fd, DINODESTART + cur_di * DINODESIZ, SEEK_SET);
            fread(block_buf, 1, DINODESIZ, fd);
            // 若这是一个可用节点，则加入空闲表
            if(block_buf[0].di_number == 0) {
                filsys.s_inode[--filsys.s_pinode] = cur_di;
                count++;
            }
            cur_di++;
        }
        filsys.s_rinode = cur_di;
    }
    if(filsys.s_pinode > NICINOD) {
        printf("FATAL ERROR: 错误的s_pinode\n");
        exit(0);
    }
    if(filsys.s_pinode == NICINOD) {
        assert(filsys.s_ninode == 0);
		seterror("索引节点已用完");
        return 0;
    }
    
    temp_inode = iget(filsys.s_inode[filsys.s_pinode++]);
    // 将这个节点设为占用，避免重复申请
    fseek(fd, DINODESTART + temp_inode->i_ino * DINODESIZ, SEEK_SET);
    temp_inode->di_number = 1;
    fwrite(&temp_inode->di_number, 1, sizeof(struct dinode), fd);
    // 剩余节点数减一
    filsys.s_ninode--;
    filsys.s_fmod = SUPDATE;
    debug("申请节点号");
    debug(temp_inode->i_ino);
    debug("剩余空闲节点数");
    debug(filsys.s_ninode);
    return temp_inode;
}

// 将一个索引节点设为空闲状态
void ifree(struct inode* inode)
{
    unsigned int dinodeid = inode->i_ino;
    debug("释放");
    debug(dinodeid);
    // iput(inode);
    
    if(inode->di_number != 0) {
        printf("FATALERROR 硬链接数不为零\n");
    }
    // 首先将空闲索引节点数量加一
    filsys.s_ninode++;
    // 如果当前空闲索引节点区还没满
    if (filsys.s_pinode > 0)
    {
        // 那么就直接把这个节点加到空闲索引节点区中
        filsys.s_inode[--filsys.s_pinode] = dinodeid;
    }
    // 如果当前空闲索引节点区已经满了
    else
    {
        // 那么更新第一个未被记录的空闲索引节点的值
        if (dinodeid < filsys.s_rinode)
        {
            // filsys.s_inode[NICINOD] = dinodeid;
            filsys.s_rinode = dinodeid;
        }
    }
}