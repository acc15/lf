cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0092 NEW) # Disable /W3 flag for MSVC by default

project(leafsync LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -DUNICODE -D_UNICODE)
    add_compile_options(/utf-8 /W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(fmt REQUIRED)
find_package(Catch2 REQUIRED)
link_libraries(fmt::fmt)

add_library(leafsync_core     
    src/util/bitset.hpp
    src/util/bitset.cpp
    src/fs/util.hpp 
    src/fs/util.cpp
    src/cmd/cmd_desc.hpp
    src/cmd/cmd.hpp 
    src/cmd/cmd.cpp
    src/cmd/sync_cmd.hpp 
    src/cmd/sync_cmd.cpp
    src/cmd/cmd_registry.hpp 
    src/cmd/cmd_registry.cpp
)

add_executable(leafsync src/main.cpp)
target_link_libraries(leafsync PRIVATE leafsync_core)

add_executable(leafsync_test
    test/util/bitset.cpp
    test/config/config.cpp
    test/unicode/unicode.cpp
)
target_include_directories(leafsync_test PRIVATE src)
target_link_libraries(leafsync_test PRIVATE leafsync_core Catch2::Catch2WithMain)

# install(TARGETS leafsync
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
