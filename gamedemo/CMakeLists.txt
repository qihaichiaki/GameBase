project(GameDemo)

file(GLOB_RECURSE LIST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(${PROJECT_NAME} ${LIST_SRC})

target_include_directories(${PROJECT_NAME}
    PRIVATE
    "{CMAKE_SOURCE_DIR}/src"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    GameAF
)