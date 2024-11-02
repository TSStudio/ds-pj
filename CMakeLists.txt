cmake_minimum_required(VERSION 3.16)
project(tsmap LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
 

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

aux_source_directory(. SRC_LIST)
add_executable(server ${SRC_LIST})

add_subdirectory(pugixml)
include_directories(pugixml/src)

add_subdirectory(nlohmann_json)

find_package(Crow)


target_link_libraries(server PUBLIC Crow::Crow pugixml nlohmann_json::nlohmann_json)
target_compile_features(server PRIVATE cxx_std_20)


install(TARGETS server)