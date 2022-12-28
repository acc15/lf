cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0092 NEW) # Disable /W3 flag for MSVC by default

project(lf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(executable_src)
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/utf-8 /W4 /Zc:__cplusplus)
    
    # See
    # https://learn.microsoft.com/en-us/windows/apps/design/globalizing/use-utf8-code-page
    # https://habr.com/ru/company/ruvds/blog/645325/
    list(APPEND executable_src src/utf8.manifest)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type)
endif()

find_package(Catch2 REQUIRED)

add_library(lf_core  

    # tree
    src/tree/tree.hpp
    src/tree/tree_print.hpp
    src/tree/tree_binary.hpp
    src/tree/tree_binary.cpp

    # index files
    src/index/sync_mode.hpp
    src/index/sync_mode.cpp
    src/index/indexer.hpp
    src/index/indexer.cpp
    src/index/index_tree.hpp
    src/index/index_tree.cpp

    # config files
    src/config/config.hpp
    src/config/config.cpp
    src/config/config_util.hpp
    src/config/config_parser.hpp
    src/util/string.hpp
    src/util/string.cpp
    src/util/pick.hpp
    src/util/console.hpp
    src/util/console.cpp
    src/io/log.hpp
    src/io/log.cpp
    src/io/null_stream.hpp
    src/io/null_stream.cpp
    src/io/joiner.hpp
    src/io/format.hpp
    src/io/serialization.hpp
    src/fs/path.hpp 
    src/fs/path.cpp
    src/fs/util.hpp 
    src/fs/util.cpp
    src/cmd/cmd_desc.hpp
    src/cmd/cmd_desc.cpp
    src/cmd/cmd.hpp 
    src/cmd/cmd.cpp
    src/cmd/sync_cmd.hpp 
    src/cmd/sync_cmd.cpp
    src/cmd/add_cmd.hpp 
    src/cmd/add_cmd.cpp 
    src/cmd/rm_cmd.hpp 
    src/cmd/rm_cmd.cpp 
    src/cmd/cmd_registry.hpp 
    src/cmd/cmd_registry.cpp
 )

add_executable(lf 
    ${executable_src}
    src/main.cpp
)
target_link_libraries(lf PRIVATE lf_core)

add_executable(lf_test 
    ${executable_src}
    test/main.cpp
    test/test_util.hpp
    test/test_util.cpp
    test/unicode/unicode.cpp

    test/tree/tree.cpp

    test/cmd/cmd_desc.cpp
    test/io/log_tester.hpp
    test/io/log_tester.cpp
    test/io/log.cpp

    test/config/config_parser.cpp
    test/config/config.cpp
    test/fs/path.cpp
    test/util/string.cpp
 )

target_include_directories(lf_test PRIVATE src)
target_link_libraries(lf_test PRIVATE lf_core Catch2::Catch2)

# install(TARGETS lf
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
