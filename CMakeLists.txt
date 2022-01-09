cmake_minimum_required(VERSION 2.8)

include(CompilerOptions.cmake)

project(SimpleIpcLib CXX)

add_subdirectory(SimpleUtils)

add_library(SimpleIpcLib STATIC
    SimpleIpc.cpp
    SharedMem.cpp
)

add_library(SimpleIpcLib::SimpleIpcLib ALIAS SimpleIpcLib)

target_link_libraries(SimpleIpcLib
    PRIVATE
        SimpleUtils::SimpleUtils
)

# Test exec
add_executable(SimpleIpc
    main.cpp
)

target_link_libraries(SimpleIpc
    PRIVATE
        SimpleIpcLib::SimpleIpcLib
)
