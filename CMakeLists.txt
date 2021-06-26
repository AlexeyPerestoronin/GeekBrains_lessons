cmake_minimum_required(VERSION 3.1.0)

set(PROJECT_CPP_STANDART "cxx_std_17")
set(PROJECT_NAME "GeekBrains")
set(LIB_NAME "RecursiveWalk")

project(${PROJECT_NAME})

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

include_directories(./)

file(GLOB HPP_FILES "./*.hpp")
file(GLOB CPP_FILES "./*.cpp")

add_library(${LIB_NAME} STATIC ${CPP_FILES} ${HPP_FILES})
target_compile_features(${LIB_NAME} PRIVATE ${PROJECT_CPP_STANDART})

add_subdirectory(./tests)
