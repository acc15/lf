cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0092 NEW) # Disable /W3 flag for MSVC by default

project(leafsync LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(executable_src)
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/utf-8 /W4 /Zc:__cplusplus)
    
    # See
    # https://learn.microsoft.com/en-us/windows/apps/design/globalizing/use-utf8-code-page
    # https://habr.com/ru/company/ruvds/blog/645325/
    list(APPEND executable_src msvc/utf8.manifest)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type)
endif()

find_package(fmt REQUIRED)
find_package(Catch2 REQUIRED)
link_libraries(fmt::fmt)

add_library(leafsync_core     
    src/util/string.hpp
    src/util/string.cpp
    src/util/tokenizer.hpp
    src/util/tokenizer.cpp
    src/util/bitset.hpp
    src/util/bitset.cpp
    src/util/hash.hpp
    src/util/hash.cpp
    src/util/console.hpp
    src/util/console.cpp
    src/err/error_handler.hpp
    src/err/error_handler.cpp
    src/fs/util.hpp 
    src/fs/util.cpp
    src/cmd/cmd_desc.hpp
    src/cmd/cmd.hpp 
    src/cmd/cmd.cpp
    src/cmd/sync_cmd.hpp 
    src/cmd/sync_cmd.cpp
    src/cmd/cmd_registry.hpp 
    src/cmd/cmd_registry.cpp
    src/config/config_mode.hpp
    src/config/config_mode.cpp
    src/config/config.hpp
    src/config/config.cpp
)

add_executable(leafsync 
    ${executable_src}
    src/main.cpp
)
target_link_libraries(leafsync PRIVATE leafsync_core)

add_executable(leafsync_test 
    ${executable_src}
    test/main.cpp
    test/test_util.hpp
    test/test_util.cpp
    test/util/hash.cpp
    test/util/bitset.cpp
    test/util/string.cpp
    test/config/config_mode.cpp
    test/config/config.cpp
    test/unicode/unicode.cpp
    test/io/file.cpp
)
target_include_directories(leafsync_test PRIVATE src)
target_link_libraries(leafsync_test PRIVATE leafsync_core Catch2::Catch2)

# install(TARGETS leafsync
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
