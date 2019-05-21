cmake_minimum_required (VERSION 3.8)

project (libva)

set(CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} INTERFACE)

option(PACKAGE_TESTS "Build the tests" ON)
if (PACKAGE_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()