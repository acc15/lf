cmake_minimum_required(VERSION 3.5)

project(leafsync LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlohmann_json REQUIRED)
find_package(fmt REQUIRED)

add_executable(leafsync 
    src/main.cpp 
    src/fs/util.hpp src/fs/util.cpp
    src/cmd/cmd.hpp src/cmd/cmd.cpp
    src/cmd/sync_cmd.hpp src/cmd/sync_cmd.cpp
    src/cmd/cmd_registry.hpp src/cmd/cmd_registry.cpp
)
target_link_libraries(leafsync PRIVATE fmt::fmt nlohmann_json::nlohmann_json)

if(MSVC)
  target_compile_options(leafsync PRIVATE /W4)
else()
  target_compile_options(leafsync PRIVATE -Wall -Wextra -Wpedantic)
endif()

# install(TARGETS leafsync
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
