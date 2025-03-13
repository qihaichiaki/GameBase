project(GameDemo)

# api示例demo
file(GLOB_RECURSE LIST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/Example.cpp")

# 2d rpg示例demo
# file(GLOB_RECURSE LIST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# resource.rc 是最后exe生成时所带的资源图标
add_executable(${PROJECT_NAME} ${LIST_SRC} resource.rc)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    "${CMAKE_SOURCE_DIR}/src/"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/Assets/"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    GameAF
)