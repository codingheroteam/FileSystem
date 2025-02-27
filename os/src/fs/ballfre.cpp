#include <stdio.h>
#include "fs.h"

/* 
 * 磁盘块分配与释放函数 
 * By Brayden
 * 
 */


// 每次从磁盘读出一块数据的缓存区
static unsigned int block_buf[BLOCKSIZ / sizeof(unsigned int)];

// 申请一个空闲块
unsigned int balloc()
{
    debug("申请空闲块");
    unsigned int free_block, free_block_num;
    unsigned int i;
    // 空闲块数量为0
    if (filsys.s_nfree == 0)
    {
        seterror("磁盘空间已满");
        return DISKFULL;
    };
    // 空闲块号
    free_block = filsys.s_free[filsys.s_pfree];
    // 若超级块中空闲块仅剩最后一个，即仅剩下一组空闲块的指针了
    if (filsys.s_pfree == 0)
    {
        debug("链表重连");
        // 从磁盘读取free_block这个块，要把这个块的内容复制到超级块的空闲块表中，因为这个块要被占用了
        fseek(fd, DATASTART + BLOCKSIZ * free_block, SEEK_SET);
        fread(block_buf, 1, BLOCKSIZ, fd);
        // 进行块复制
        for (i = 0; i < NICFREE; i++)
        {
            filsys.s_free[i] = block_buf[i];
        }
        // 因为其他块必然都是满 NICFREE 个的，所以指针直接设为末尾就行了
        filsys.s_pfree = NICFREE - 1;
    }
    // 若还有很多，那处理就很容易了
    else
    {
        filsys.s_pfree--;
    }
    filsys.s_nfree--;
    filsys.s_fmod = SUPDATE;
    debug("申请的块号：");
    debug(free_block);
    return free_block;
}
using namespace std;
// 还回一个空闲块
void bfree(unsigned int block)
{
    // cout << "free" << block << endl;
    int i;
    // 如果超级块中空闲块数组满了
    if (filsys.s_pfree == NICFREE - 1)
    {
        // 那么就把超级块中空闲块数组直接搬到block块里，再将超级块中空闲块数组指向block块
        // 进行块复制
        for (i = 0; i < NICFREE; i++)
        {
            block_buf[i] = filsys.s_free[i];
        }

        fseek(fd, DATASTART + block * BLOCKSIZ, SEEK_SET);
        fwrite(block_buf, 1, BLOCKSIZ, fd);

        filsys.s_free[0] = block;
        filsys.s_pfree = 0;
    }
    // 没满的话就是简简单单的直接加入
    else
    {
        filsys.s_free[++filsys.s_pfree] = block;
    }
    filsys.s_nfree++;
    filsys.s_fmod = SUPDATE;
}