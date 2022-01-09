cmake_minimum_required(VERSION 2.8)

include(CompilerOptions.cmake)

project(SimpleIpcLib CXX)

add_subdirectory(SimpleUtils)

add_library(SimpleIpc STATIC
    SimpleIpc.cpp
    SharedMem.cpp
)

add_library(SimpleIpc::SimpleIpc ALIAS SimpleIpc)

target_link_libraries(SimpleIpc
    PRIVATE
        SimpleUtils::SimpleUtils
)

# Test application
add_executable(SimpleIpcTestApp
    TestApp/main.cpp
)

target_link_libraries(SimpleIpcTestApp
    PRIVATE
        SimpleIpc::SimpleIpc
)
