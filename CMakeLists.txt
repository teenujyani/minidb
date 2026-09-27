cmake_minimum_required(VERSION 3.16)

project(MiniDB
    VERSION 0.1.0
    DESCRIPTION "A relational database engine built from scratch in C++"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(minidb
    src/main.cpp
    src/storage/page.cpp
    src/storage/disk_manager.cpp
)

target_include_directories(minidb
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)