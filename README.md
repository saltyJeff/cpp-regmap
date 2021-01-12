# C++ Regmaps
This library is designed to speed up the development of
register maps (regmaps) for device libraries. It uses
a lot of C++ 17 hacks to make actually writing the drivers
as painless as possible.

## Requirements
* C++ 17 compliant compiler
    - Tested on MinGW
    - Tested on g++ 8.3.0
* An implementation of:
    * `#include <type_traits>`
    * `#include <cstdint>`

## Installation
This library is header-only. Either copy the headers in `include/regmap/`
or `git submodule` this folder and `add_subdirectory` in CMake. Link against
the `regmap` CMake library.

## Benefits
* Fluent-ish code (see the writeup)
* Abuse of `constexpr` means most of the generics are compiled out
* No dynamic allocation (cry about OOM at compile-time!)
* Automatically handles memoization
* Long compile times from the template unrolling gives you an excuse to take longer breaks

## Guide
See the [writeup](guide.md)

## Notes on design
The library is designed to leave as small of a memory footprint as possible. Most-everything is
set up as a template, which translates to larger binaries because they have to
be instantiated on each specialization. However, unlike the C purists, our fancy
C++ abstraction has 0 dynamic allocation and compile-time warnings.
