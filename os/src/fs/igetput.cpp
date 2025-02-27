#include <stdio.h>
#include "fs.h"
#include <malloc.h>


/* 
 * 获取释放索引节点内容 
 * By Brayden
 * 
 */
using namespace std;

struct inode *iget(unsigned int dinodeid)
{
    // cout << "iget" << dinodeid << endl;

    int inodeid = dinodeid % NHINO;
    // 根据id找到哈希链表
    struct inode* temp = hinode[inodeid].i_forw;

    // 在哈希表中查找是否存在请求节点，若存在，直接返回指针
    while (temp != NULL) {
        // 找到则直接返回
        if (temp->i_ino == dinodeid) {
            temp->i_count++;
            // debug("命中");
            // cout << "命中" << dinodeid << " " << temp->i_ino << endl;
            return temp;
        }
        // 不断向下寻找
        temp = temp->i_forw;
    }

    // 哈希表中没有，看来只能从磁盘读取，然后加到哈希表了
    // 首先计算要请求的节点在磁盘的位置
    long addr = DINODESTART + dinodeid * DINODESIZ;
    // 申请内存
    struct inode *newinode;

    newinode = (struct inode *)malloc(sizeof(struct inode));

    if (newinode == NULL)
    {
        // 申请失败
        printf("内存不足\n");
        exit(EXIT_FAILURE);
    }

    debug(addr);
    // 定位磁盘
    fseek(fd, addr, SEEK_SET);
    fread(&newinode->di_number, sizeof(struct dinode), 1, fd);

    newinode->i_forw = hinode[inodeid].i_forw;
    if (newinode->i_forw)
    {
        newinode->i_forw->i_back = newinode;
    }

    newinode->i_back = (struct inode*)&hinode[inodeid]; // 待定
    hinode[inodeid].i_forw = newinode;

    newinode->i_count = 1; 
    newinode->i_flag = 'a';
    newinode->i_ino = dinodeid;
    return newinode;
}

void iput(struct inode *pinode) /* iput ( ) */
{
    long addr;
    unsigned int block_num;
    int i;
    if(pinode == 0) {
        return;
    }
    // cout << "iput" << pinode->i_ino << endl;
    
    // 仍有引用，则暂时不写入磁盘
    if (pinode->i_count > 1)
    {
        pinode->i_count--;
        return;
    }
    // 没有引用了，要写入磁盘
    else
    {
        // 此文件未被删除
        if (pinode->di_number != 0)
        {
            // cout << "写回" << pinode->i_ino << endl;
            /*write back the mode */
            addr = DINODESTART + pinode->i_ino * DINODESIZ;
            // DINODESTART + dinodeid * DINODESIZ;
            fseek(fd, addr, SEEK_SET);
            fwrite(&pinode->di_number, sizeof(struct dinode), 1, fd);
            // cout << "iput 写入 " << pinode->i_ino << "数据块地址" << pinode->di_addr[0] << " 大小" << pinode->di_size << endl;
        }

        // 此文件已被删除
        else
        {
            // cout << "删掉" << endl;
            // /*    rm the mode & the block of the file in the disk */
            // 计算文件占用了多少个块
            for (i = 0;i < getInodeBlockNum(pinode); i++)
            {
                // 释放这些块
                bfree(pinode->di_addr[i]);
            }
            ifree(pinode);
        };

        /*    free the mode in the memory */
        if (pinode->i_forw == NULL)
            pinode->i_back->i_forw = NULL;
        else {
            pinode->i_forw->i_back = pinode->i_back;
            pinode->i_back->i_forw = pinode->i_forw;
        };
        free(pinode);
    };
}