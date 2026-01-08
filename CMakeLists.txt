cmake_minimum_required(VERSION 3.20)
project(TypeTreeRipper)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if (MSVC)
    add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:/Zc:preprocessor>)
endif()

add_subdirectory(source)
