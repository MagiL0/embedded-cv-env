SET(CMAKE_C_COMPILER "C:/mingw64/bin/gcc.exe")
set (CMAKE_CXX_COMPILER "C:/mingw64/bin/g++.exe")


cmake_minimum_required (VERSION 3.22)
# 项目信息
project (TestVision)

# 指定 C 版本
# set(CMAKE_C_STANDARD 99)

# 报错
# if (MSVC)
#     # warning level 4 and all warnings as errors
#     add_compile_options(/W4 /WX)
# else()
#     # lots of warnings and all warnings as errors
#     add_compile_options(-Wall -Wextra -pedantic -Werror)
# endif()

# 添加头文件
include_directories(
    ${PROJECT_SOURCE_DIR}/include/
)

# 添加源文件
file(GLOB DIR_SRCS 
    ${PROJECT_SOURCE_DIR}/src/*.c
)
# 把src目录下所有源文件写入变量DIR_SRCS
# aux_source_directory(./src DIR_SRCS)


# 指定生成目标
add_executable(testVision ${DIR_SRCS})
