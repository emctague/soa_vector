cmake_minimum_required(VERSION 4.1)
project(soa_vector)

set(CMAKE_CXX_STANDARD 23)

find_package(Catch2 3 REQUIRED)

add_executable(run_tests tests.cpp soa_vector.h)
target_link_libraries(run_tests PRIVATE Catch2::Catch2WithMain)
include(Catch)
catch_discover_tests(run_tests)
