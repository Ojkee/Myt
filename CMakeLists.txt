cmake_minimum_required(VERSION 3.15)
project(Myt CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wall -Wextra -pedantic -Wconversion -Wsign-conversion)
endif()

include_directories(include)

find_package(PkgConfig REQUIRED)
pkg_check_modules(RAYLIB REQUIRED raylib)

include_directories(${RAYLIB_INCLUDE_DIRS})
link_directories(${RAYLIB_LIBRARY_DIRS})

file(GLOB_RECURSE CORE_SRC src/*.cc)
list(FILTER CORE_SRC EXCLUDE REGEX ".*main\\.cc$")

# -------------------------
# 1. Main app
# -------------------------
add_executable(myt
    ${CORE_SRC}
    src/main.cc
)

target_link_libraries(
  myt
  PRIVATE ${RAYLIB_LIBRARIES}
)

# -------------------------
# 2. Tests
# -------------------------
file(GLOB_RECURSE TEST_SRC tests/*.cc)
list(FILTER TEST_SRC EXCLUDE REGEX ".*test_cfg_main\\.cc$")
add_executable(myt_tests
    ${CORE_SRC}
    ${TEST_SRC}
    tests/test_cfg_main.cc
)

target_link_libraries(myt_tests PRIVATE ${RAYLIB_LIBRARIES})

target_include_directories(myt_tests PRIVATE tests)

enable_testing()
add_test(NAME MytTests COMMAND myt_tests)
