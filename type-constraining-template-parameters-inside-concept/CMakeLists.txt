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
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall)
add_compile_options(-stdlib=libc++)
add_compile_options(-g)
add_compile_options(-O)
add_link_options(-stdlib=libc++)

# Add our dependencies
#

foreach(prog
    static_assert
    static_assert_v2
    )
  add_executable(${prog} src/${prog}.cpp)
  target_link_libraries(${prog})
endforeach()

