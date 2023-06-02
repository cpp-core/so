cmake_minimum_required (VERSION 3.24 FATAL_ERROR)
project (checking-uniqueness VERSION 0.1 LANGUAGES C CXX ASM)

# Laod the cmake recipes and utilities.
#
include(${CMAKE_CURRENT_LIST_DIR}/cmake/load_cmake_helpers.cmake)

# compile_commands.json
#
symlink_compile_commands()

# Convenience targets for git operations
#
make_submodule_pull_target()

# Put executables in the top-level binary directory
#
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Setup compilation before adding dependencies
#
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall)
add_compile_options(-stdlib=libc++)
add_compile_options(-g)
add_compile_options(-O)
add_link_options(-stdlib=libc++)

add_util()
add_chrono()
add_mdspan()
add_spdlog()
add_cc()
find_package(Eigen3)

set(nanobench_DIR /Users/mmelton/opt/lib/nanobench/cmake)
find_package(nanobench REQUIRED)

# Add our dependencies
#
foreach(prog
    s0 s1 s2 s3 s4 s5 s6 s7 s8 s9 s10 s11 s12 s13 s14 s15 s16 s17 s18 s19
    )
  add_executable(${prog} src/${prog}.cpp)
  target_link_libraries(${prog} util::util chrono::chrono cc::cc)
endforeach()

# Add our dependencies
#
foreach(prog
    p0 p1 p2 p3 p4 p5 p6 p7 p8 p9
    )
  add_executable(${prog} src/${prog}.cpp)
  target_link_libraries(${prog} pthread Eigen3::Eigen nanobench::nanobench)
endforeach()

