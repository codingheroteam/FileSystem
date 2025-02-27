#include "cmd.h"
#include "../fs/fs.h"
#include <iostream>

using namespace std;


/**
 * @brief		chmod命令
 * @author		Brayden
 * @date		2024-06
 */


int cmd_chmod(int argc, char* argv[]) {
    string mode = argv[1];
    int u = mode[0] - '0';
    int g = mode[1] - '0';
    int o = mode[2] - '0';
    
    if(chmod(argv[2], u | (g << 4) | (o << 8)) == SYSCALL_FAIL) {
        printerror("chmod");
    }
    
    return 0;
}