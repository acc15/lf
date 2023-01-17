cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0092 NEW) # Disable /W3 flag for MSVC by default

project(lf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (UNIX)
    set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local")
endif()

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
include_directories(src)

add_library(lf_core  

    # tree
    src/tree/tree.hpp
    src/tree/print.hpp
    src/tree/binary.hpp
    src/tree/tracked_tree.hpp

    # index
    src/index/sync_mode.hpp 
    src/index/sync_mode.cpp
    src/index/index.hpp
    src/index/index.cpp
    src/index/indexer.hpp
    src/index/indexer.cpp

    # state
    src/state/state.hpp
    src/state/state.cpp

    # config
    src/config/config.hpp
    src/config/config.cpp
    src/config/config_parser.hpp

    # util
    src/util/string.hpp
    src/util/string.cpp
    src/util/pick.hpp
    src/util/console.hpp
    src/util/console.cpp

    # io
    src/io/log.hpp
    src/io/log.cpp
    src/io/null_stream.hpp
    src/io/null_stream.cpp
    src/io/ios_flags.hpp
    src/io/ios_flags.cpp
    src/io/format_stream.hpp
    src/io/format_stream.cpp
    src/io/joiner.hpp
    src/io/format.hpp
    src/io/datetime.cpp
    src/io/datetime.hpp
    src/io/file_type.cpp
    src/io/file_type.hpp
    src/io/util.hpp
    src/io/util.cpp

    # opts
    src/opts/opt_map.hpp
    src/opts/opt_map.cpp
    src/opts/opt.hpp
    src/opts/opts.hpp
    src/opts/opts.cpp

    # fs
    src/fs/path.hpp 
    src/fs/path.cpp
    src/fs/time.hpp 
    src/fs/time.cpp
    src/fs/error.hpp
    src/fs/error.cpp
    src/fs/serialization.hpp
    
    # sync
    src/sync/synchronizer.cpp
    src/sync/synchronizer.hpp

    # cmd
    src/cmd/cmd.hpp 
    src/cmd/cmd.cpp
    src/cmd/cmd_registry.hpp 
    src/cmd/cmd_registry.cpp
    src/cmd/impl/sync_cmd.hpp 
    src/cmd/impl/sync_cmd.cpp
    src/cmd/impl/add_cmd.hpp 
    src/cmd/impl/add_cmd.cpp 
    src/cmd/impl/rm_cmd.hpp 
    src/cmd/impl/rm_cmd.cpp 
    src/cmd/impl/info_cmd.hpp 
    src/cmd/impl/info_cmd.cpp 
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
    test/opts/opts.cpp
    
    test/cmd/cmd.cpp
    test/io/log_tester.hpp
    test/io/log_tester.cpp
    test/io/log.cpp
    test/io/datetime.cpp
    test/io/ios_flags.cpp

    test/config/config_parser.cpp
    test/config/config.cpp
    test/fs/path.cpp

    test/fs/time.cpp

    test/fs/serialization.cpp
    test/util/string.cpp
    test/util/string_map.cpp

    test/sync/synchronizer.cpp
 )

target_include_directories(lf_test PRIVATE test)
target_link_libraries(lf_test PRIVATE lf_core Catch2::Catch2)

install(TARGETS lf)
