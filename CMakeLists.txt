cmake_minimum_required (VERSION 3.8)

project (libva)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME}
	INTERFACE
		${PROJECT_SOURCE_DIR}/include
)

option(PACKAGE_TESTS "Build the tests" ON)
if (PACKAGE_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()