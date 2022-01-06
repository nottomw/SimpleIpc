cmake_minimum_required(VERSION 2.8)

include(CompilerOptions.cmake)

project(SimpleIpcLib CXX)

add_library(SimpleIpcLib STATIC
    SimpleIpc.cpp
    SharedMem.cpp
    RelativeAllocator.cpp
)

add_library(SimpleIpcLib::SimpleIpcLib ALIAS SimpleIpcLib)

# Test exec
add_executable(SimpleIpc
    main.cpp
)

target_link_libraries(SimpleIpc
    PRIVATE
        SimpleIpcLib::SimpleIpcLib
)
