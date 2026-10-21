cmake_minimum_required(VERSION 3.25)

add_executable(Game
        src/main.cpp
        src/ClickListener.cpp
)

target_link_libraries(Game PRIVATE engine)

target_link_libraries(Game PRIVATE
    project_sanitizers
    project_warnings
)

target_include_directories(Game PRIVATE ../engine)
target_link_directories(Game PRIVATE ../engine/cmake-build-debug)
target_compile_definitions(Game PRIVATE POC_ASSET_DIR="${CMAKE_SOURCE_DIR}/assets")