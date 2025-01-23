project(GameAF)

file(GLOB_RECURSE LIST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(${PROJECT_NAME} STATIC ${LIST_SRC})

# 包含头文件 easyx
target_include_directories(${PROJECT_NAME}
    PRIVATE
    "${PUBLIC_THIRDPARTY}/EasyX/include"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    "${PUBLIC_THIRDPARTY}/easyx/lib/EasyXw.lib"
)
