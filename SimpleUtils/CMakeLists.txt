cmake_minimum_required(VERSION 2.8)

add_library(SimpleUtils STATIC
    RelativeAllocator.cpp
)

add_library(SimpleUtils::SimpleUtils ALIAS SimpleUtils)

