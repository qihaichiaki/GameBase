project(GameAF)

file(GLOB_RECURSE LIST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(${PROJECT_NAME} STATIC ${LIST_SRC})

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
    GAMEAF_USE_EASYX
)

# 包含头文件 easyx
target_include_directories(${PROJECT_NAME}
    PRIVATE
    "${THIRDPARTY_PATH}/easyx/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/global"
    "${CMAKE_CURRENT_SOURCE_DIR}/common"
    "${CMAKE_CURRENT_SOURCE_DIR}/game_object"
    "${CMAKE_CURRENT_SOURCE_DIR}/scene"
)
