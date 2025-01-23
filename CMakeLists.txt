cmake_minimum_required(VERSION 3.30.0)
project(GameBase)

set(CMAKE_CXX_STANDARD 17)

# CXX相关编译使用utf-8编码
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# 设置公共三方库路径
set(THIRDPARTY_PATH "${CMAKE_SOURCE_DIR}/thirdparty")

# 设置编译输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_NAME}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/bin")

# 子工程添加目录
add_subdirectory(src)

add_subdirectory(gamedemo)