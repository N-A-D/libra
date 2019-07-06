cmake_minimum_required (VERSION 3.8)

project (libra)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME}
	INTERFACE
		${PROJECT_SOURCE_DIR}/include
)

option(BUILD_LIBRA_TESTS "Build the tests" ON)
if (BUILD_LIBVA_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()