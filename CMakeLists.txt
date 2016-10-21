cmake_minimum_required(VERSION 3.2)

project(propertypp LANGUAGES CXX)

include(CheckCSourceCompiles)
include(CheckCCompilerFlag)
include(CheckCXXSourceCompiles)
include(CheckCXXSymbolExists)
include(CheckCXXCompilerFlag)
include(CheckFunctionExists)
include(CheckTypeSize)
include(CheckIncludeFiles)
include(CheckIncludeFileCXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

check_include_file_cxx(iostream HAVE_IOSTREAM)
check_include_file_cxx(string HAVE_STRING)
check_include_file_cxx(exception HAVE_EXCEPTION)
check_include_file_cxx(stdexcept HAVE_STDEXCEPT)
check_include_file_cxx(memory HAVE_MEMORY)

set(EXPORT_INCLUDES
	${CMAKE_CURRENT_SOURCE_DIR}/include/export
)

add_library(
	${PROJECT_NAME} STATIC

	${CMAKE_CURRENT_SOURCE_DIR}/src/property.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/sqlite.cpp

	${EXPORT_INCLUDES}/propertypp/property.hpp
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	${EXPORT_INCLUDES}
	${SQLITE3_INCLUDE_DIRS}
)

target_link_libraries(
	${PROJECT_NAME}
	${SQLITE3_LIBRARIES}
)
