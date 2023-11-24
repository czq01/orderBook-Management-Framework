cmake_minimum_required(VERSION 3.9.0)
project(order_book VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

# set base path
set(BASE_PATH ${PROJECT_SOURCE_DIR})

# compile option:
add_compile_options(-O2 -std=c++20 -mavx)

# include & link
set(PROJ_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/include)
include_directories(
    ${PROJ_INCLUDE_PATH}
    ${PROJ_INCLUDE_PATH}/**
)

# add unit test
add_subdirectory(./unit_test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
