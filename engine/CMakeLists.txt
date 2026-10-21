cmake_minimum_required(VERSION 3.25)

find_package(RmlUi CONFIG REQUIRED)

add_library(engine
        src/core/Engine.cpp
        src/ui/NullRenderInterface.cpp
        src/ui/LoggingSystemInterface.cpp
)

target_include_directories(engine PUBLIC
        include
        "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/include"
)

target_link_libraries(engine PUBLIC
        project_sanitizers
        project_warnings
        RmlUi::Core
)

target_compile_features(engine PUBLIC cxx_std_20)
