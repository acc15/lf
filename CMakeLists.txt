cmake_minimum_required(VERSION 3.5)

project(leafsync LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(fmt REQUIRED)
find_package(Catch2 REQUIRED)
link_libraries(fmt::fmt)

set(src
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

set (test
    test/util/bitset.cpp
)

add_executable(leafsync ${src} src/main.cpp)
add_executable(leafsync_test ${src} ${test} )
target_include_directories(leafsync_test PRIVATE src)
target_link_libraries(leafsync_test PRIVATE Catch2::Catch2WithMain)

# install(TARGETS leafsync
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
