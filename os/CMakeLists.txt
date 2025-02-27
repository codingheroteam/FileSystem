# 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 项目信息
project(FileSystemProject)

# 设置默认构建类型为 Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加debug参数
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fbounds-check")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fbounds-check")

# 包含头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 设置编译器选项，指定源文件编码和执行编码为GBK，解决windows下乱码问题
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fexec-charset=GBK")
endif ()

# 文件管理系统源文件
set(FS_SOURCES
    ./src/fs/dir.cpp
    ./src/fs/format.cpp
    ./src/fs/install.cpp
    ./src/fs/igetput.cpp
    ./src/fs/iallfre.cpp
    ./src/fs/ballfre.cpp
    ./src/fs/log.cpp
    ./src/fs/fs.cpp
    ./src/fs/name.cpp
    ./src/fs/access.cpp
    ./src/fs/create.cpp
    ./src/fs/open.cpp
    ./src/fs/rdwt.cpp
    ./src/fs/halt.cpp
    ./src/fs/close.cpp
    ./src/fs/mod.cpp
    ./src/fs/rmfidir.cpp
)


# 命令支持源文件
set(CMD_SOURCES
    ./src/cmd/cmd.cpp
    ./src/cmd/cmd_base.cpp
    ./src/cmd/cmd_bash.cpp
    ./src/cmd/cmd_cat.cpp
    ./src/cmd/cmd_chmod.cpp
    ./src/cmd/cmd_cp.cpp
    ./src/cmd/cmd_help.cpp
    ./src/cmd/cmd_link.cpp
    ./src/cmd/cmd_rename.cpp
    ./src/cmd/cmd_rm.cpp
    ./src/cmd/cmd_test.cpp
    ./src/cmd/cmd_user.cpp
    ./src/cmd/cmd_writer.cpp
)

# 源文件
set(TEST_SOURCES
    ./src/app/main.cpp
    ${FS_SOURCES}
    ${CMD_SOURCES}
)

# 添加可执行文件
add_executable(main ${TEST_SOURCES})

# 链接库
if (WIN32)
    target_link_libraries(main ws2_32)
    set_target_properties(main PROPERTIES DEBUG_POSTFIX "_d")
    set_target_properties(main PROPERTIES RELEASE_POSTFIX "_r")
else ()
    target_link_libraries(main pthread)
    set_target_properties(main PROPERTIES DEBUG_POSTFIX "_d")
    set_target_properties(main PROPERTIES RELEASE_POSTFIX "_r")
endif ()
