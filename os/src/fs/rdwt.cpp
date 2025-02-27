#include <stdio.h>
#include "fs.h"
#include <math.h>

/* 
 * 读写文件 
 * By Brayden, Peter
 * 
 */


unsigned int s_read(int ifd, void *buf, unsigned int size)
{
    unsigned long off;
    int block, block_off, i, j, file_idx;
    struct inode *inode;
    char *temp_buf;

    if (ifd < 0 || ifd >= NOFILE)
    {
        seterror("非法文件描述符");
        return 0;
    }

    file_idx = user[user_id].u_ofile[ifd];

    if (file_idx == SYSOPENFILE)
    {
        seterror("未使用的文件描述符");
        return 0;
    }

    inode = sys_ofile[file_idx].f_inode;
    if (!(sys_ofile[file_idx].f_flag & FREAD))
    {
        seterror("未以读模式打开");
        return 0;
    }

    temp_buf = (char *)buf;
    off = sys_ofile[file_idx].f_off;
    // 若欲读的大小大于文件大小，则将欲读大小设为正好到达文件尾
    if ((off + size) > inode->di_size)
        size = inode->di_size - off;
    block_off = off % BLOCKSIZ;
    block = off / BLOCKSIZ;
    if (block_off + size <= BLOCKSIZ)
    {
        fseek(fd, DATASTART + inode->di_addr[block] * BLOCKSIZ + block_off, SEEK_SET);
        size = fread(temp_buf, 1, size, fd);
        temp_buf[size] = 0;
        // 然后要更新文件的偏移量
        sys_ofile[user[user_id].u_ofile[ifd]].f_off = off + size;

        return size;
    }

    // 下面是一个块写不下的情况
    // 实际已经读取到的大小
    int cursize = 0;
    // 首先将当前块读完
    fseek(fd, DATASTART + inode->di_addr[block] * BLOCKSIZ + block_off, SEEK_SET);
    cursize += fread(temp_buf, 1, BLOCKSIZ - block_off, fd);

    // 下面是剩下的未读入数据，若仍满一个块，那就直接读
    temp_buf += cursize;
    for (i = 0; i < (size - cursize) / BLOCKSIZ; i++)
    {

        fseek(fd, DATASTART + inode->di_addr[block + 1 + i] * BLOCKSIZ, SEEK_SET);
        fread(temp_buf, 1, BLOCKSIZ, fd);
        temp_buf += BLOCKSIZ;
    }

    block_off = (size + off) % BLOCKSIZ;
    block = (size + off) / BLOCKSIZ;
    fseek(fd, DATASTART + inode->di_addr[block] * BLOCKSIZ, SEEK_SET);
    fread(temp_buf, 1, block_off, fd);
    sys_ofile[user[user_id].u_ofile[ifd]].f_off += size;

    return size;
}

unsigned int s_write(int ifd, const void *buf, unsigned int size)
{
    unsigned short off;
    int block, block_off, i, j, file_idx;
    struct inode *inode;
    char *temp_buf;

    if (ifd < 0 || ifd >= NOFILE)
    {
        seterror("非法文件描述符");
        return -1;
    }

    file_idx = user[user_id].u_ofile[ifd];

    if (file_idx == SYSOPENFILE)
    {
        seterror("未使用的文件描述符");
        return 0;
    }

    inode = sys_ofile[file_idx].f_inode;
    if (!(sys_ofile[file_idx].f_flag & FWRITE || sys_ofile[file_idx].f_flag & FAPPEND || sys_ofile[file_idx].f_flag & FPLUS))
    {
        seterror("未以写模式打开");
        return 0;
    }

    temp_buf = (char *)buf;

    off = sys_ofile[file_idx].f_off;    // 文件偏移量
    block_off = off % BLOCKSIZ;         // 块内偏移量
    block = off / BLOCKSIZ;             // 起始块号

    // 计算这个文件已经用了几个块了
    int blocknum = getInodeBlockNum(inode);

    // 计算写完size一共需要几个块
    int tarblocknum = (off + size + (BLOCKSIZ - 1)) / BLOCKSIZ;

    if (tarblocknum >= NADDR)
    {
        seterror("超出最大文件大小限制");
        return 0;
    }


    // 首先一次性申请所有需要的块
    int succ = 1; // 是否成功标记
    for(i = blocknum; i < tarblocknum; i++) {
        unsigned int b = balloc();
        // 分配失败，退出并标记失败
        if(b == DISKFULL) {
            succ = 0;
            break;
        }
        // 分配成功
        inode->di_addr[i] = b;
    }

    // 若分配失败，需要返还已申请的块，然后退出
    if(!succ) {
        for (j = blocknum; j < i; j++) {
            bfree(inode->di_addr[j]);
            inode->di_addr[j] = 0;
        }
        seterror("无法写入，空间不足");
        return 0;
    }


    // 下面开始逐个块写入
    int bufcursor = 0;  // buf指针，当bufcursor==size时，说明已全部写入
    while(1) {
        // 已写完
        if(bufcursor == size) {
            break;
        }
        // 首先找到写入位置
        unsigned int datacursor = off + bufcursor; // 文件数据位置指针，指明当前在文件总体数据中是第几个字符
        unsigned int diskblock = inode->di_addr[datacursor / BLOCKSIZ]; // 根据文件数据位置指针，可以算出其在磁盘上的块号
        unsigned int diskblockoff = datacursor % BLOCKSIZ; // 根据文件数据位置指针，可以算出其在磁盘上的块偏移地址
        unsigned int stepsize = std::min(BLOCKSIZ - diskblockoff, size - bufcursor); // 本次能够写入的大小，受块大小限制和剩于未写入大小限制
        // 磁盘寻址到写入位置
        fseek(fd, DATASTART + diskblock * BLOCKSIZ + diskblockoff, SEEK_SET);
        // 执行写入
        fwrite(temp_buf + bufcursor, 1, stepsize, fd);
        // 更新buf指针
        bufcursor += stepsize;
    }

    // 更新文件参数
    sys_ofile[user[user_id].u_ofile[ifd]].f_off += size;
    inode->di_size = std::max(inode->di_size, (short unsigned int)(off + size));
    debug("更新文件大小为");
    debug(inode->di_size);
    return size;
}