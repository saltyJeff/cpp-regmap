# C++ Regmaps
This library is designed to speed up the development of
register maps (regmaps) for device libraries. It uses
a lot of C++ 17 hacks to make actually writing the drivers
as painless as possible.

## Requirements
* C++ 17 compliant compiler
    - Tested on MinGW

## Installation
This library is header-only. Either copy the headers in `include/regmap.h`
or `git submodule` this folder and `add_subdirectory` in CMake. Link against
the `regmap` CMake library.

## Benefits
* Abuse of `constexpr` means most of the code is compiled out
* Fluent-ish code

## Example
*TODO*