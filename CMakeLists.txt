cmake_minimum_required(VERSION 3.20)
project(RVC LANGUAGES C CXX)

# Production code: C11 (GCC). Unit tests: C++ with GoogleTest (G++).
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(RVC_COVERAGE "Instrument the C production code for gcov coverage" ON)

set(RVC_C_WARNINGS -Wall -Wextra -Wpedantic -Werror)
set(RVC_SRC ${CMAKE_SOURCE_DIR}/src)
set(RVC_INC ${CMAKE_SOURCE_DIR}/include)

# ---------------------------------------------------------------------------
# GoogleTest
# ---------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
    GIT_SHALLOW    TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

# ---------------------------------------------------------------------------
# Main (compiled only; its Tick loop never ends and it needs platform ports,
# SC Section 5.5 / 9 item 5). Tests use rvc_startup()/rvc_tick() instead.
# ---------------------------------------------------------------------------
add_library(rvc_main_obj OBJECT ${RVC_SRC}/main.c)
target_include_directories(rvc_main_obj PRIVATE ${RVC_INC})
target_compile_options(rvc_main_obj PRIVATE ${RVC_C_WARNINGS})

# ---------------------------------------------------------------------------
# rvc_add_unit_test(<name> MODULES <module...> DOUBLES <double...>)
#   MODULES : production modules (src/<m>.c) under test, coverage-instrumented
#   DOUBLES : link-seam test doubles (tests/doubles/<d>.c)
#   The test source is tests/<name>.cpp.
# Each executable links its own copy of the modules it needs, so a test double
# can replace a real module at link time without hooks in production code.
# ---------------------------------------------------------------------------
function(rvc_add_unit_test name)
    cmake_parse_arguments(ARG "" "" "MODULES;DOUBLES" ${ARGN})

    set(mod_srcs "")
    foreach(m IN LISTS ARG_MODULES)
        list(APPEND mod_srcs ${RVC_SRC}/${m}.c)
    endforeach()
    add_library(${name}_modules STATIC ${mod_srcs})
    target_include_directories(${name}_modules PUBLIC ${RVC_INC})
    target_compile_options(${name}_modules PRIVATE ${RVC_C_WARNINGS})
    if(RVC_COVERAGE)
        target_compile_options(${name}_modules PRIVATE --coverage -O0 -g)
    endif()

    set(dbl_srcs "")
    foreach(d IN LISTS ARG_DOUBLES)
        list(APPEND dbl_srcs ${CMAKE_SOURCE_DIR}/tests/doubles/${d}.c)
    endforeach()

    add_executable(${name} ${CMAKE_SOURCE_DIR}/tests/${name}.cpp ${dbl_srcs})
    target_include_directories(${name} PRIVATE ${RVC_INC} ${CMAKE_SOURCE_DIR}/tests/doubles)
    target_link_libraries(${name} PRIVATE ${name}_modules GTest::gtest_main)
    if(RVC_COVERAGE)
        target_link_options(${name} PRIVATE --coverage)
    endif()
    gtest_discover_tests(${name} DISCOVERY_MODE PRE_TEST)
endfunction()

rvc_add_unit_test(test_afferent
    MODULES left_sensor right_sensor dust_sensor front_sensor
            obstacle_location dust_existence detection
    DOUBLES fake_port)

rvc_add_unit_test(test_interfaces
    MODULES motor_interface cleaner_interface
    DOUBLES fake_port)

rvc_add_unit_test(test_actions
    MODULES move_forward move_backward turn_left turn_right
            clean_normal clean_off power_up_clean
    DOUBLES fake_interfaces)

rvc_add_unit_test(test_avoidance
    MODULES avoidance)

rvc_add_unit_test(test_controller
    MODULES controller avoidance
    DOUBLES fake_stores fake_actions)

rvc_add_unit_test(test_system
    MODULES rvc detection controller avoidance
            left_sensor right_sensor dust_sensor front_sensor
            obstacle_location dust_existence
            move_forward move_backward turn_left turn_right
            clean_normal clean_off power_up_clean
            motor_interface cleaner_interface
    DOUBLES fake_port)

# ---------------------------------------------------------------------------
# CH-01: caller-callee relationships match the approved Structure Chart.
# ---------------------------------------------------------------------------
find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
    add_test(NAME CallHierarchy.CH01_MatchesStructureChart
             COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/check_call_hierarchy.py
                     ${RVC_SRC})
endif()
