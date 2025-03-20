cmake_minimum_required(VERSION 3.14)
project(disk_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)

find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBURING IMPORTED_TARGET liburing)

add_executable(sync_access sync_access.cc)
add_executable(batch_access batch_access.cc)
add_executable(generate_blob generate_blob.cc)
add_executable(test_uring test_uring.cc)
add_executable(test_read test_read.cc)

target_link_libraries(sync_access PRIVATE Threads::Threads)
target_link_libraries(batch_access PRIVATE Threads::Threads PkgConfig::LIBURING)
target_link_libraries(test_uring PRIVATE Threads::Threads PkgConfig::LIBURING)