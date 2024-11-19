cmake_minimum_required(VERSION 3.30)

project(tsmap LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ReleaseLocal)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_RELEASELOCAL "-O3 -march=native")

aux_source_directory(. SRC_LIST)
add_executable(server ${SRC_LIST})

add_subdirectory(pugixml)
include_directories(pugixml/src)

add_subdirectory(nlohmann_json)

find_package(Crow REQUIRED)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.83.0 COMPONENTS)
include_directories(${Boost_INCLUDE_DIRS}) 

target_link_libraries(server PUBLIC Crow::Crow pugixml nlohmann_json::nlohmann_json pqxx)
target_compile_features(server PRIVATE cxx_std_23)


install(TARGETS server)