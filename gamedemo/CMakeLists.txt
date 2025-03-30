project(GameDemo)

# api示例demo
# file(GLOB_RECURSE LIST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/Example.cpp")

# 2d rpg示例demo
file(GLOB_RECURSE LIST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# 找包
set(CMAKE_PREFIX_PATH "D:/MyProgramFile/other/CPPLibs/libtorch")  # 指向 libtorch 根目录
find_package(Torch REQUIRED)

# resource.rc 是最后exe生成时所带的资源图标
add_executable(${PROJECT_NAME} ${LIST_SRC} resource.rc)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    "${CMAKE_SOURCE_DIR}/src/"
    ${TORCH_INCLUDE_DIRS}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/Assets/"
    TRAIN_PATH="${CMAKE_CURRENT_SOURCE_DIR}/script/"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    GameAF
    ${TORCH_LIBRARIES}
)