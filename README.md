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

## Guide
See the [writeup](guide.md)

## Notes on design
The library is designed to leave as small of a memory footprint as possible. Most-everything is
set up as a template, which translates to larger binaries because they have to
be instantiated on each specialization.