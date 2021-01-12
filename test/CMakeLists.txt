add_executable(regmap_test main.cpp utility_tests.cpp
        static_tests.cpp test_common.h doctest.h
        regmap_test.cpp)

add_executable(regmap_test_compile_sz compileSize.cpp)