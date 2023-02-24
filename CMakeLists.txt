cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0092 NEW) # Disable /W3 flag for MSVC by default

project(lf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(cppcheck NAMES cppcheck)
if (cppcheck)
    set(CMAKE_CXX_CPPCHECK ${cppcheck} "--enable=warning" "--inline-suppr")
endif()

if (UNIX)
    set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local")
elseif(WIN32)
    file(TO_CMAKE_PATH "$ENV{LOCALAPPDATA}\\lf" CMAKE_INSTALL_PREFIX)
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

    # log filter
    src/log/filter/log_filter.hpp
    src/log/filter/log_filter.cpp
    src/log/filter/log_level_filter.hpp
    src/log/filter/log_level_filter.cpp
    
    # log formatter
    src/log/formatter/log_formatter.hpp
    src/log/formatter/simple_log_formatter.hpp
    src/log/formatter/simple_log_formatter.cpp

    # log sink
    src/log/sink/log_sink.hpp
    src/log/sink/log_sink.cpp
    src/log/sink/composite_log_sink.hpp
    src/log/sink/composite_log_sink.cpp
    src/log/sink/stream_log_sink.hpp
    src/log/sink/stream_log_sink.cpp
    src/log/sink/rolling_file_info.hpp
    src/log/sink/rolling_file_info.cpp
    src/log/sink/rolling_file_sink.hpp
    src/log/sink/rolling_file_sink.cpp

    # log
    src/log/log_level.hpp
    src/log/log_level.cpp
    src/log/log_message.hpp
    src/log/log_stream.hpp
    src/log/log_stream.cpp
    src/log/log.hpp
    src/log/log.cpp
    src/log/log_config.hpp
    src/log/log_config.cpp

    # io
    src/io/null_stream.hpp
    src/io/null_stream.cpp
    src/io/ios_flags.hpp
    src/io/ios_flags.cpp
    src/io/format_stream.hpp
    src/io/format_stream.cpp
    src/io/joiner.hpp
    src/io/format.hpp
    src/io/time.cpp
    src/io/time.hpp
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
    src/fs/util.hpp 
    src/fs/util.cpp
    src/fs/time.hpp 
    src/fs/time.cpp
    src/fs/serialization.hpp
    src/fs/os_path.cpp
    src/fs/os_path.hpp
    
    # sync
    src/sync/synchronizer.cpp
    src/sync/synchronizer.hpp
    src/sync/entry_synchronizer.cpp
    src/sync/entry_synchronizer.hpp
    src/sync/path_info.hpp
    src/sync/path_info.cpp
    src/sync/sync_item.hpp

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

install(TARGETS lf)

add_executable(lf_test 
    ${executable_src}
    test/main.cpp
    test/test_util.hpp
    test/test_util.cpp
    test/unicode/unicode.cpp

    test/tree/tree.cpp
    test/opts/opts.cpp

    test/log/sink/rolling_file_info.cpp
    test/log/sink/rolling_file_sink.cpp
    
    test/cmd/cmd.cpp
    test/io/log_tester.hpp
    test/io/log_tester.cpp
    test/io/time.cpp
    test/io/ios_flags.cpp

    test/config/config_parser.cpp
    test/config/config.cpp
    test/fs/util.cpp
    test/fs/time.cpp

    test/fs/serialization.cpp
    test/util/pick.cpp
    test/util/string.cpp

    test/sync/synchronizer.cpp
    test/sync/entry_synchronizer.cpp
    
    test/fs/adv_fstream.cpp
)

target_include_directories(lf_test PRIVATE test)
target_link_libraries(lf_test PRIVATE lf_core Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(lf_test)

