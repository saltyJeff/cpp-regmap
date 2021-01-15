cmake_minimum_required(VERSION 3.13)
project(regmap)

set(CMAKE_CXX_STANDARD 17)

add_library(regmap INTERFACE )
target_include_directories(regmap INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include/regmap/)
target_sources(regmap INTERFACE ${SRC_ROOT}/regmap.h
        ${SRC_ROOT}/bitset.h
        ${SRC_ROOT}/bus.h
        ${SRC_ROOT}/utils.h
        ${SRC_ROOT}/memoizer.h
        ${SRC_ROOT}/register_utils.h
        ${SRC_ROOT}/register_specialization.h
        ${SRC_ROOT}/alufix.h
        ${SRC_ROOT}/alufix_types.h)

add_subdirectory(test)
target_link_libraries(regmap_test PRIVATE regmap)
target_link_libraries(regmap_test_compile_sz PRIVATE regmap)