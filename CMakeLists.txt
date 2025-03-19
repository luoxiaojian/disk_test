cmake_minimum_required(VERSION 3.14)
project(disk_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)

find_package(Threads REQUIRED)

add_executable(sync_access sync_access.cc)
add_executable(generate_blob generate_blob.cc)

target_link_libraries(sync_access PRIVATE Threads::Threads)