cmake_minimum_required(VERSION 3.9.0)
project(order_book VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

# set base path
set(BASE_PATH ${PROJECT_SOURCE_DIR})

# compile option:
SET(CMAKE_BUILD_TYPE "Release")
add_compile_options(-O2 -std=c++17 )
add_compile_options(-march=native -mavx -flto)
# include & link
set(PROJ_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/include)
include_directories(
    ${PROJ_INCLUDE_PATH}
    ${PROJ_INCLUDE_PATH}/**
)

# output path
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BASE_PATH}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BASE_PATH}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BASE_PATH}/bin)

link_directories(
    ${BASE_PATH}/lib
    ${BASE_PATH}/lib/**
)

add_library(orderBook src/base.cpp)
target_link_libraries(orderBook pthread)

add_executable(order_book_test main.cpp)
target_link_libraries(order_book_test orderBook)

# add unit test
add_subdirectory(./unit_test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
