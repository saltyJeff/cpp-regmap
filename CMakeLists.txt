cmake_minimum_required(VERSION 3.17)
project(regmap)

set(CMAKE_CXX_STANDARD 17)

add_library(regmap INTERFACE)
target_include_directories(regmap INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include/regmap/)
target_sources(regmap INTERFACE ${SRC_ROOT}/regmap.h
        ${SRC_ROOT}/bitstuff.h
        ${SRC_ROOT}/bus.h
        ${SRC_ROOT}/endianfix.h
        ${SRC_ROOT}/utils.h)

add_subdirectory(test)

target_link_libraries(test PRIVATE regmap)