cmake_minimum_required(VERSION 3.15)
project(Myt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wall -Wextra -pedantic -Wconversion -Wsign-conversion)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2)

include_directories(include)

file(GLOB_RECURSE CORE_SRC src/*.cc)
list(FILTER CORE_SRC EXCLUDE REGEX ".*main\\.cc$")
file(GLOB_RECURSE HEADER_FILES include/*.hpp)

# -------------------------
# 1. Main App
# -------------------------
qt_add_executable(myt
    ${HEADER_FILES}
    ${CORE_SRC}
    src/main.cc
    resources.qrc
)

target_link_libraries(myt
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
)

# -------------------------
# 2. Tests (opcjonalnie)
# -------------------------
file(GLOB_RECURSE TEST_SRC tests/*.cc)
list(FILTER TEST_SRC EXCLUDE REGEX ".*test_cfg_main\\.cc$")

add_executable(myt_tests
    ${HEADER_FILES}
    ${CORE_SRC}
    ${TEST_SRC}
    tests/test_cfg_main.cc
)

target_link_libraries(myt_tests
    PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::QuickControls2
)

target_include_directories(myt_tests PRIVATE tests)

enable_testing()
add_test(NAME MytTests COMMAND myt_tests)
