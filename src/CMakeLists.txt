project(GameAF)

file(GLOB_RECURSE LIST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

add_library(${PROJECT_NAME} STATIC ${LIST_SRC})

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
    GAMEAF_USE_EASYX
)

# 包含头文件 easyx
target_include_directories(${PROJECT_NAME}
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/"
    "${THIRDPARTY_PATH}/easyx/include/"
)
